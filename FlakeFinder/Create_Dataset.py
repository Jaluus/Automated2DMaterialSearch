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
IMAGE_DIRECTORY = r"C:\Users\Transfersystem User\Pictures\01_FlakeFinder\dataset"
EXFOLIATED_MATERIAL = "wse2_AU"
SCAN_NAME = "WSe2_on_Au-SiO2-grid_2_AUparam"
CHIP_THICKNESS = "90nm"
SCAN_USER = "Jo"
EXFOLIATION_METHOD = "undefined"
# Possibilities  20 , 50 , 100 , not defined 5
MAGNIFICATION = 20

# Directory Paths
scan_directory = os.path.join(IMAGE_DIRECTORY, SCAN_NAME)

# File Paths
file_path = os.path.dirname(os.path.abspath(__file__))
scan_meta_path = os.path.join(scan_directory, "meta.json")
overview_path = os.path.join(scan_directory, "overview.png")
overview_compressed_path = os.path.join(scan_directory, "overview_compressed.jpg")
mask_path = os.path.join(scan_directory, "mask.png")
scan_area_path = os.path.join(scan_directory, "scan_area_map.png")
parameter_directory = os.path.join(file_path, "Parameters")

flat_field_path = os.path.join(
    parameter_directory,
    "Flatfields",
    f"{EXFOLIATED_MATERIAL.lower()}_{CHIP_THICKNESS}_{MAGNIFICATION}x.png",
)
magnification_params_path = os.path.join(
    parameter_directory,
    "Scan_Magnification",
    f"{MAGNIFICATION}x.json",
)
camera_settings_path = os.path.join(
    parameter_directory,
    "Camera_Parameters",
    f"{EXFOLIATED_MATERIAL.lower()}_{MAGNIFICATION}x.json",
)
microscope_settings_path = os.path.join(
    parameter_directory,
    "Microscope_Parameters",
    f"{EXFOLIATED_MATERIAL.lower()}_{MAGNIFICATION}x.json",
)

# Get the data from the json files
with open(camera_settings_path) as f:
    camera_settings = json.load(f)
with open(microscope_settings_path) as f:
    microscope_settings = json.load(f)
with open(magnification_params_path) as f:
    magnification_params = json.load(f)

# Creating Paths
if not os.path.exists(scan_directory):
    os.makedirs(scan_directory)

META_DICT = {
    "scan_user": SCAN_USER,
    "scan_name": SCAN_NAME,
    "chip_thickness": CHIP_THICKNESS,
    "scan_exfoliated_material": EXFOLIATED_MATERIAL,
    "scan_exfoliation_method": EXFOLIATION_METHOD,
    "scan_magnificaiton": MAGNIFICATION,
    "microscope_parameters": microscope_settings,
    "camera_parameters": camera_settings,
}

# Convert to Magnification index
MAG_IDX_DICT = {
    2.5: 1,
    5: 2,
    20: 3,
    50: 4,
    100: 5,
}

# Dump the meta data
with open(scan_meta_path, "w") as f:
    json.dump(META_DICT, f, sort_keys=True, indent=4)

# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Init the drivers
motor_driver = motor_driver_class()
camera_driver = camera_driver_class()
microscope_driver = microscope_driver_class()

print("Starting to raster in 2.5x...")
image_2_directory, meta_2_directory = raster.raster_plate(
    scan_directory=scan_directory,
    motor_driver=motor_driver,
    microscope_driver=microscope_driver,
    camera_driver=camera_driver,
    camera_settings=camera_settings,
    microscope_settings=microscope_settings,
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

print("Creating scan area mask...")
labeled_scan_area = stitcher.create_scan_area_map_from_mask(
    masked_overview,
    erode_iterations=1,
    **magnification_params,
)
cv2.imwrite(scan_area_path, labeled_scan_area)

new_flatfield, _ = calibrate_scope(
    current_flatfield=flat_field,
    motor=motor_driver,
    microscope=microscope_driver,
    camera=camera_driver,
    needed_magnification_idx=MAG_IDX_DICT[MAGNIFICATION],
    camera_settings=camera_settings,
    microscope_settings=microscope_settings,
)

# Assigning the new flatfield and Background Values if they were used
if new_flatfield is not None:
    flat_field = new_flatfield.copy()

start = time.time()

print(f"Rastering in {MAGNIFICATION}x...")
raster.raster_scan_area_map(
    scan_directory,
    labeled_scan_area,
    motor_driver,
    microscope_driver,
    camera_driver,
    magnification=MAGNIFICATION,
    camera_settings=camera_settings,
    microscope_settings=microscope_settings,
    # flat_field=flat_field,
    **magnification_params,
)

print(
    f"Total elapsed Time: {(time.time() - start) // 3600:02.0f}:{((time.time() - start) // 60 )% 60:02.0f}:{int(time.time() - start) % 60:02.0f}"
)
