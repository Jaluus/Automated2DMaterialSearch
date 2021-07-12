import sys
import os
import cv2
import time
import matplotlib.pyplot as plt
import json

# Custom imports
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import (
    microscope_driver_class,
)
from Drivers.Motor_Driver.tango_class import motor_driver_class
from Utils.etc_functions import calibrate_scope
import Utils.raster_functions as raster
import Utils.stitcher_functions as stitcher

# Constants
IMAGE_DIRECTORY = r"C:\Users\Transfersystem User\Desktop\Mic_bilder"
EXFOLIATED_MATERIAL = "empty"
SCAN_NAME = "Dataset_EmptyChip_120721"
CHIP_THICKNESS = "90nm"
SCAN_USER = "Jan"
EXFOLIATION_METHOD = ""

META_DICT = {
    "scan_user": SCAN_USER,
    "scan_name": SCAN_NAME,
    "chip_thickness": CHIP_THICKNESS,
    "scan_exfoliated_material": EXFOLIATED_MATERIAL,
    "scan_exfoliation_method": EXFOLIATION_METHOD,
}

# Directory Paths
scan_directory = os.path.join(IMAGE_DIRECTORY, SCAN_NAME)

# File Paths
file_path = os.path.dirname(os.path.abspath(__file__))
scan_meta_path = os.path.join(scan_directory, "meta.json")
overview_path = os.path.join(scan_directory, "overview.png")
overview_compressed_path = os.path.join(scan_directory, "overview_compressed.jpg")
mask_path = os.path.join(scan_directory, "mask.png")
scan_area_path = os.path.join(scan_directory, "scan_area_map.png")

# Creating Paths
if not os.path.exists(scan_directory):
    os.makedirs(scan_directory)

# Dump the Scan Metadata into the folder
with open(scan_meta_path, "w") as fp:
    json.dump(META_DICT, fp, sort_keys=True, indent=4)

motor_driver = motor_driver_class()
camera_driver = camera_driver_class()
microscope_driver = microscope_driver_class()

print("Starting to raster in 2.5x...")
image_2_directory, meta_2_directory = raster.raster_plate(
    scan_directory,
    motor_driver,
    microscope_driver,
    camera_driver,
)

print("Compressing 2.5x Images...")
compressed_images_2_directory = stitcher.compress_images(image_2_directory)

print("Stitching Images...")
overview_image = stitcher.stitch_image(compressed_images_2_directory)
cv2.imwrite(overview_path, overview_image)

print("Compressing Overview Image...")
overview_image_compressed = cv2.resize(overview_image, (2000, 2000))
cv2.imwrite(
    overview_compressed_path,
    overview_image_compressed,
    [int(cv2.IMWRITE_JPEG_QUALITY), 80],
)

print("Creating mask...")
masked_overview = stitcher.create_mask_from_stitched_image(overview_image)
cv2.imwrite(mask_path, masked_overview)

masked_overview = cv2.imread(mask_path, 0)

print("Creating scan area mask...")
labeled_scan_area = stitcher.create_scan_area_map_from_mask(masked_overview)
cv2.imwrite(scan_area_path, labeled_scan_area)

print("Please Calibrate the 20x Scope")
print("Use E and R to Swap the Scopes")
print("Use Q to finish the Calibration")
print("Make sure to end the Calibration when in the 20x Scope")
calibrate_scope(
    motor_driver,
    microscope_driver,
    camera_driver,
)

start = time.time()

print("Rastering in 20x...")
raster.raster_scan_area_map(
    scan_directory,
    labeled_scan_area,
    motor_driver,
    microscope_driver,
    camera_driver,
)

print(
    f"Total elapsed Time: {(time.time() - start) // 3600:02.0f}:{((time.time() - start) // 60 )% 60:02.0f}:{int(time.time() - start) % 60:02.0f}"
)
