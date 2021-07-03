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

    def mask_background(
        self,
        img,
        r_min: int = 120,
        r_max: int = 160,
        g_min: int = 120,
        g_max: int = 150,
        b_min: int = 150,
        b_max: int = 200,
    ):
        """
        Maskes the Background\n
        The Values are standard values of 90nm SiO Chips with removed Vignette\n
        returns the masked background, as well as the mask\n
        """
        img_blurred = cv2.GaussianBlur(img, (5, 5), 10)
        img_r = img_blurred[:, :, 2]
        img_g = img_blurred[:, :, 1]
        img_b = img_blurred[:, :, 0]

        img_r = cv2.inRange(img_r, r_min, r_max)
        img_g = cv2.inRange(img_g, g_min, g_max)
        img_b = cv2.inRange(img_b, b_min, b_max)

        # We have an invertet mask => Bitwise_and and not or
        mask = cv2.bitwise_and(img_r, img_b)
        mask = cv2.bitwise_and(mask, img_g)

        # Remove the excess edges from the masked Flakes by infalting the Mask
        mask = cv2.erode(mask, disk(2), iterations=4)

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
        size_thresh=200,
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
            - 'num_pixels' (int): Number of Pixels belongig to the Flake
            - 'layer' (String): The Layer Type
            - 'mean_contrast' (Nx3 Array): The mean BGR Contrasts relative to the background
            - 'stddev_contrast' (Nx3 Array): The Stddevs BGR Contrasts relative to the background
            - 'mean_background' (Nx3 Array): The mean BGR background values
            - 'position' (2x1 tuple): The Position of the Flake in mm relative to the upper left hand corner
            - 'width_bbox' (int): The Width of the Bounding Box
            - 'height_bbox' (int): The Height of the Bounding Box
            - 'proximity_stddev' (float): the standartdeviation of the close proximity of the Flake
            - 'entropy' (float): The entropy Value of the Flake
            - 'position_x_micro' (float): The x Middlepoint of the flake on the image
            - 'position_y_micro' (float): The y Middlepoint of the flake on the image
            - 'size_micro' (float): The Size of the Flake in μm²
        """
        # Define some Default Vars
        masks = {}
        num_pixels = {}
        detected_flakes = []

        BLUR_STRENGTH = 3
        GAUSS_KSIZE = (5, 5)
        ENTROPY_THRESHOLD = 2.4
        MICROMETER_PER_PIXEL = 0.3833

        # Removing the Vignette from the Image
        if self.flat_field is not None:
            image = self.remove_vignette(
                image,
                self.flat_field,
            )

        # blur the image a bit
        image_blurred = image  # cv2.GaussianBlur(image, GAUSS_KSIZE, BLUR_STRENGHT)

        # Conversion to the right format, internaly im working with Pixel thresholds
        # The Conversion in the 20x scope is 1 px = 0.15 μm²
        pixel_threshold = size_thresh // (MICROMETER_PER_PIXEL ** 2)

        # get the Background of the image ~15 ms
        image_background, background_mask = self.mask_background(image_blurred)

        # find the mean b g r values of the background ~2 ms
        mean_background_values = cv2.mean(
            image_background,
            mask=background_mask,
        )[:-1]

        # calculate the contrast ~50ms with numba
        contrasts = self.calc_contrast(image_blurred, mean_background_values)

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
                x, y, w, h = cv2.boundingRect(masked_flake)

                #### Calculate the Entropy

                # Expand the Bounding Box
                x_min = max(x - 20, 0)
                x_max = min(x + w + 20, image.shape[1])
                y_min = max(y - 20, 0)
                y_max = min(y + h + 20, image.shape[0])
                # Cut out the Bounding boxes
                entropy_area = image[
                    y_min:y_max,
                    x_min:x_max,
                ]
                entropy_area_mask = masked_flake[
                    y_min:y_max,
                    x_min:x_max,
                ]

                # Erode the mask to not accidentally have the Edges in the mean
                entropy_area_mask = cv2.erode(entropy_area_mask, disk(2), iterations=2)

                # go to Gray as we only need the gray Entropy
                entropy_area_gray = cv2.cvtColor(entropy_area, cv2.COLOR_BGR2GRAY)

                # Do the entropy function, really quit as we only use a small part
                entropied_image_area = entropy(entropy_area_gray, disk(2))

                # Get the Mean entropy of the flake, maybe use a different Metric -> Not so good for big flakes
                flake_entropy = cv2.mean(
                    entropied_image_area,
                    mask=entropy_area_mask,
                )[0]

                # Filter High entropy Flakes, aka dirt
                if flake_entropy > ENTROPY_THRESHOLD:
                    continue

                #### Find the Close Proximity of the Flake

                # Dilate the Flake mask
                proximity_mask = cv2.dilate(masked_flake, disk(7), iterations=6)

                # dilate the Flake mask to remove the edges
                dilated_masked_flake = cv2.dilate(masked_flake, disk(4), iterations=4)

                # Xor the mask to only get the surroundings
                proximity_mask = cv2.bitwise_xor(dilated_masked_flake, proximity_mask)

                _, proximity_stddev = cv2.meanStdDev(
                    cv2.cvtColor(image_blurred, cv2.COLOR_BGR2GRAY),
                    mask=proximity_mask,
                )
                #################
                flake_dict = {
                    "mask": masked_flake,
                    "layer": layer_name,
                    "num_pixels": flake_pixel_number,
                    "size_micro": flake_pixel_number * (MICROMETER_PER_PIXEL ** 2),
                    "mean_contrast": mean_contrast,
                    "stddev_contrast": stddev_contrast,
                    "mean_background": mean_background_values,
                    "position": (x, y),
                    "width_bbox": w,
                    "height_bbox": h,
                    "position_x_micro": (x + w / 2) * MICROMETER_PER_PIXEL,
                    "position_y_micro": (y + h / 2) * MICROMETER_PER_PIXEL,
                    "proximity_stddev": proximity_stddev[0][0],
                    "entropy": flake_entropy,
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
