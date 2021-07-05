import cv2
import numpy as np
import matplotlib.pyplot as plt
from skimage.morphology import disk


def mark_on_overview(
    overview_image,
    motor_pos,
    flake_number: int = None,
    x_motor_range: float = 105,
    y_motor_range: float = 103.333,
    x_step: float = 0.7380,
    y_step: float = 0.4613,
):
    overview_copy = overview_image.copy()

    picture_coords = np.array(
        [
            int(motor_pos[0] * overview_copy.shape[0] / x_motor_range),
            int(motor_pos[1] * overview_copy.shape[1] / y_motor_range),
        ]
    )

    frame_size = np.array(
        [
            int(x_step * overview_copy.shape[0] / x_motor_range),
            int(y_step * overview_copy.shape[1] / y_motor_range),
        ]
    )

    cv2.rectangle(
        overview_copy,
        picture_coords,
        picture_coords + frame_size,
        [0, 255, 0],
        thickness=1,
    )
    if flake_number is not None:
        cv2.putText(
            overview_copy,
            str(flake_number),
            (
                int(picture_coords[0] + frame_size[0] / 2),
                int(picture_coords[1] + frame_size[1] / 2),
            ),
            cv2.FONT_HERSHEY_DUPLEX,
            0.7,
            [0, 0, 255],
            thickness=2,
        )

    return overview_copy


def mark_flake(flake, image, image_path):
    plt.rcParams["figure.dpi"] = 300

    marked_image = image.copy()
    (x, y) = flake["position_bbox"]
    w = flake["width_bbox"]
    h = flake["height_bbox"]
    cv2.rectangle(
        marked_image,
        (x - 20, y - 20),
        (x + w + 20, y + h + 20),
        color=[0, 0, 255],
        thickness=2,
    )

    outline_flake = cv2.dilate(flake["mask"], disk(3), iterations=2)
    outline_flake = cv2.morphologyEx(flake["mask"], cv2.MORPH_GRADIENT, disk(2))

    marked_image[outline_flake != 0] = [0, 0, 255]

    plt.text(
        x - 20,
        y - 40,
        f"{flake['num_pixels'] * 0.15:.0f}μm² , S = {flake['entropy']:.2f} , σ = {flake['proximity_stddev']:.2f}",
        color="red",
    )
    plt.title(flake["layer"])
    plt.imshow(cv2.cvtColor(marked_image, cv2.COLOR_BGR2RGB))
    plt.tight_layout()
    plt.savefig(image_path)
    plt.clf()