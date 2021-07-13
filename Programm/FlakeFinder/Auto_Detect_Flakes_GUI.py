import json
import os
import shutil
import sys
import time

import cv2

import Utils.raster_functions as raster
import Utils.stitcher_functions as stitcher
import Utils.upload_functions as uploader
from Classes.detection_class import detector_class
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class
from GUI.parameter_picker import parameter_picker_class
from Utils.etc_functions import *

# getting the Parameters
try:
    parameter_picker = parameter_picker_class()
    parameter_dict = parameter_picker.take_input()
except:
    sys.exit(0)

# Created Metadict
META_DICT = {
    **parameter_dict,
    "scan_time": time.time(),
}


# Directory Paths
scan_directory = os.path.join(META_DICT["image_directory"], META_DICT["scan_name"])

# File Paths
file_path = os.path.dirname(os.path.abspath(__file__))
print(file_path)

scan_meta_path = os.path.join(scan_directory, "meta.json")
overview_path = os.path.join(scan_directory, "overview.png")
overview_compressed_path = os.path.join(scan_directory, "overview_compressed.jpg")
mask_path = os.path.join(scan_directory, "mask.png")
scan_area_path = os.path.join(scan_directory, "scan_area_map.png")

flat_field_path = os.path.join(
    file_path,
    "Parameters",
    "Flatfields",
    f"{META_DICT['chip_thickness']}.png",
)
contrasts_path = os.path.join(
    file_path,
    "Parameters",
    "Contrasts",
    f"{META_DICT['scan_exfoliated_material'].lower()}_{META_DICT['chip_thickness']}.json",
)

# Open the Jsons and get the needed Data
with open(contrasts_path) as f:
    contrast_params = json.load(f)

# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Creating Paths
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
myDetector = detector_class(
    contrast_dict=contrast_params,
    flat_field=flat_field,
    size_threshold=META_DICT["size_threshold"],
    entropy_threshold=META_DICT["entropy_threshold"],
)

start = time.time()
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
overview_image_compressed = cv2.imread(overview_compressed_path)

print("Creating mask...")
masked_overview = stitcher.create_mask_from_stitched_image(overview_image)
cv2.imwrite(mask_path, masked_overview)

print("Creating scan area mask...")
labeled_scan_area = stitcher.create_scan_area_map_from_mask(
    masked_overview,
    erode_iterations=1,
)
cv2.imwrite(scan_area_path, labeled_scan_area)

print("Removing unneeded 2.5x directory...")
dir_path = os.path.dirname(image_2_directory)
shutil.rmtree(dir_path)

print(
    f"Time to create overview Image: {(time.time() - start) // 3600:02.0f}:{((time.time() - start) // 60 )% 60:02.0f}:{int(time.time() - start) % 60:02.0f}"
)

print("Please Calibrate the 20x Scope")
print("Use E and R to Swap the Scopes")
print("Use Q to finish the Calibration")
print("Make sure to end the Calibration when in the 20x Scope")
calibrate_scope(
    motor_driver,
    microscope_driver,
    camera_driver,
)

local = time.time()

print("Finding Flakes in 20x...")
raster.search_scan_area_map(
    scan_directory=scan_directory,
    area_map=labeled_scan_area,
    motor_driver=motor_driver,
    microscope_driver=microscope_driver,
    camera_driver=camera_driver,
    detector=myDetector,
    overview=overview_image_compressed,
)

print(
    f"Time to search in 20x: {(time.time() - local) // 3600:02.0f}:{((time.time() - local) // 60 )% 60:02.0f}:{int(time.time() - local) % 60:02.0f}"
)
local = time.time()

print("Revisiting each Flake to take Pictures...")
for mag in [3, 4, 5, 1, 2]:
    raster.read_meta_and_center_flakes(
        scan_directory,
        motor_driver,
        microscope_driver,
        camera_driver,
        magnification=mag,
    )

print(
    f"Elapsed Time during revisiting: {(time.time() - local) // 3600:02.0f}:{((time.time() - local) // 60 )% 60:02.0f}:{int(time.time() - local) % 60:02.0f}"
)

print("Turning off the Lamp on the Microscope to conserve the Lifetime...")
microscope_driver.lamp_off()

print("Creating Histograms...")
Create_Metahistograms(scan_directory)

print("Uploading the Scan Directory...")
uploader.upload_directory(scan_directory, META_DICT["server_url"])

print(
    f"Total elapsed Time: {(time.time() - start) // 3600:02.0f}:{((time.time() - start) // 60 )% 60:02.0f}:{int(time.time() - start) % 60:02.0f}"
)
