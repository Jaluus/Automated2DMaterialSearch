"""
A collection of various functions
"""
import re
import functools
import time
import cv2
from FlakeFinder.Drivers.Camera_Driver.camera_class import camera_driver_class
from FlakeFinder.Drivers.Microscope_Driver.microscope_class import (
    microscope_driver_class,
)
from FlakeFinder.Drivers.Motor_Driver.tango_class import motor_driver_class


def sorted_alphanumeric(data):
    """
    sorts an array of strings alphanumericly
    """

    def convert(text):
        return int(text) if text.isdigit() else text.lower()

    def alphanum_key(key):
        return [convert(c) for c in re.split("([0-9]+)", key)]

    return sorted(data, key=alphanum_key)


def timer(func):
    """
    A simple timer decorator to time my functions
    """

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        return_values = func(*args, **kwargs)
        print(time.time() - start)
        return return_values

    return wrapper


def calibrate_scope(
    motor: motor_driver_class,
    microscope: microscope_driver_class,
    camera: camera_driver_class,
):

    MAG_KEYS = {
        1: "2.5x",
        2: "5x",
        3: "20x",
        4: "50x",
        5: "100x",
    }

    while True:

        img = camera.get_image()

        img_small = cv2.resize(img, (960, 600))

        cv2.circle(
            img_small,
            (480, 300),
            10,
            color=[255, 0, 0],
            thickness=3,
        )

        curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]

        cv2.imshow("Calibration Window", img_small)
        cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

        key = cv2.waitKey(1)
        if key == ord("q"):
            break

        elif key == ord("e"):
            microscope.rotate_nosepiece_forward()

        elif key == ord("r"):
            microscope.rotate_nosepiece_backward()

    cv2.destroyAllWindows()
