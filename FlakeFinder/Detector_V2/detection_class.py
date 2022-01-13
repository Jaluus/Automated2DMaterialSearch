import cv2
import numpy as np
from numba import jit
from skimage import measure
from skimage.filters.rank import entropy
from skimage.morphology import disk
import copy
import matplotlib.pyplot as plt

from Detector.detector_functions import remove_vignette


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
        custom_background_values=None,
        size_threshold: int = 0,
        entropy_threshold: float = np.inf,
        sigma_treshold: float = np.inf,
        magnification: float = 20,
    ):
        """Create the Detection Class

        Args:
            contrast_dict (dict): A Dictionary with the Keys "layers" and "color_radius"
            flat_field (NxMx1 Array, optional): The background Image, if none is given doesnt correct the Vignette, HIGHLY RECOMMENDED. Defaults to None.
            size_treshold (int, optional): The minimum size of a detected Flake, in nm. Defaults to 0.
            entropy_threshold (float, optional): The maxmimum Entropy of a detected Flake, good values are about 2.4. Defaults to Infinity.
            sigma_treshold (float, optional): The maximum Sigma, aka the proximity values of a detected Flake good values aber abot 30 to 50. Defaults to Infinity.
            magnification (float, optional): The Magnification of the Camera, used to calculate the size of the flakes. Defaults to 20.
        """
        # these are some parameters for the calculation of flake size
        # Need to change this later
        self.micrometer_per_pixel = {
            2.5: 3.0754,
            5: 1.5377,
            20: 0.3844,
            50: 0.1538,
            100: 0.0769,
        }

        if flat_field is not None:
            self.flat_field = flat_field.copy()
        else:
            self.flat_field = None

        if custom_background_values is not None:
            self.custom_background_values = np.array(custom_background_values.copy())
        else:
            self.custom_background_values = None

        self.contrast_dict = copy.deepcopy(contrast_dict)
        self.searched_layers = self.contrast_dict.keys()
        self.size_thresh = size_threshold
        self.entropy_thresh = entropy_threshold
        self.sigma_thresh = sigma_treshold
        self.magnification = magnification

        # detection parameters
        self.sharpen_kernel = np.array([[-1, -1, -1], [-1, 9, -1], [-1, -1, -1]])
        self.upper_thresh_canny = 100
        self.lower_thresh_canny = 50
        self.median_blur_kernel_size = 7
        self.min_candidate_size = 200

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

    def mask_background(self, img, radius=7):
        """
        Maskes the Background\n
        The Values are standard values of 90nm SiO Chips with removed Vignette\n
        returns the masked background, as well as the mask\n
        """
        img_r = img[:, :, 2]
        img_g = img[:, :, 1]
        img_b = img[:, :, 0]

        # A threshold which removes the Unwanted background of the non chip
        # Currently Unused

        hist_r = cv2.calcHist([img_r], [0], None, [256], [0, 256])
        hist_g = cv2.calcHist([img_g], [0], None, [256], [0, 256])
        hist_b = cv2.calcHist([img_b], [0], None, [256], [0, 256])

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
        image: np.ndarray,
        mean_background_values: np.ndarray,
    ):
        """
        calculates the contrasts of the image with the given mean background values\n
        These need to be given in the order of BGR and as a numpy array\n
        Implemented with Numba for better performance\n
        Returned in B G R
        """
        contrasts = image / mean_background_values - 1
        return contrasts

    def mask_contrasted_image_rectangle(
        self,
        contrasts,
        current_layer,
    ):

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

        # finding the intersection of the Masks (mask_a ∩ mask_b ∩ mask_c)
        mask = cv2.bitwise_and(contrast_r_threshed, contrast_g_threshed)
        mask = cv2.bitwise_and(mask, contrast_b_threshed)
        return mask

    @staticmethod
    @jit(nopython=True)
    def mask_contrasted_image_new_inner(
        contrasts,
        contrast_r,
        contrast_g,
        contrast_b,
        radius_r,
        radius_g,
        radius_b,
    ):
        mask = np.zeros(shape=(contrasts.shape[0], contrasts.shape[1]), dtype=np.uint8)
        for i in range(mask.shape[0]):
            for j in range(mask.shape[1]):
                pixel = contrasts[i, j]

                dist_b = ((pixel[0] - contrast_b) / radius_b) ** 2
                dist_g = ((pixel[1] - contrast_g) / radius_g) ** 2
                dist_r = ((pixel[2] - contrast_r) / radius_r) ** 2

                if (dist_b + dist_g + dist_r) <= 1:
                    mask[i, j] = 255

        return mask

    def mask_contrasted_image_ellipsoid(
        self,
        contrasts: np.ndarray,
        current_layer: dict,
    ):
        """Findes contrasts of pixels within a ellipsoid around the given contrast values

        Args:
            contrasts (XxYx3 Array): The contrast of each pixel, in BGR
            current_layer (Dict): A Dict with the Keys "contrast" and "color_radius", each with a Dict with the Keys "r", "g", "b"

        Returns:
            XxY Array : a mask of pixels with the threshold
        """
        # for more information
        # https://math.stackexchange.com/questions/76457/check-if-a-point-is-within-an-ellipse

        ## we had to write this wrapper function to not give jit the dict, it cant handle dicts
        mask = detector_class.mask_contrasted_image_new_inner(
            contrasts,
            current_layer["contrast"]["r"],
            current_layer["contrast"]["g"],
            current_layer["contrast"]["b"],
            current_layer["color_radius"]["r"],
            current_layer["color_radius"]["g"],
            current_layer["color_radius"]["b"],
        )

        return mask

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
        num_pixels = {}
        detected_flakes = []
        MICROMETER_PER_PIXEL = self.micrometer_per_pixel[self.magnification]

        # Removing the Vignette from the Image
        if self.flat_field is not None:
            image = remove_vignette(
                image,
                self.flat_field,
            )

        # Conversion to the right format, internaly im working with Pixel thresholds
        # The Conversion in the 20x scope is 1 px = 0.15 μm²
        pixel_threshold = self.size_thresh // (MICROMETER_PER_PIXEL ** 2)

        # Check if we already supplied pre-calculated background values
        if self.custom_background_values is None:
            # get the Background of the image ~15 ms
            image_background, background_mask = self.mask_background(image)

            # find the mean b g r values of the background ~2 ms
            mean_background_values = cv2.mean(
                image_background,
                mask=background_mask,
            )[:-1]
        else:
            mean_background_values = self.custom_background_values

        image_sharpened = cv2.filter2D(image, -1, self.sharpen_kernel)
        image_preprocessed = cv2.medianBlur(
            image_sharpened, self.median_blur_kernel_size
        )

        edges = cv2.Canny(image_preprocessed, 50, 150)
        edges = cv2.dilate(edges, None, iterations=1)

        # find contours
        contours, hierarchy = cv2.findContours(
            edges, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE
        )

        # generate flake candidates
        candidates = [
            contours[i]
            for i in range(len(contours))
            if hierarchy[0, i, 3] != -1
            and hierarchy[0, hierarchy[0, i, 3], 3] == -1
            and cv2.contourArea(contours[i]) > pixel_threshold
        ]

        for candidate in candidates:

            # get the mask of the candidate
            candidate_mask = np.zeros(image.shape[:-1], dtype=np.uint8)
            cv2.drawContours(candidate_mask, [candidate], -1, 255, -1)

            # Extract the mean color of the candidate
            color = cv2.mean(image_preprocessed, candidate_mask)[:-1]
            contrast = color / mean_background_values - 1

            # check if the contrast is within any of the ellipsoids
            # go through all the layers we selected
            for layer_name in self.searched_layers:

                # Set the current layer by getting the value using the key 'layer'
                current_layer = self.contrast_dict[layer_name]

                # define variables for later use and easier reading
                layer_contrast_array = np.array(
                    [
                        current_layer["contrast"]["b"],
                        current_layer["contrast"]["g"],
                        current_layer["contrast"]["r"],
                    ]
                )
                layer_color_radius_array = np.array(
                    [
                        current_layer["color_radius"]["b"],
                        current_layer["color_radius"]["g"],
                        current_layer["color_radius"]["r"],
                    ]
                )

                # check if the contrast is within the ellipsoid
                dist = np.linalg.norm(
                    (contrast - layer_contrast_array) / layer_color_radius_array
                )

                # if the contrast is outside the ellipsoid, skip this candidate
                # Else we might have a flake!
                if dist > 1:
                    continue

                # now we characterize the Flake
                # You can do a lot of stuff here, like filter for Entropy or other metrics
                # im just gonna call it a day for now and just do basic stuff
                #################

                #### Calculate the Contrast of the Flakes
                mean_contrast = list(contrast)
                stddev_contrast = [0, 0, 0]

                #### count the number of pixels belonging to the flake
                flake_pixel_number = cv2.countNonZero(candidate_mask)

                #### Calculate a Bounding Box
                x, y, w, h = cv2.boundingRect(candidate)

                # add this later
                ((center_x, center_y), (width_r, height_r), rotation) = cv2.minAreaRect(
                    candidate
                )

                aspect_ratio = float(w) / h

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
                cut_out_flake_mask = candidate_mask[
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
                proximity_mask = cv2.dilate(candidate_mask, disk(7), iterations=6)

                # dilate the Flake mask to remove the edges
                dilated_masked_flake = cv2.dilate(candidate_mask, disk(4), iterations=4)

                # Xor the mask to only get the surroundings
                proximity_mask = cv2.bitwise_xor(dilated_masked_flake, proximity_mask)

                _, proximity_stddev = cv2.meanStdDev(
                    cv2.cvtColor(image, cv2.COLOR_BGR2GRAY),
                    mask=proximity_mask,
                )
                #################
                flake_dict = {
                    "mask": candidate_mask,
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

        # Finally, return all the detected flakes
        return np.array(detected_flakes)
