import json
import os
import time
import sys
import cv2
import matplotlib.pyplot as plt
import numpy as np


from skimage.morphology import disk
from PIL import ImageFont, ImageDraw, Image

sys.path.insert(
    0, r"C:\Users\Transfersystem User\Desktop\Repos\BachelorThesis\Programm"
)


from FlakeFinder.Classes.detection_class import detector_class
from FlakeFinder.Utils.etc_functions import *
from FlakeFinder.Utils.marker_functions import *


# Constants
IMAGE_DIRECTORY = r"C:\Users\Transfersystem User\Desktop\Mic_bilder"
SCAN_NAME = "Dataset_Eike_050721"

# Directory Paths
scan_directory = os.path.join(IMAGE_DIRECTORY, SCAN_NAME)

# Defining directorys
save_dir = os.path.join(scan_directory, "20x", "Masked_Images")
image_dir = os.path.join(scan_directory, "20x", "Pictures")
meta_dir = os.path.join(scan_directory, "20x", "Meta")
overview_path = os.path.join(scan_directory, "overview.png")
marked_overview_path = os.path.join(scan_directory, "overview_marked.png")
scan_meta_data_path = os.path.join(scan_directory, "meta.json")

# Creating non Existant Paths
if not os.path.exists(save_dir):
    os.makedirs(save_dir)

# Defining parameter Paths
flat_field_path = os.path.join(
    os.path.dirname(__file__), "Parameters", "Flatfields", "90nm.png"
)
contrasts_path = os.path.join(
    os.path.dirname(__file__), "Parameters", "Contrasts", "graphene_90nm.json"
)
background_values_path = os.path.join(
    os.path.dirname(__file__), "Parameters", "Background_Values", "90nm.json"
)

image_names = sorted_alphanumeric(os.listdir(image_dir))
meta_names = sorted_alphanumeric(os.listdir(meta_dir))
num_images = len(image_names)

# Load the Overview Image
overview_image = cv2.imread(overview_path)

# Open the Json and get the Need infos
with open(contrasts_path) as f:
    contrast_params = json.load(f)
with open(background_values_path) as f:
    background_values_params = json.load(f)

# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Defining colors and the Look of the Outline
colors = {
    "monolayer": [0, 255, 0],  # green
    "bilayer": [0, 255, 255],  # yellow
    "trilayer": [0, 0, 255],  # red
}

# The Colors are getting flipped
plt_colors = {
    "monolayer": "lime",  # green
    "bilayer": "cyan",  # yellow
    "trilayer": "blue",  # red
}

font_path = os.path.join(os.path.dirname(__file__), "Helvetica.ttf")
font = ImageFont.truetype(font_path, 40)

# Define the start time
start_time = time.time()

# Detector Init
myDetector = detector_class(
    contrast_dict=contrast_params,
    background_values=background_values_params,
    flat_field=flat_field,
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
        # overview_image = mark_on_overview(
        #     overview_image, current_image_number, meta_data["motor_pos"]
        # )

        # Operate on each Flake
        for flake in detected_flakes:
            current_flake_number += 1

            # Copy the Image to not Modify the Original
            # If you dont know why I do this, Google Mutable Types
            # In short, I will fuck up my Image if im not copying it as I just pass a reference
            draw_image = image.copy()

            # Extract some data from the Dict
            (x, y) = flake["position_bbox"]
            w = flake["width_bbox"]
            h = flake["height_bbox"]
            cv2.rectangle(
                draw_image,
                (x - 20, y - 20),
                (x + w + 20, y + h + 20),
                color=colors[flake["layer"]],
                thickness=2,
            )

            # Dilate the flake outline and get the gradient to get a nice border
            outline_flake = cv2.dilate(flake["mask"], disk(3))
            outline_flake = cv2.morphologyEx(flake["mask"], cv2.MORPH_GRADIENT, disk(2))

            # Draw this border on the image
            draw_image[outline_flake != 0] = colors[flake["layer"]]

            #### All this is just to draw some text on the image, opencv doesnt allow µ, so I had to improvise
            img_pil = Image.fromarray(draw_image)
            draw = ImageDraw.Draw(img_pil)
            draw.text(
                (x + w + 25, y - 35),
                f"{flake['num_pixels'] * 0.15:.0f} µm²\nS = {flake['entropy']:.2f}\nσ = {flake['proximity_stddev']:.2f}",
                fill=plt_colors[flake["layer"]],
                font=font,
            )
            draw_image = np.array(img_pil)
            #####

            # Now save the Image to its new Home
            cv2.imwrite(
                os.path.join(save_dir, f"{current_flake_number}_{image_name}"),
                draw_image,
            )

cv2.imwrite(marked_overview_path, overview_image)

print(
    f"Total elapsed time: {(time.time() - start_time) // 60}:{int(time.time() - start_time) % 60}"
)
