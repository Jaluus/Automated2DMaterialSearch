import json
import os
import time
import cv2
import numpy as np

from skimage.morphology import disk

from Utils.preprocessor_functions import remove_vignette
from Utils.etc_functions import *
from Utils.marker_functions import *


# Converter fallback for json
def convert(o):
    if isinstance(o, np.generic):
        return o.item()
    raise TypeError


# Constants
IMAGE_DIRECTORY = r"E:\Datasets"
SCAN_NAME = "Graphene-Sunaja-20-04-2022"
EXFOLIATED_MATERIAL = "graphene"
CHIP_THICKNESS = "90nm"
MAGNIFICATION = 20
CUSTOM_BACKGROUND_VALUES = None  # [125, 115, 132]  # can be [B,G,R] or None

#### Detector Parameters ####

# Which kind of detector to use
USED_DETECTOR = "COV"  # "COV" or "LEGACY"


#### COV Detector Parameters ####
# B = 0, G = 1, R = 2
# Dont use the Blue Channel if you have a lot of tape residue on the chips
# This would look like USED_CHANNELs = [1, 2] if you only want to use the Red and Green Channel
USED_CHANNELS = [0, 1, 2]

# How much the ellipse is stretched in the Blue green and red channel
# Its advised to scale the blue channel up if you have a lot of tape residue on the chips and still want to use it
COVARIANCE_SCALING_FACTORS = [2, 2, 1]

# The "Radius" which is used to still classify the flakes as a certain thickness
STANDARD_DEVIATION_THRESHOLD = 6


#### General Detector Parameters ####

# Some Threshold parameters
SIZE_THRESHOLD = 200


#### Detection Parameters ####
NUM_ANALYSED_IMAGES = -1  # -1 means all images
START_IMAGE = 0

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

image_names = sorted_alphanumeric(os.listdir(image_dir))[
    START_IMAGE:NUM_ANALYSED_IMAGES
]
meta_names = sorted_alphanumeric(os.listdir(meta_dir))[START_IMAGE:NUM_ANALYSED_IMAGES]
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

# An indexing method for found flakes
current_flake_number = 0
current_image_number = 0

# Run the program in the main shell
# For multiprocessing puroposes
if __name__ == "__main__":

    # detector init
    if USED_DETECTOR == "COV":
        from Detector_COV.detection_class import detector_class

        myDetector = detector_class(
            contrast_dict=contrast_params,
            custom_background_values=CUSTOM_BACKGROUND_VALUES,
            size_threshold=SIZE_THRESHOLD,
            magnification=MAGNIFICATION,
            standard_deviation_threshold=STANDARD_DEVIATION_THRESHOLD,
            used_channels=USED_CHANNELS,
            covariance_scaling_factors=COVARIANCE_SCALING_FACTORS,
        )

    elif USED_DETECTOR == "LEGACY":
        from Detector.detection_class import detector_class

        myDetector = detector_class(
            contrast_dict=contrast_params,
            custom_background_values=CUSTOM_BACKGROUND_VALUES,
            size_threshold=SIZE_THRESHOLD,
            magnification=MAGNIFICATION,
        )

    else:
        raise ValueError("USED_DETECTOR must be either 'COV' or 'LEGACY'")

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

        # Removing the vignette from the image
        image = remove_vignette(image, flat_field)

        # ~120ms
        detected_flakes = myDetector.detect_flakes(image)

        # Operation on the flakes
        if len(detected_flakes) != 0:
            current_image_number += 1
            print(
                f"A total of {len(detected_flakes)} flakes were found in image {image_name}"
            )

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

                # # Extract some data from the Dict
                (x, y) = flake["position_bbox"]
                w = flake["width_bbox"]
                h = flake["height_bbox"]
                # cv2.rectangle(
                #     image,
                #     (x - 20, y - 20),
                #     (x + w + 20, y + h + 20),
                #     color=[0, 0, 0],
                #     thickness=2,
                # )

                # Dilate the flake outline and get the gradient to get a nice border
                # outline_flake = cv2.dilate(flake["mask"], disk(2))
                outline_flake = cv2.morphologyEx(
                    flake["mask"],
                    cv2.MORPH_GRADIENT,
                    disk(1),
                )

                # Draw this border on the image
                image[outline_flake != 0] = [0, 0, 255]

                cv2.putText(
                    image,
                    f"{flake['layer']}",
                    (x, y),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    thickness=1,
                    fontScale=1,
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
                    os.path.join(save_dir_meta, f"{current_flake_number}_{meta_name}"),
                    "w",
                ) as f:
                    json.dump(flake, f, indent=4, sort_keys=True, default=convert)

    cv2.imwrite(marked_overview_path, overview_image)

    print(
        f"Total Elapsed Time: {(time.time() - start_time) // 3600:02.0f}:{((time.time() - start_time) // 60 )% 60:02.0f}:{int(time.time() - start_time) % 60:02.0f}"
    )
