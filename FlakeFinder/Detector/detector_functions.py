import cv2
import numpy as np


def remove_vignette(image, flat_field):
    """Removes the Vignette from the Image

    Args:
        image (NxMx3 Array): The Image with the Vignette
        flat_field (NxMx3 Array): the Flat Field in RGB

    Returns:
        (NxMx3 Array): The Image without the Vignette
    """
    # convert to hsv and cast to 16bit, to be able to add more than 255
    image_hsv = np.asarray(cv2.cvtColor(image, cv2.COLOR_BGR2HSV), dtype=np.uint16)
    flat_field_hsv = np.asarray(
        cv2.cvtColor(flat_field, cv2.COLOR_BGR2HSV), dtype=np.uint16
    )

    # get the filter and apply it to the image
    image_hsv[:, :, 2] = (
        image_hsv[:, :, 2]
        / flat_field_hsv[:, :, 2]
        * cv2.mean(flat_field_hsv[:, :, 2])[0]
    )

    # clip it back to 255
    image_hsv[:, :, 2][image_hsv[:, :, 2] > 255] = 255

    # Recast to uint8 as the color depth is 8bit per channel
    image_hsv = np.asarray(image_hsv, dtype=np.uint8)

    # reconvert to bgr
    image_no_vigentte = cv2.cvtColor(image_hsv, cv2.COLOR_HSV2BGR)
    return image_no_vigentte
