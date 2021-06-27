import sys
import os
import cv2
import time
import matplotlib.pyplot as plt

# Custom imports
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class
import Utils.raster_functions as raster
import Utils.stitcher_functions as stitcher

# flow
# 1. Raster the Plate in 2.5x
# 2. compress the images
# 3. stitch the image
# 4. make a mask
# 5. make a scan_area_mask
# 6. raster over the scan area

material = "Graphene"
scan_name = "FullScanTest"

microscope_image_dir = r"C:\Users\Transfersystem User\Desktop\Mic_bilder"
scan_directory = os.path.join(microscope_image_dir, material, scan_name)
overview_path = os.path.join(scan_directory, "overview.png")
mask_path = os.path.join(scan_directory, "mask.png")
scan_area_path = os.path.join(scan_directory, "scan_area_map.png")

# Creating Paths
if not os.path.exists(scan_directory):
    os.makedirs(scan_directory)

motor_driver = motor_driver_class()
camera_driver = camera_driver_class()
microscope_driver = microscope_driver_class()

print("Starting to raster in 2.5x...")
picture_dir, meta_dir = raster.raster_plate(
    scan_directory,
    motor_driver,
    microscope_driver,
    camera_driver,
)

print("Compressing...")
compressed_images_dir = stitcher.compress_images(picture_dir)

print("Stitching images...")
stitcher.stitch_image(compressed_images_dir, overview_path)

print("Creating mask...")
stitcher.create_mask_from_stitched_image(overview_path, mask_path, threshold_value=45)

print("Creating scan area mask...")
labeled_scan_area = stitcher.create_scan_area_map_from_mask(
    mask_path, scan_area_path, erode_iterations=1, percentage_threshold=0.8
)

labeled_scan_area = cv2.imread(scan_area_path, 0)

print("Rastering in 20x...")
raster.raster_scan_area_map(
    scan_directory,
    labeled_scan_area,
    motor_driver,
    microscope_driver,
    camera_driver,
)
