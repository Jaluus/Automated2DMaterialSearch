"""
A collection of helper functions to stitch a collection of images together
"""
import cv2
import numpy as np
import os
import matplotlib.pyplot as plt
from skimage import measure
import sys

from Utils.etc_functions import *


def compress_images(
    picture_directory: str,
    compressed_directory_name: str = "Compressed",
    factor: int = 4,
    quality: int = 80,
):
    """
    takes the absolut path of the picture_directory and creates a new Folder which holds all the compressed images.\n
    Images have the same name.\n
    returns the compressed image dir path
    """

    img_paths = os.listdir(picture_directory)
    num_pics = len(img_paths)
    upper_dir = os.path.dirname(picture_directory)

    new_compressed_directory = os.path.join(upper_dir, compressed_directory_name)
    # Create the new folder, if it does not exist
    if not os.path.exists(new_compressed_directory):
        os.makedirs(new_compressed_directory)

    # Getting the old dimensions
    img = cv2.imread(os.path.join(picture_directory, img_paths[0]))
    height = img.shape[1]
    width = img.shape[0]

    # The new dimensions
    new_width = int(width / factor)
    new_height = int(height / factor)

    for idx, img_path in enumerate(img_paths):

        print(
            f"\rCurrent Image: {idx+1} / {num_pics}\r",
            end="",
            flush=True,
        )

        single_img_path = os.path.join(picture_directory, img_path)
        img = cv2.imread(single_img_path)

        small_img = cv2.resize(img, (new_height, new_width))

        # extracte the raw image name without .png "
        new_file_name = ".".join(img_path.split(".")[:-1])

        new_img_path = os.path.join(new_compressed_directory, f"{new_file_name}.jpg")

        # Write the image with a given quality
        cv2.imwrite(new_img_path, small_img, [int(cv2.IMWRITE_JPEG_QUALITY), quality])

    print("")
    return new_compressed_directory


def stitch_image(
    picture_directory: str,
    stitched_image_path=None,
    x_rows: int = 21,
    y_rows: int = 31,
    x_pix_offset: int = 403,
    y_pix_offset: int = 273,
):
    """
    Stitches images together with the given pixel offsets and given number of rows and columns.\n
    Default Params are for 2.5x Magnificication and 5mm x-movement and 3.3333 mm y-movement\n
    Returns a stitched image and path\n
    TODO: infer rows and colums from directory
    """

    # getting all the pictures in the directory sorted!
    pic_files = sorted_alphanumeric(os.listdir(picture_directory))

    full_pic_arr_y = [None] * x_rows
    full_pic = None

    for i in range(x_rows):
        for j in range(y_rows):
            curr_idx = i * y_rows + j
            full_path = os.path.join(picture_directory, pic_files[curr_idx])
            img = cv2.imread(full_path)

            if full_pic_arr_y[i] is None:
                full_pic_arr_y[i] = img[:y_pix_offset, :, :].copy()
            else:
                full_pic_arr_y[i] = np.concatenate(
                    (full_pic_arr_y[i], img[:y_pix_offset, :, :]),
                    axis=0,
                )

        if full_pic is None:
            full_pic = full_pic_arr_y[i][:, :x_pix_offset, :].copy()
        else:
            full_pic = np.concatenate(
                (full_pic, full_pic_arr_y[i][:, :x_pix_offset, :]),
                axis=1,
            )

    if stitched_image_path is not None:
        try:
            cv2.imwrite(stitched_image_path, full_pic)
        except OSError as e:
            print(e)

    return full_pic


def create_mask_from_stitched_image(
    stitched_image_path: str,
    mask_path: str = "none",
    threshold_value: int = 45,
    blur_kernel: int = 11,
    blur_strength: int = 100,
):
    """
    Creates a mask form a given stitched image
    """

    stitched_image = cv2.imread(stitched_image_path)

    stitched_image_grey = cv2.cvtColor(stitched_image, cv2.COLOR_BGR2GRAY)

    stitched_image_grey = cv2.GaussianBlur(
        stitched_image_grey, (blur_kernel, blur_kernel), blur_strength
    )

    ret, mask = cv2.threshold(
        stitched_image_grey, threshold_value, 255, cv2.THRESH_BINARY_INV
    )

    mask = cv2.erode(mask, np.ones((5, 5)), iterations=4)
    mask = cv2.dilate(mask, np.ones((5, 5)), iterations=4)

    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, np.ones((21, 21)), iterations=5)

    if mask_path != "none":
        try:
            cv2.imwrite(mask_path, mask)
        except OSError as e:
            print(e)

    return mask


def create_scan_area_map_from_mask(
    mask_path: str,
    scan_area_map_path: str = None,
    view_field_x: float = 0.7380,
    view_field_y: float = 0.4613,
    percentage_threshold: float = 0.9,
    erode_iterations: int = 0,
):
    """
    Creates a Labeled Scan Area Map and returns it

    Args:
        mask_path (str): The path to the saved black and white mask
        scan_area_map_path (str, optional): The optional path of where to save the Labeled scan area map. Defaults to None.
        view_field_x (float, optional): the x View Field of the 20x in mm. Defaults to 0.7380.
        view_field_y (float, optional): the y View Field of the 20x in mm. Defaults to 0.4613.
        percentage_threshold (float,optional): The threshold for when a part of the map should still be considered as a part of the flake. Defaults to 0.9.
        erode_iter (int, optional): How often to erode the Mask to remove edges. Defaults to 0.

    Returns:
        labeled_scan_area (NxMx1 Array) : The scan area map
    """

    # Load the mask
    mask = cv2.imread(mask_path, 0)

    # our area map has a resolution of 8463 pixels / 100 mm
    pixel_resolution_y = mask.shape[0] / 100
    pixel_resolution_x = mask.shape[1] / 100

    # Real 20x area (0.7380 x 0.4613)
    ## (Unused) offset area (0.6666 x 0.4)
    x_frame = int(pixel_resolution_x * view_field_x)
    y_frame = int(pixel_resolution_y * view_field_y)

    # Create a new mask where we draw on

    width = mask.shape[0]
    height = mask.shape[1]

    # Create an array which saves the points where we can raster to
    scan_area = np.zeros((int(width / y_frame) + 1, int(height / x_frame) + 1))

    i = 0
    while int(i * x_frame) < height:
        j = 0
        while int(j * y_frame) < width:

            # Crop to the part of the Image which would be seen by the 20x scope
            x_start = i * x_frame
            y_start = j * y_frame
            x_end = (i + 1) * x_frame
            y_end = (j + 1) * y_frame
            crop_arr = mask[y_start:y_end, x_start:x_end]

            non_zero_pixels = cv2.countNonZero(crop_arr)

            # find the percentage of non background pixels
            percentage_non_background = non_zero_pixels / (
                crop_arr.shape[0] * crop_arr.shape[1]
            )

            # Save the Image only if a certain percantage of the image is not background
            if percentage_non_background >= percentage_threshold:
                scan_area[j, i] = 1

            j += 1
        i += 1

    # Small adjustments
    scan_area = cv2.erode(scan_area, np.ones((3, 3)), iterations=1 + erode_iterations)
    scan_area = cv2.dilate(scan_area, np.ones((3, 3)), iterations=1)

    # find each chip in the image
    labeled_scan_area = measure.label(scan_area.copy())

    if scan_area_map_path is not None:
        try:
            cv2.imwrite(scan_area_map_path, labeled_scan_area)
        except OSError as e:
            print(e)
            print("Your path seems to be wrong")

    return labeled_scan_area


if __name__ == "__main__":
    pass
