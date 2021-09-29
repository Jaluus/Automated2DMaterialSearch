import json
import os
import time
import sys
import cv2
import matplotlib.pyplot as plt
import numpy as np

from skimage.morphology import disk
from PIL import ImageFont, ImageDraw, Image

from Detector.detection_class import detector_class
from Utils.etc_functions import *
from Utils.marker_functions import *


# Constants
IMAGE_DIRECTORY = r"C:\Users\Transfersystem User\Pictures\01_FlakeFinder\dataset"
SCAN_NAME = "hbn_2109"
EXFOLIATED_MATERIAL = "hBNml"
CHIP_THICKNESS = "70nm"
MAGNIFICATION = 20

# Some Threshold parameters
ENTROPY_THRESHOLD = 4
SIZE_THRESHOLD = 20
SIGMA_THRESHOLD = 500

# -1 if you want to analyse all images
NUM_ANALYSED_IMAGES = -1


# Directory Paths
file_path = os.path.dirname(os.path.abspath(__file__))
scan_directory = os.path.join(IMAGE_DIRECTORY, SCAN_NAME)

# Defining directorys
save_dir = os.path.join(scan_directory, f"{MAGNIFICATION}x", "Masked_Images")
save_dir_meta = os.path.join(scan_directory, f"{MAGNIFICATION}x", "Masked_Images_Meta")
image_dir = os.path.join(scan_directory, f"{MAGNIFICATION}x", "Pictures")
meta_dir = os.path.join(scan_directory, f"{MAGNIFICATION}x", "Meta")
overview_path = os.path.join(scan_directory, "overview.png")
marked_overview_path = os.path.join(scan_directory, "overview_marked.png")
scan_meta_data_path = os.path.join(scan_directory, "meta.json")

# Creating non Existant Paths
if not os.path.exists(save_dir):
    os.makedirs(save_dir)
if not os.path.exists(save_dir_meta):
    os.makedirs(save_dir_meta)

# Defining parameter Paths
flat_field_path = os.path.join(
    file_path,
    "Parameters",
    "Flatfields",
    f"{EXFOLIATED_MATERIAL.lower()}_{CHIP_THICKNESS}_{MAGNIFICATION}x.png",
)
contrasts_path = os.path.join(
    file_path,
    "Parameters",
    "Contrasts",
    f"{EXFOLIATED_MATERIAL.lower()}_{CHIP_THICKNESS}.json",
)

image_names = sorted_alphanumeric(os.listdir(image_dir))[:NUM_ANALYSED_IMAGES]
meta_names = sorted_alphanumeric(os.listdir(meta_dir))[:NUM_ANALYSED_IMAGES]
num_images = len(image_names)

# Load the Overview Image
overview_image = cv2.imread(overview_path)

# Open the Json and get the Need infos
with open(contrasts_path) as f:
    contrast_params = json.load(f)

# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Define the start time
start_time = time.time()

# Detector Init
myDetector = detector_class(
    contrast_dict=contrast_params,
    flat_field=flat_field,
    entropy_threshold=ENTROPY_THRESHOLD,
    size_threshold=SIZE_THRESHOLD,
    sigma_treshold=SIGMA_THRESHOLD,
    magnification=MAGNIFICATION,
)

# An indexing method for found flakes
current_flake_number = 0
current_image_number = 0

for idx, (image_name, meta_name) in enumerate(zip(image_names, meta_names)):

    # Logging where we are
    if idx % (num_images // 100) == 1:
        curr_used_time = time.time() - start_time
        print(
            f"{idx} / {num_images} ({idx / num_images * 100 :.0f}%) | Time to go: {curr_used_time / idx * (num_images-idx):.0f}s | Time per Image {curr_used_time / idx * 1000:.0f}ms"
        )

    # just read the image ~37ms
    image_path = os.path.join(image_dir, image_name)
    image = cv2.imread(image_path)

    # ~120ms
    detected_flakes = myDetector.detect_flakes(image)

    # Operation on the flakes
    if len(detected_flakes) != 0:
        current_image_number += 1
        print(image_name, meta_name)

        # load the Metadata
        meta_path = os.path.join(meta_dir, meta_name)
        meta_data = json.load(open(meta_path, "r"))

        # mark the flake on overview map
        overview_image = mark_on_overview(
            overview_image=overview_image, motor_pos=meta_data["motor_pos"]
        )

        # Operate on each Flake
        for flake in detected_flakes:
            current_flake_number += 1

            # Copy the Image to not Modify the Original
            # If you dont know why I do this, Google Mutable Types
            # In short, I will fuck up my Image if im not copying it as I just pass a reference
            # draw_image = image.copy()

            # Extract some data from the Dict
            (x, y) = flake["position_bbox"]
            w = flake["width_bbox"]
            h = flake["height_bbox"]
            cv2.rectangle(
                image,
                (x - 20, y - 20),
                (x + w + 20, y + h + 20),
                color=[0, 0, 0],
                thickness=2,
            )

            # Dilate the flake outline and get the gradient to get a nice border
            outline_flake = cv2.dilate(flake["mask"], disk(3))
            outline_flake = cv2.morphologyEx(outline_flake, cv2.MORPH_GRADIENT, disk(2))

            # Draw this border on the image
            image[outline_flake != 0] = [0, 0, 0]

            cv2.putText(
                image,
                f"{flake['layer']}\nFlake-Nr.: {current_flake_number}",
                (x + w + 25, y - 35),
                cv2.FONT_HERSHEY_SIMPLEX,
                thickness=2,
                fontScale=2,
                color=(0, 0, 0),
            )
            #####

        # Now save the Image to its new Home
        cv2.imwrite(
            os.path.join(save_dir, f"{image_name}"),
            image,
        )

        # Delete the mask from the dict as it is not json serializable
        for flake in detected_flakes:
            del flake["mask"]

            with open(
                os.path.join(save_dir_meta, f"{current_flake_number}_{meta_name}"), "w"
            ) as f:
                json.dump(flake, f, indent=4, sort_keys=True)

cv2.imwrite(marked_overview_path, overview_image)

print(
    f"Total Elapsed Time: {(time.time() - start_time) // 3600:02.0f}:{((time.time() - start_time) // 60 )% 60:02.0f}:{int(time.time() - start_time) % 60:02.0f}"
)
