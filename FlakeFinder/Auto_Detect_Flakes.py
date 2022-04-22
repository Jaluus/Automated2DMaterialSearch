import json
import os
import shutil
import time

import cv2

import Utils.raster_functions as raster
import Utils.stitcher_functions as stitcher
import Utils.upload_functions as uploader
from Detector_COV.detection_class import detector_class

# Custom imports
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class
from Utils.etc_functions import *

START_TIME = time.time()

# Constants
SERVER_URL = "http://134.61.6.112:5000/upload"
IMAGE_DIRECTORY = (
    r"C:\Users\Transfersystem User\Pictures\01_FlakeFinder\automatic_search"
)
SCAN_NAME = "graphene_for_bubble_push"
SCAN_USER = "sunaja"
EXFOLIATED_MATERIAL = "Graphene"
CHIP_THICKNESS = "90nm"
MAGNIFICATION = 20

# B = 0, G = 1, R = 2
# Dont use the Blue Channel if you have a lot of tape residue on the chips
USED_CHANNELS = [0, 1, 2]
COVARIANCE_SCALING_FACTORS = [2, 1, 1]

# The "Radius" which is used to still classify the flakes as a certain thickness
STANDARD_DEVIATION_THRESHOLD = 5

# Filter Parameter
SIZE_THRESHOLD = 200

# Created Metadict
META_DICT = {
    "server_url": SERVER_URL,
    "standard_deviation_threshold": STANDARD_DEVIATION_THRESHOLD,
    "image_directory": IMAGE_DIRECTORY,
    "scan_name": SCAN_NAME,
    "scan_user": SCAN_USER,
    "scan_exfoliated_material": EXFOLIATED_MATERIAL,
    "chip_thickness": CHIP_THICKNESS,
    "scan_magnification": MAGNIFICATION,
    "chip_thickness": CHIP_THICKNESS,
    "size_threshold": SIZE_THRESHOLD,
    "scan_time": START_TIME,
}

# Convert to Magnification index
MAG_IDX_DICT = {
    2.5: 1,
    5: 2,
    20: 3,
    50: 4,
    100: 5,
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
parameter_directory = os.path.join(file_path, "Parameters")

flat_field_path = os.path.join(
    parameter_directory,
    "Flatfields",
    f"{EXFOLIATED_MATERIAL.lower()}_{CHIP_THICKNESS}_{MAGNIFICATION}x.png",
)
contrast_params_path = os.path.join(
    parameter_directory,
    "Contrasts",
    f"{EXFOLIATED_MATERIAL.lower()}_{CHIP_THICKNESS}.json",
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
magnification_params_path = os.path.join(
    parameter_directory,
    "Scan_Magnification",
    f"{MAGNIFICATION}x.json",
)

# Open the Jsons and get the needed Data
with open(contrast_params_path) as f:
    contrast_params = json.load(f)
with open(camera_settings_path) as f:
    camera_settings = json.load(f)
with open(microscope_settings_path) as f:
    microscope_settings = json.load(f)
with open(magnification_params_path) as f:
    magnification_params = json.load(f)

# Read the flat field and save it in the scan directory for later use
flat_field = cv2.imread(flat_field_path)
cv2.imwrite(os.path.join(scan_directory, "flat_field.png"), flat_field)

# Creating Directories for the Scan
if not os.path.exists(scan_directory):
    os.makedirs(scan_directory)

# Dump the Scan Metadata into the folder
with open(scan_meta_path, "w") as fp:
    json.dump(META_DICT, fp, sort_keys=True, indent=4)

# Driver Initialization
motor_driver = motor_driver_class()
camera_driver = camera_driver_class()
microscope_driver = microscope_driver_class()

# Detector Initialization
# Detector Init
myDetector = detector_class(
    contrast_dict=contrast_params,
    size_threshold=SIZE_THRESHOLD,
    magnification=MAGNIFICATION,
    standard_deviation_threshold=STANDARD_DEVIATION_THRESHOLD,
    used_channels=USED_CHANNELS,
    covariance_scaling_factors=COVARIANCE_SCALING_FACTORS,
)


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

print("Removing unneeded 2.5x directory...")
dir_path = os.path.dirname(image_2_directory)
shutil.rmtree(dir_path)

print(
    f"Time to create overview Image: {(time.time() - START_TIME) // 3600:02.0f}:{((time.time() - START_TIME) // 60 )% 60:02.0f}:{int(time.time() - START_TIME) % 60:02.0f}"
)

new_flatfield, new_background_values = calibrate_scope(
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
if new_background_values is not None:
    myDetector.custom_background_values = new_background_values

scan_area_time_start = time.time()
print(f"Finding Flakes in {MAGNIFICATION}x...")
raster.search_scan_area_map(
    scan_directory=scan_directory,
    area_map=labeled_scan_area,
    motor_driver=motor_driver,
    microscope_driver=microscope_driver,
    camera_driver=camera_driver,
    detector=myDetector,
    flat_field=flat_field,
    magnification_idx=MAG_IDX_DICT[MAGNIFICATION],
    overview=overview_image_compressed,
    camera_settings=camera_settings,
    microscope_settings=microscope_settings,
    **magnification_params,
)
scan_area_time_duration = time.time() - scan_area_time_start

print(
    f"Time to search in {MAGNIFICATION}x: {scan_area_time_duration // 3600:02.0f}:{(scan_area_time_duration // 60 )% 60:02.0f}:{int(scan_area_time_duration) % 60:02.0f}"
)

revisit_time_start = time.time()
print("Revisiting each Flake to take Pictures...")
for current_magnification_index in [3, 4, 5, 1, 2]:
    raster.read_meta_and_center_flakes(
        scan_directory,
        motor_driver,
        microscope_driver,
        camera_driver,
        magnification_idx=current_magnification_index,
        camera_settings=camera_settings,
        microscope_settings=microscope_settings,
    )
revisit_time_duration = time.time() - revisit_time_start

print(
    f"Elapsed Time during revisiting: {revisit_time_duration // 3600:02.0f}:{(revisit_time_duration // 60 )% 60:02.0f}:{int(revisit_time_duration) % 60:02.0f}"
)

print("Turning off the Lamp on the Microscope to conserve the Lifetime...")
microscope_driver.lamp_off()

print("Uploading the Scan Directory...")
uploader.upload_directory(scan_directory, SERVER_URL)

full_scan_duration = time.time() - START_TIME
print(
    f"Total elapsed Time: {full_scan_duration // 3600:02.0f}:{(full_scan_duration // 60 )% 60:02.0f}:{int(full_scan_duration) % 60:02.0f}"
)
