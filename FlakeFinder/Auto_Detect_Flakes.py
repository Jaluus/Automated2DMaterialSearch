import sys
import os
import cv2
import time
import matplotlib.pyplot as plt
import json

# Custom imports
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class
from Classes.detection_class import detector_class
import Utils.raster_functions as raster
import Utils.stitcher_functions as stitcher


material = "Graphene"
scan_name = "Eikes_Flocken_2"

microscope_image_dir = r"C:\Users\Transfersystem User\Desktop\Mic_bilder"
scan_directory = os.path.join(microscope_image_dir, material, scan_name)
overview_path = os.path.join(scan_directory, "overview.png")
mask_path = os.path.join(scan_directory, "mask.png")
scan_area_path = os.path.join(scan_directory, "scan_area_map.png")

flat_field_path = os.path.join(
    os.path.dirname(__file__), "Detection", "Flatfield", "90nm.png"
)
contrasts_path = os.path.join(
    os.path.dirname(__file__), "Detection", "Params", "contrasts_Alex.json"
)

# Open the Json and get the Need infos
with open(contrasts_path) as f:
    detector_params = json.load(f)

# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Creating Paths
if not os.path.exists(scan_directory):
    os.makedirs(scan_directory)

# Driver Creation
motor_driver = motor_driver_class()
camera_driver = camera_driver_class()
microscope_driver = microscope_driver_class()

# Detector Init
myDetector = detector_class(detector_params, flat_field=flat_field)

# #### Creating the Overview and the Scan Area Mask
# print("Starting to raster in 2.5x...")
# picture_dir, meta_dir = raster.raster_plate(
#     scan_directory,
#     motor_driver,
#     microscope_driver,
#     camera_driver,
# )

# print("Compressing...")
# compressed_images_dir = stitcher.compress_images(picture_dir)

# print("Stitching images...")
# stitcher.stitch_image(compressed_images_dir, overview_path)

# print("Creating mask...")
# stitcher.create_mask_from_stitched_image(overview_path, mask_path)

# print("Creating scan area map...")
# labeled_scan_area = stitcher.create_scan_area_map_from_mask(mask_path, scan_area_path)
# ####

# read back the labeled scan area Mask
labeled_scan_area = cv2.imread(scan_area_path, 0)

print("Find Flakes in 20x...")
raster.search_scan_area_map(
    scan_directory,
    labeled_scan_area,
    motor_driver,
    microscope_driver,
    camera_driver,
    myDetector,
)

for mag in [3, 4, 5, 1, 2]:
    raster.read_meta_and_center_flakes(
        scan_directory,
        motor_driver,
        microscope_driver,
        camera_driver,
        magnification=mag,
    )
