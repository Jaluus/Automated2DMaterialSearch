import copy

import cv2
import numpy as np
from numba import jit
from skimage import measure
from skimage.filters.rank import entropy
from skimage.morphology import disk
import time


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
        custom_background_values=None,
        size_threshold: int = 0,
        entropy_threshold: float = np.inf,
        sigma_treshold: float = np.inf,
        magnification: float = 20,
        standard_deviation_threshold: float = 2,
        used_channels: list = [0, 1, 2],
        covariance_scaling_factors: list = [1, 1, 1],
    ):
        """Create the Detection Class

        Args:
            contrast_dict (dict): A Dictionary with the Keys "layers" and "color_radius"
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
        self.standard_deviation_threshold = standard_deviation_threshold

        # Conversion to the right format, internaly im working with Pixel thresholds
        # The Conversion in the 20x scope is 1 px = 0.15 μm²
        self.pixel_threshold = self.size_thresh // (
            self.micrometer_per_pixel[self.magnification] ** 2
        )

        # define the used channels
        self.used_channels = used_channels

        # add some more keys to the contrast_dict
        # the inverse of the cholesky decomposition of the covariance matrix in order to speed up the calculation of the distance
        # and the mean of the contrast to make it easier to handle
        # also handle the values of mean etc.. internally differently

        scaling_matrix = np.sqrt(np.diag(covariance_scaling_factors))

        self.means = []
        self.inv_cholesky_matrices = []
        self.layer_indexes = {}

        for idx, layer in enumerate(self.contrast_dict.keys()):

            # scaling the covariance matrix for example, stretch the blue dimension
            covariance_matrix = np.array(self.contrast_dict[layer]["covariance_matrix"])
            covariance_matrix = scaling_matrix @ covariance_matrix @ scaling_matrix

            contrast_mean = np.array(
                [
                    self.contrast_dict[layer]["contrast"]["b"],
                    self.contrast_dict[layer]["contrast"]["g"],
                    self.contrast_dict[layer]["contrast"]["r"],
                ]
            )
            cholesky = np.linalg.inv(np.linalg.cholesky(covariance_matrix))

            self.contrast_dict[layer]["inv_cholesky_decomposition"] = cholesky
            self.contrast_dict[layer]["contrast_mean"] = contrast_mean

            self.means.append(contrast_mean)
            self.inv_cholesky_matrices.append(cholesky)
            self.layer_indexes[layer] = idx + 1

        self.means = np.array(self.means)
        self.inv_cholesky_matrices = np.array(self.inv_cholesky_matrices)

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

    def mask_background(self, img, radius=4):
        """
        Maskes the Background\n
        The Values are standard values of 90nm SiO Chips with removed Vignette\n
        returns the masked background, as well as the mask\n
        """
        masks = []

        for i in range(3):
            img_channel = img[:, :, i]

            # A threshold which removes the Unwanted background of the non chip
            # Currently Unused

            hist_channel = cv2.calcHist([img_channel], [0], None, [256], [0, 256])

            hist_max_channel = np.argmax(hist_channel)

            threshed_channel = cv2.inRange(
                img_channel,
                int(hist_max_channel - radius),
                int(hist_max_channel + radius),
            )
            background_mask_channel = cv2.erode(threshed_channel, np.ones((3, 3)))
            masks.append(background_mask_channel)

        final_mask = cv2.bitwise_and(masks[0], masks[1])
        final_mask = cv2.bitwise_and(final_mask, masks[2])

        masked_image = cv2.bitwise_and(img, img, mask=final_mask)

        return masked_image, final_mask

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

    @staticmethod
    @jit(nopython=True)
    def mask_contrasted_image_ellipsoid(
        contrasts,
        mean,
        inv_cholesky,
        standard_deviations,
    ):
        max_dist = standard_deviations ** 2
        dist = 0
        tmp = 0
        mask = np.zeros(shape=(contrasts.shape[0], contrasts.shape[1]), dtype=np.uint8)
        for i in range(mask.shape[0]):
            for j in range(mask.shape[1]):
                # calculate the Mahalanobis distance by utilizing the speedup of the inverse Cholesky decomposition
                ###################
                # tmp_1 = inv_cholesky[0, 0] * diff[0]
                # tmp_2 = inv_cholesky[1, 0] * diff[0] + inv_cholesky[1, 1] * diff[1]
                # tmp_3 = inv_cholesky[2, 0] * diff[0] + inv_cholesky[2, 1] * diff[1] + inv_cholesky[2, 2] * diff[2]
                # dist = tmp_1 **2 + tmp_2 **2 + tmp_3 **2
                ###################

                # init the distance to 0
                dist = 0

                # assume the pixel is part of the ellipsoid
                mask[i, j] = 255

                # run this loop for each channel
                for k in range(contrasts.shape[2]):

                    # calculate the distance for the current channel
                    tmp = 0
                    for h in range(k + 1):
                        tmp += (mean[h] - contrasts[i, j][h]) * inv_cholesky[k, h]
                    dist += tmp ** 2

                    # if the distance is bigger then the max distance, stop
                    # we dont need to calculate the rest of the distances
                    # as we already know that the pixel is not part of the ellipsoid
                    if dist > max_dist:
                        mask[i, j] = 0
                        break
        return mask

    @staticmethod
    @jit(nopython=True)
    def mask_contrasted_image_ellipsoid_MULTI(
        contrasts,
        means,
        inv_choleskys,
        standard_deviations,
    ):
        max_dist = standard_deviations ** 2
        tmp = 0
        smallest_distance = -1
        current_closest_layer = 0
        mask = np.zeros(shape=(contrasts.shape[0], contrasts.shape[1]), dtype=np.uint8)
        for i in range(mask.shape[0]):
            for j in range(mask.shape[1]):
                # calculate the Mahalanobis distance by utilizing the speedup of the inverse Cholesky decomposition
                ###################
                # tmp_1 = inv_cholesky[0, 0] * diff[0]
                # tmp_2 = inv_cholesky[1, 0] * diff[0] + inv_cholesky[1, 1] * diff[1]
                # tmp_3 = inv_cholesky[2, 0] * diff[0] + inv_cholesky[2, 1] * diff[1] + inv_cholesky[2, 2] * diff[2]
                # dist = tmp_1 **2 + tmp_2 **2 + tmp_3 **2
                ###################

                # use -1 as a standin for infinity
                smallest_distance = -1
                current_closest_layer = 0

                # calculate the distance for each thickness and select the one with the smallest distance
                for thickness_index in range(means.shape[0]):

                    # init the distance to 0
                    distance_from_ellipsoid = 0

                    # run this loop to calculate the distance
                    for k in range(contrasts.shape[2]):
                        # calculate the distance for the current channel
                        tmp = 0
                        for h in range(k + 1):
                            tmp += (
                                means[thickness_index, h] - contrasts[i, j, h]
                            ) * inv_choleskys[thickness_index, k, h]
                        distance_from_ellipsoid += tmp ** 2

                    # check if the smallest distance was already set
                    # if not set it
                    # this is dont to circumvent the use of infinity
                    if smallest_distance == -1 and distance_from_ellipsoid < max_dist:
                        smallest_distance = distance_from_ellipsoid
                        current_closest_layer = thickness_index + 1
                        continue

                    # Set the current layer as the closest layer if the distance is smaller than the current smallest distance
                    if (
                        distance_from_ellipsoid < max_dist
                        and distance_from_ellipsoid < smallest_distance
                    ):
                        smallest_distance = distance_from_ellipsoid
                        current_closest_layer = thickness_index + 1

                mask[i, j] = current_closest_layer

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
        detected_flakes = []

        # Removing some noise by Gaussian Filtering
        # image = cv2.GaussianBlur(image, (9, 9), 2)
        image = cv2.medianBlur(image, 5)

        # Check if we already supplied pre-calculated background values
        if self.custom_background_values is None:
            # get the Background of the image ~15 ms
            image_background, background_mask = self.mask_background(image)

            # find the mean b g r values of the background ~2 ms
            mean_background_values = np.array(
                cv2.mean(
                    image_background,
                    mask=background_mask,
                )[:-1],
                dtype=np.uint8,
            )
        else:
            mean_background_values = self.custom_background_values

        # calculate the contrast ~50ms with numba
        contrasts = self.calc_contrast(image, mean_background_values)

        full_layer_mask = detector_class.mask_contrasted_image_ellipsoid_MULTI(
            contrasts[:, :, self.used_channels],
            self.means[:, self.used_channels],
            self.inv_cholesky_matrices[:, self.used_channels, :][
                :, :, self.used_channels
            ],
            self.standard_deviation_threshold,
        )
        full_layer_mask = cv2.morphologyEx(full_layer_mask, cv2.MORPH_OPEN, disk(2))

        # iterate over all Masks and Label them
        for layer_name in self.searched_layers:
            # get the Mask for the current layer
            layer_index = self.layer_indexes[layer_name]
            layer_mask = cv2.inRange(full_layer_mask, layer_index, layer_index)

            if cv2.countNonZero(layer_mask) < self.pixel_threshold:
                continue

            # label each connected 'blob' on the mask with an individual number
            num_labels, labeled_mask = cv2.connectedComponents(
                layer_mask, connectivity=4
            )

            # iterate over all flake, values, 0 is the Background, dont look at that
            for i in range(1, num_labels + 1):

                # mask out only the pixels of certain flakes, quite fast ~1ms
                masked_flake = cv2.inRange(labeled_mask, i, i)
                flake_pixel_number = cv2.countNonZero(masked_flake)

                # if the flake has less pixel than the Threshold take the next one
                if flake_pixel_number < self.pixel_threshold:
                    continue

                # extract the toplevel contour
                contours, hierarchy = cv2.findContours(
                    image=masked_flake,
                    mode=cv2.RETR_TREE,
                    method=cv2.CHAIN_APPROX_NONE,
                )

                top_level_contour = [
                    contours[i]
                    for i in range(len(contours))
                    if hierarchy[0, i, 3] == -1
                ]

                # Fill all the holes in the contour
                masked_flake = cv2.drawContours(
                    masked_flake, top_level_contour, -1, 255, -1
                )

                # now we characterize the Flake
                #################

                #### Calculate the Contrast of the Flakes
                mean_contrast, stddev_contrast = cv2.meanStdDev(
                    contrasts, mask=masked_flake
                )
                mean_contrast = mean_contrast[:, 0].tolist()
                stddev_contrast = stddev_contrast[:, 0].tolist()

                #### Skip the flake if the convex hull is way bigger than the normal contour
                convex_hull = cv2.convexHull(top_level_contour[0])
                convex_hull_area = cv2.contourArea(convex_hull)
                contour_area = cv2.contourArea(top_level_contour[0])

                if convex_hull_area > 5 * contour_area:
                    continue

                #### Calculate a Bounding Box
                x, y, w, h = cv2.boundingRect(top_level_contour[0])

                # add this later
                ((center_x, center_y), (width_r, height_r), rotation) = cv2.minAreaRect(
                    top_level_contour[0]
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
                    cut_out_flake_mask, disk(1), iterations=1
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
                flake_entropy = cv2.mean(
                    entropied_image_area,
                    mask=cut_out_flake_mask,
                )[0]

                # Filter High entropy Flakes, aka dirt
                # if flake_entropy > self.entropy_thresh:
                #    continue

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
                        flake_pixel_number
                        * (self.micrometer_per_pixel[self.magnification] ** 2),
                        1,
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
                        round(
                            center_x * self.micrometer_per_pixel[self.magnification], 0
                        ),
                        round(
                            center_y * self.micrometer_per_pixel[self.magnification], 0
                        ),
                    ),
                    "proximity_stddev": round(proximity_stddev[0][0], 2),
                    "entropy": round(flake_entropy, 2),
                }

                detected_flakes.append(flake_dict)

        return np.array(detected_flakes)
