"""
A collection of various functions
"""
import functools
import json
import os
import re
import time

import cv2
import matplotlib.pyplot as plt
import numpy as np
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class


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


def get_chip_directorys(scan_directory):
    """Yields all the chip directorys in the Current scan Directory

    Args:
        scan_directory (string): The path of the current scan

    Yields:
        string: A path to a chip Directory
    """
    chip_directory_names = [
        chip_directory_name
        for chip_directory_name in sorted_alphanumeric(os.listdir(scan_directory))
        if (
            os.path.isdir(os.path.join(scan_directory, chip_directory_name))
            and chip_directory_name[:4] == "Chip"
        )
    ]

    # iterate over all chip directory names
    for chip_directory_name in chip_directory_names:

        # get the full path to the chip dir
        chip_directory = os.path.join(scan_directory, chip_directory_name)

        yield chip_directory


def get_flake_directorys(scan_directory: str):
    """Yields all the Flake directorys in the Current scan Directory

    Args:
        scan_directory (string): The apth of the current scan
        callback_function (function) : a Callback function that is beeing calld each time a new Chip is created

    Yields:
        string: An Absolute Path to a Flake Directory
    """
    chip_directory_names = [
        chip_directory_name
        for chip_directory_name in sorted_alphanumeric(os.listdir(scan_directory))
        if (
            os.path.isdir(os.path.join(scan_directory, chip_directory_name))
            and chip_directory_name[:4] == "Chip"
        )
    ]

    # iterate over all chip directory names
    for chip_directory_name in chip_directory_names:

        # get the full path to the chip dir
        chip_directory = os.path.join(scan_directory, chip_directory_name)

        # Extract all the Flake Directory names
        flake_directory_names = [
            flake_directory_name
            for flake_directory_name in sorted_alphanumeric(os.listdir(chip_directory))
            if os.path.isdir(os.path.join(chip_directory, flake_directory_name))
        ]

        # iterate over all flake directory names
        for flake_directory_name in flake_directory_names:

            # get the full path to the flake dir
            flake_directory = os.path.join(chip_directory, flake_directory_name)
            yield flake_directory


def Create_Metahistograms(
    scan_directory,
    layers=["monolayer", "bilayer", "trilayer"],
):
    # All wanted Metadata for Histograms, has to be nicer
    hist_dir = os.path.join(scan_directory, "Histograms")
    if not os.path.exists(hist_dir):
        os.makedirs(hist_dir)

    meta_dict = {}

    for layer in layers:
        meta_dict[layer] = {
            "size": [],
            "mean_contrast_r": [],
            "mean_contrast_g": [],
            "mean_contrast_b": [],
            "aspect_ratio": [],
            "proximity_stddev": [],
            "entropy": [],
        }

    flake_directorys = get_flake_directorys(scan_directory)
    for flake_directory in flake_directorys:
        meta_path = os.path.join(flake_directory, "meta.json")
        with open(meta_path, "r") as f:
            meta = json.load(f)
            flake_meta = meta["flake"]

        if flake_meta["thickness"] in meta_dict.keys():
            for key in meta_dict[flake_meta["thickness"]].keys():
                meta_dict[flake_meta["thickness"]][key].append(flake_meta[key])

    num_hists = len(layers)

    for key in meta_dict[layers[0]].keys():
        fig, axes = plt.subplots(num_hists, sharex=True)
        for idx, layer in enumerate(layers):
            array = meta_dict[layer][key]

            axes[idx].grid()
            axes[idx].set_ylabel("counts")
            axes[idx].set_title(f"{layer} {key}")
            axes[idx].hist(
                array,
                bins=50,
                rwidth=0.8,
                label=f"n = {len(array)}",
            )
            axes[idx].legend()
        plt.savefig(os.path.join(hist_dir, f"{key}_hist.png"))

    flake_meta_path = os.path.join(scan_directory, "flake_histogram_data.json")
    with open(flake_meta_path, "w") as f:
        json.dump(meta_dict, f, sort_keys=True, indent=4)


if __name__ == "__main__":
    Create_Metahistograms(
        r"C:\Users\duden\Desktop\Mikroskop Bilder\Eikes_Flocken_Full_Final"
    )
