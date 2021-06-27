import cv2
import numpy as np
import os

from skimage.morphology.selem import star
from Utils.etc_functions import *
import time
import json
import matplotlib.pyplot as plt
from skimage.morphology import disk
from Classes.detection_class import detector_class

plt.rcParams["figure.dpi"] = 300

# Pixel Offset @ 8463pix
pixel_delta_full = (140, 0)

scan_dir = r"C:\Users\duden\Desktop\Mikroskop Bilder\Graphene\FullScanE"

# Defining Paths
save_dir = os.path.join(scan_dir, "20x", "Masked_Images")
if not os.path.exists(save_dir):
    os.makedirs(save_dir)

image_dir = os.path.join(scan_dir, "20x", "Pictures")
meta_dir = os.path.join(scan_dir, "20x", "Meta")

overview_path = os.path.join(scan_dir, "stitched_image.png")
marked_overview_path = os.path.join(scan_dir, "stitched_image_marked.png")

flat_field_path = os.path.join(
    os.path.dirname(__file__), "Detection", "Flatfield", "90nm.png"
)
contrasts_path = os.path.join(
    os.path.dirname(__file__), "Detection", "Params", "contrasts_Alex.json"
)

image_names = sorted_alphanumeric(os.listdir(image_dir))
meta_names = sorted_alphanumeric(os.listdir(meta_dir))

# Load the Overview Image
overview_image = cv2.imread(overview_path)

# Open the Json and get the Need infos
with open(contrasts_path) as f:
    json_data = json.load(f)


# Read the flat field
flat_field = cv2.imread(flat_field_path)

# Defining colors and the Look of the Outline
colors = {
    "monolayer": [0, 255, 0],  # green
    "bilayer": [0, 255, 255],  # yellow
    "trilayer": [0, 0, 255],  # red
}

plt_colors = {
    "monolayer": "lime",  # green
    "bilayer": "yellow",  # yellow
    "trilayer": "red",  # red
}

# Saving the Contrasts for later
flake_contrasts = {
    "monolayer": [],
    "bilayer": [],
    "trilayer": [],
}

# Define the start time
start_time = time.time()

# Initializing the Detector
myDetector = detector_class(json_data, flat_field=flat_field)
# myDetector.set_searched_layers(["trilayer"])

num_images = len(image_names)

# An indexing method for forund flakes
ctr = 0

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
        ctr += 1
        print(image_name)

        # load the Metadata
        meta_path = os.path.join(meta_dir, meta_name)
        data = json.load(open(meta_path, "r"))
        motor_pos = np.array(data["motor_pos"], dtype=np.float64)

        # mark the flake on overview map
        picture_coords = np.array(motor_pos / 100 * overview_image.shape[0], dtype=int)
        cv2.circle(
            overview_image,
            (
                picture_coords[0] + pixel_delta_full[0],
                picture_coords[1] + pixel_delta_full[1],
            ),
            20,
            [0, 255, 0],
            thickness=5,
        )
        cv2.putText(
            overview_image,
            str(ctr),
            (
                picture_coords[0] - 15 + pixel_delta_full[0],
                picture_coords[1] + 6 + pixel_delta_full[1],
            ),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            [0, 0, 255],
            thickness=2,
        )

        # Mark the Flakes on the Image
        for flake in detected_flakes:
            (x, y) = flake["position"]
            w = flake["width_bbox"]
            h = flake["height_bbox"]
            cv2.rectangle(
                image,
                (x - 20, y - 20),
                (x + w + 20, y + h + 20),
                color=colors[flake["layer"]],
                thickness=2,
            )

            outline_flake = cv2.dilate(flake["mask"], disk(3))
            outline_flake = cv2.morphologyEx(flake["mask"], cv2.MORPH_GRADIENT, disk(2))

            image[outline_flake != 0] = colors[flake["layer"]]

            plt.text(
                x - 20,
                y - 40,
                f"{flake['num_pixels'] * 0.15:.0f}μm² , S = {flake['entropy']:.2f} , σ = {flake['proximity_stddev']:.2f}",
                color=plt_colors[flake["layer"]],
            )
        plt.title(f"Overview ID: {ctr:.0f}")
        plt.imshow(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
        plt.tight_layout()
        plt.savefig(os.path.join(save_dir, image_name))
        plt.clf()


cv2.imwrite(marked_overview_path, overview_image)

print(time.time() - start_time)
