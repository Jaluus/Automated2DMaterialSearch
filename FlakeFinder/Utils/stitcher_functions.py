"""
A collection of helper functions to stitch a collection of images together
"""
import cv2
import numpy as np
import os
import shutil
import matplotlib.pyplot as plt
from skimage import measure
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), "..", ".."))
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
    stitched_image_path: str = "none",
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

    if stitched_image_path != "none":
        try:
            cv2.imwrite(stitched_image_path, full_pic)
        except OSError as e:
            print(e)

    return full_pic


def create_mask_from_stitched_image(
    stitched_image_path: str,
    mask_path: str = "none",
    threshold_value: int = 80,
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


def create_area_scan_map_from_mask(
    mask_path: str,
    scan_area_map_path: str = "none",
    view_field_x: float = 0.7380,
    view_field_y: float = 0.4613,
    erode_iter: int = 1,
):
    """
    Creates a Scan Area Map and returns it\n
    view_field_x is the x-dimension of the 20x Magnification Viewfield\n
    view_field_y is the y-dimension of the 20x Magnification Viewfield\n
    erode is the number of time the mask is being eroded\n
    returns the labeled scna area
    """
    # Load the mask
    mask = cv2.imread(mask_path)

    # our area map has a resolution of 8401 pixels / 100 mm
    pixel_resolution_y = mask.shape[0] / 100
    pixel_resolution_x = mask.shape[1] / 100

    # Real 20x area (0.7380 x 0.4613)
    ## (Later) Used offset area (0.6666 x 0.4)
    x_frame = int(pixel_resolution_x * view_field_x)
    y_frame = int(pixel_resolution_y * view_field_y)

    # Create a new mask where we draw on
    new_mask = mask.copy()

    width = mask.shape[0]
    height = mask.shape[1]

    # Create an array which saves the points where we can raster to
    scan_area = np.zeros((int(width / y_frame) + 1, int(height / x_frame) + 1))

    i = 0
    while int(i * x_frame) < height:
        j = 0
        while int(j * y_frame) < width:

            # crop a part of the full image and look at the values
            x_start = i * x_frame
            y_start = j * y_frame
            x_end = (i + 1) * x_frame
            y_end = (j + 1) * y_frame
            crop_arr = mask[y_start:y_end, x_start:x_end]

            if np.all(crop_arr):
                scan_area[j, i] = 1

            j += 1
        i += 1

    scan_area = cv2.erode(scan_area, np.ones((3, 3)), iterations=erode_iter)

    # find each chip in the image
    labeled_scan_area = measure.label(scan_area.copy())

    if scan_area_map_path != "none":
        try:
            cv2.imwrite(scan_area_map_path, labeled_scan_area)
        except OSError as e:
            print(e)

    return labeled_scan_area


if __name__ == "__main__":
    scan_dir = r"C:\Users\duden\Desktop\Mikroskop Bilder\WSe2\FullScanD"

    picture_dir = os.path.join(scan_dir, "2.5x", "Pictures")
    compressed_dir = os.path.join(scan_dir, "2.5x", "Compressed")
    stitched_path = os.path.join(scan_dir, "stiched_image.png")
    mask_path = os.path.join(scan_dir, "mask.png")
    scan_area_path = os.path.join(scan_dir, "scan_area.png")

    compress_images(picture_dir)

    stitch_image(
        compressed_dir,
        stitched_path,
    )

    create_mask_from_stitched_image(
        stitched_path,
        mask_path,
    )
