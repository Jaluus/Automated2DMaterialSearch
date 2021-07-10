import cv2
import numpy as np
from numba import jit
from skimage import measure
from skimage.filters.rank import entropy
from skimage.morphology import disk
import copy
import matplotlib.pyplot as plt


class detector_class:
    """
    A Detection Class

    TODO: Add more Keys to the Contrasts json file, e.g. for background masking rgb values

    Functions:
        - set_searched_layers
        - mask_background
        - calc_contrast
        - remove_vignette
        - detect_flakes

    """

    def __init__(
        self,
        contrast_dict: dict,
        flat_field=None,
        size_threshold=200,
        entropy_threshold=2.4,
    ):
        """Create the Detection Class

        Args:
            contrast_dict (dict): A Dictionary with the Keys "layers" and "color_radius"
            flat_field (NxMx1 Array, optional): The background Image, if none is given doesnt correct the Vignette, HIGHLY RECOMMENDED. Defaults to None.
        """

        self.flat_field = flat_field.copy()

        # make sure not to accidentally fuck up your dict
        self.contrast_dict = copy.deepcopy(contrast_dict)
        self.searched_layers = self.contrast_dict.keys()

        self.size_thresh = size_threshold
        self.entropy_thresh = entropy_threshold

    def set_searched_layers(
        self,
        which_layers: list,
    ):
        """Sets the Currently Searched Layers

        Args:
            which_layers (Nx1 Array, optional): A List/Array of string specifing the keys of the searched Layers e.g. ["monolayer","bilayer"]. Defaults to None, which means all given Layers.

        """
        # Check for layers
        self.searched_layers = which_layers

    def mask_background(self, img, radius=7, blue_bg=80):
        """
        Maskes the Background\n
        The Values are standard values of 90nm SiO Chips with removed Vignette\n
        returns the masked background, as well as the mask\n
        """
        img_r = img[:, :, 2]
        img_g = img[:, :, 1]
        img_b = img[:, :, 0]

        # A threshold which removes the Unwanted background of the non chip
        ret, threshed_b_background = cv2.threshold(
            img_b, blue_bg, 255, cv2.THRESH_BINARY
        )

        hist_r = cv2.calcHist([img_r], [0], threshed_b_background, [256], [0, 256])
        hist_g = cv2.calcHist([img_g], [0], threshed_b_background, [256], [0, 256])
        hist_b = cv2.calcHist([img_b], [0], threshed_b_background, [256], [0, 256])

        hist_max_r = np.argmax(hist_r)
        hist_max_g = np.argmax(hist_g)
        hist_max_b = np.argmax(hist_b)

        threshed_r = cv2.inRange(
            img_r, int(hist_max_r - radius), int(hist_max_r + radius)
        )
        threshed_r = cv2.erode(threshed_r, np.ones((3, 3)))

        threshed_g = cv2.inRange(
            img_g, int(hist_max_g - radius), int(hist_max_g + radius)
        )
        threshed_g = cv2.erode(threshed_g, np.ones((3, 3)))

        threshed_b = cv2.inRange(
            img_b, int(hist_max_b - radius), int(hist_max_b + radius)
        )
        threshed_b = cv2.erode(threshed_b, np.ones((3, 3)))

        mask = cv2.bitwise_and(threshed_r, threshed_g)
        mask = cv2.bitwise_and(mask, threshed_b)

        masked_image = cv2.bitwise_and(img, img, mask=mask)

        return masked_image, mask

    @staticmethod
    @jit(nopython=True)
    def calc_contrast(
        image,
        mean_background_values,
    ):
        """
        calculates the contrasts of the image with the given mean background values\n
        Implemented with Numba for better performance\n
        Returned in B G R
        """
        contrasts = np.zeros_like(image, dtype=np.float32)
        for i in range(3):
            contrasts[:, :, i] = (
                image[:, :, i] - mean_background_values[i]
            ) / mean_background_values[i]
        return contrasts

    def remove_vignette(self, image, flat_field):
        """Removes the Vignette from the Image

        Args:
            image (NxMx3 Array): The Image with the Vignette
            flat_field (NxMx3 Array): the Flat Field in RGB

        Returns:
            (NxMx3 Array): The Image without the Vignette
        """
        # convert to hsv and cast to 16bit, to be able to add more than 255
        image_hsv = np.asarray(cv2.cvtColor(image, cv2.COLOR_BGR2HSV), dtype=np.uint16)
        flat_field_hsv = np.asarray(
            cv2.cvtColor(flat_field, cv2.COLOR_BGR2HSV), dtype=np.uint16
        )

        # get the filter and apply it to the image
        image_hsv[:, :, 2] = (
            image_hsv[:, :, 2]
            / flat_field_hsv[:, :, 2]
            * cv2.mean(flat_field_hsv[:, :, 2])[0]
        )

        # clip it back to 255
        image_hsv[:, :, 2][image_hsv[:, :, 2] > 255] = 255

        # Recast to uint8 as the color depth is 8bit per channel
        image_hsv = np.asarray(image_hsv, dtype=np.uint8)

        # reconvert to bgr
        image_no_vigentte = cv2.cvtColor(image_hsv, cv2.COLOR_HSV2BGR)
        return image_no_vigentte

    def detect_flakes(
        self,
        image,
    ):
        """
        Detects Flakes in the given Image, Expects images without vignette

        TODO: Add the rest of the keys to the Dict Description

        Args:
            image (NxMx3 Array): The Image which the detection is being run on

            size_thresh (int, optional): The Threshold for Detecting a flake on an Image in μm². Defaults to 200.

        Returns:
            (Nx1 Array): an Array with the number of entry corrosponding to the number of Flakes
            [flake1,flake2,flake3,flake4, ...]  Each flake Object is a Dict
            The dict contains the following Keys:
            - 'mask' (NxMx1 Array): A Mask which contains the Flake
            - 'layer' (String): The Layer Type
            - 'num_pixels' (int): Number of Pixels belongig to the Flake
            - 'mean_contrast' (Nx3 Array): The mean BGR Contrasts relative to the background
            - 'stddev_contrast' (Nx3 Array): The Stddevs BGR Contrasts relative to the background
            - 'mean_background' (Nx3 Array): The mean BGR background values
            - 'position_bbox' (2x1 tuple): The Position of the Cardinal Bounding Box in pixels relative to the upper left hand corner
            - 'width_bbox' (int): The Width of the Cardinal Bounding Box
            - 'height_bbox' (int): The Height of the Cardinal Bounding Box
            - 'center_rotated' (2x1 tuple): The Center of the rotated Bounding Box
            - 'width_rotated' (float): The Width of the rotated Bounding Box
            - 'height_rotated' (float): The Height of the rotated Bounding Box
            - 'rotation' (float): The Rotation of the rotated Bounding Box
            - 'aspect_ratio' (float): The Aspect ratio of the Flake
            - 'entropy' (float): The entropy Value of the Flake
            - 'position_micro' (2x1 tuple): The Middlepoint of the flake on the image in mm from the top left
            - 'proximity_stddev' (float): the Standarddeviation of the close proximity of the Flake
            - 'size_micro' (float): The Size of the Flake in μm²
        """
        # Define some Default Vars
        masks = {}
        num_pixels = {}
        detected_flakes = []
        MICROMETER_PER_PIXEL = 0.3833
        BACKGROUND_THRESH = 0.2

        # Removing the Vignette from the Image
        if self.flat_field is not None:
            image = self.remove_vignette(
                image,
                self.flat_field,
            )

        # Conversion to the right format, internaly im working with Pixel thresholds
        # The Conversion in the 20x scope is 1 px = 0.15 μm²
        pixel_threshold = self.size_thresh // (MICROMETER_PER_PIXEL ** 2)

        # get the Background of the image ~15 ms
        image_background, background_mask = self.mask_background(image)

        # Counting the number of background pixels
        # if its to low, we most likely have to much dirt or non chip on the image
        background_pixels = cv2.countNonZero(background_mask)
        if background_pixels / (image.shape[0] * image.shape[1]) < BACKGROUND_THRESH:
            return np.array(detected_flakes)

        # find the mean b g r values of the background ~2 ms
        mean_background_values = cv2.mean(
            image_background,
            mask=background_mask,
        )[:-1]

        # calculate the contrast ~50ms with numba
        contrasts = self.calc_contrast(image, mean_background_values)

        # go through all layers
        for layer_name in self.searched_layers:

            # Set the current layer by getting the value using the key 'layer'
            current_layer = self.contrast_dict[layer_name]

            ## Threshing the contrasted Images only for Mono ~30ms
            contrast_b_threshed = cv2.inRange(
                contrasts[:, :, 0],
                current_layer["contrast"]["b"] - current_layer["color_radius"]["b"],
                current_layer["contrast"]["b"] + current_layer["color_radius"]["b"],
            )
            contrast_g_threshed = cv2.inRange(
                contrasts[:, :, 1],
                current_layer["contrast"]["g"] - current_layer["color_radius"]["g"],
                current_layer["contrast"]["g"] + current_layer["color_radius"]["g"],
            )
            contrast_r_threshed = cv2.inRange(
                contrasts[:, :, 2],
                current_layer["contrast"]["r"] - current_layer["color_radius"]["r"],
                current_layer["contrast"]["r"] + current_layer["color_radius"]["r"],
            )

            #### all this ~4 ms
            # finding the intersection of the Masks (mask_a ∩ mask_b ∩ mask_c)
            mask = cv2.bitwise_and(contrast_r_threshed, contrast_g_threshed)
            mask = cv2.bitwise_and(mask, contrast_b_threshed)

            # a bit of cleanup
            mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, disk(2), iterations=1)
            mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, disk(2), iterations=1)
            # mask = cv2.erode(mask, disk(2), iterations=1)
            # mask = cv2.dilate(mask, disk(2), iterations=1)

            # counting the number of pixels and saving the masks only if it has more Pixels
            num_pixels = cv2.countNonZero(mask)

            if num_pixels > pixel_threshold:
                masks[layer_name] = mask
            #### to here

        # Return empty array when you found no masks
        if len(masks.keys()) == 0:
            return np.array(detected_flakes)  # This array is Empty

        # iterate over all Masks and Label them
        for layer_name in masks.keys():

            # label each connected 'blob' on the mask with an individual number
            labeled_mask = measure.label(
                masks[layer_name],
                background=0,
                connectivity=2,
            )

            # iterate over all flake, values, 0 is the Background, dont look at that
            for i in range(1, np.max(labeled_mask) + 1):

                # mask out only the pixels of certain flakes, quite fast ~1ms
                masked_flake = cv2.inRange(labeled_mask, i, i)
                flake_pixel_number = cv2.countNonZero(masked_flake)

                # if the flake has less pixel than the Threshold take the next one
                if flake_pixel_number < pixel_threshold:
                    continue

                # Start with a bit hole filling if there are some
                # maybe do this later with Countour finding?
                masked_flake = cv2.morphologyEx(
                    masked_flake, cv2.MORPH_CLOSE, disk(4), iterations=3
                )

                # now we characterize the Flake
                # You can do a lot of stuff here, like filter for Entropy or other metrics
                # im just gonna call it a day for now and just do basic stuff
                #################

                #### Calculate the Contrast of the Flakes
                mean_contrast, stddev_contrast = cv2.meanStdDev(
                    contrasts, mask=masked_flake
                )
                mean_contrast = mean_contrast[:, 0].tolist()
                stddev_contrast = stddev_contrast[:, 0].tolist()

                #### Calculate a Bounding Box

                contour, _ = cv2.findContours(
                    image=masked_flake,
                    mode=cv2.RETR_TREE,
                    method=cv2.CHAIN_APPROX_NONE,
                )

                x, y, w, h = cv2.boundingRect(contour[0])

                # add this later
                ((center_x, center_y), (width_r, height_r), rotation) = cv2.minAreaRect(
                    contour[0]
                )

                aspect_ratio = round(max(width_r / height_r, height_r / width_r), 2)

                #### Calculate the Entropy

                # Expand the Bounding Box
                x_min = max(x - 20, 0)
                x_max = min(x + w + 20, image.shape[1])
                y_min = max(y - 20, 0)
                y_max = min(y + h + 20, image.shape[0])
                # Cut out the Bounding boxes
                cut_out_flake = image[
                    y_min:y_max,
                    x_min:x_max,
                ]
                cut_out_flake_mask = masked_flake[
                    y_min:y_max,
                    x_min:x_max,
                ]

                # Erode the mask to not accidentally have the Edges in the mean
                cut_out_flake_mask = cv2.erode(
                    cut_out_flake_mask, disk(2), iterations=2
                )

                # go to Gray as we only need the gray Entropy
                entropy_area_gray = cv2.cvtColor(cut_out_flake, cv2.COLOR_BGR2GRAY)

                # Do the entropy function, really quit as we only use a small part
                entropied_image_area = entropy(
                    entropy_area_gray,
                    selem=disk(2),
                    mask=cut_out_flake_mask,
                )

                # Get the max entropy of the flake, maybe use a different Metric -> Not so good for big flakes
                # flake_entropy = np.max(
                #     entropied_image_area,
                # )
                flake_entropy = cv2.mean(
                    entropied_image_area,
                    mask=cut_out_flake_mask,
                )[0]

                # Filter High entropy Flakes, aka dirt
                if flake_entropy > self.entropy_thresh:
                    continue

                #### Find the Close Proximity of the Flake

                # Dilate the Flake mask
                proximity_mask = cv2.dilate(masked_flake, disk(7), iterations=6)

                # dilate the Flake mask to remove the edges
                dilated_masked_flake = cv2.dilate(masked_flake, disk(4), iterations=4)

                # Xor the mask to only get the surroundings
                proximity_mask = cv2.bitwise_xor(dilated_masked_flake, proximity_mask)

                _, proximity_stddev = cv2.meanStdDev(
                    cv2.cvtColor(image, cv2.COLOR_BGR2GRAY),
                    mask=proximity_mask,
                )
                #################
                flake_dict = {
                    "mask": masked_flake,
                    "layer": layer_name,
                    "num_pixels": flake_pixel_number,
                    "size_micro": round(
                        flake_pixel_number * (MICROMETER_PER_PIXEL ** 2), 1
                    ),
                    "mean_contrast": [round(c, 3) for c in mean_contrast],
                    "stddev_contrast": [round(c, 3) for c in stddev_contrast],
                    "mean_background": [round(c, 3) for c in mean_background_values],
                    "position_bbox": (x, y),
                    "width_bbox": int(w),
                    "height_bbox": int(h),
                    "center_rotated": (int(center_x), int(center_y)),
                    "width_rotated": int(width_r),
                    "height_rotated": int(height_r),
                    "rotation": round(rotation, 3),
                    "aspect_ratio": round(aspect_ratio, 2),
                    "position_micro": (
                        round(center_x * MICROMETER_PER_PIXEL, 0),
                        round(center_y * MICROMETER_PER_PIXEL, 0),
                    ),
                    "proximity_stddev": round(proximity_stddev[0][0], 2),
                    "entropy": round(flake_entropy, 2),
                }

                detected_flakes.append(flake_dict)

        return np.array(detected_flakes)


if __name__ == "__main__":
    import json

    img = cv2.imread("9_112.png")

    ff = cv2.imread("flat_fields/90nm.png")

    img = cv2.GaussianBlur(img, (3, 3), 3)

    with open("json/contrasts.json") as f:
        json_data = json.load(f)

    myDetector = detector_class(json_data)

    arr = myDetector.detect_flakes(img, size_thresh=0)
