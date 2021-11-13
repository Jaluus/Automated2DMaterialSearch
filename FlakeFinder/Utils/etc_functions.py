"""
A collection of various functions
"""
import functools
import json
import os
import re
import sys
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


def set_microscope_and_camera_settings(
    microscope_settings_dict: dict,
    camera_settings_dict: dict,
    magnification_idx: int,
    camera_driver: camera_driver_class,
    microscope_driver: microscope_driver_class,
):
    """Sets the Microscrope and Camera Settings as well as the right Magnification\n
    Checks if the settings have changed and updates them if they have not\n

    Args:
        microscope_settings_dict (dict): The settings for the microscope as a dict
        camera_settings_dict (dict): The settings for the camera as a dict
        magnification (int): The magnification of the Microscope as the Index\n
        1: 2,5x, 2: 5x, 3: 20x, 4: 50x, 5: 100x
        camera_driver (camera_driver_class): The camera driver
        microscope_driver (microscope_driver_class): The microscope driver
    """

    # First sets the right Magnification
    microscope_driver.set_mag(magnification_idx)
    time.sleep(1)

    # Now set the Camera Settings
    camera_driver.set_properties(**camera_settings_dict[str(magnification_idx)])
    time.sleep(1)

    # Now set the Microscope Settings
    microscope_driver.set_lamp_voltage(
        microscope_settings_dict[str(magnification_idx)]["light_voltage"]
    )
    microscope_driver.set_lamp_aperture_stop(
        microscope_settings_dict[str(magnification_idx)]["aperture"]
    )
    time.sleep(1)


def calibrate_scope(
    motor: motor_driver_class,
    microscope: microscope_driver_class,
    camera: camera_driver_class,
    needed_magnification_idx: int,
):
    """Starts the scope calibration process

    Args:
        motor (motor_driver_class):
        microscope (microscope_driver_class):
        camera (camera_driver_class):
        needed_magnification_idx (int): The Magnificaiton which is needed to be calibrated

    Returns:
        (IMAGE , 3-Tuple ): The new Flatfield Image and the background Values, can also return None, None if none is selected
    """
    MAG_KEYS = {
        1: "2.5x",
        2: "5x",
        3: "20x",
        4: "50x",
        5: "100x",
    }

    new_flatfield = None
    new_background_values = None

    print("----------------------------")
    print(f"Please calibrate the {MAG_KEYS[needed_magnification_idx]}x Scope")
    print("Use E and R to Swap the Scopes")
    print("Use Q to finish the Calibration")
    print(
        "Use F to select a new Flatfield image to use ase Background reference, if none is selected the default is used"
    )
    print(
        f"Make sure to end the Calibration when in the {MAG_KEYS[needed_magnification_idx]}x Scope"
    )
    print("----------------------------")

    cv2.namedWindow("Calibration Window")
    curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
    cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

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
        cv2.imshow("Calibration Window", img_small)

        key = cv2.waitKey(1)
        if key == ord("q"):
            # Check if the scope is actually in the right magnification
            if microscope.get_properties()["nosepiece"] == needed_magnification_idx:
                break
            else:
                print(
                    f"Please calibrate the microscope to the {MAG_KEYS[needed_magnification_idx]} Scope, you are currently in the {MAG_KEYS[microscope.get_properties()['nosepiece']]} Scope"
                )

        elif key == ord("f"):
            new_flatfield = img.copy()
            new_background_values = cv2.mean(new_flatfield)[:-1]

        elif key == ord("e"):
            microscope.rotate_nosepiece_forward()
            curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
            cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

        elif key == ord("r"):
            microscope.rotate_nosepiece_backward()
            curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
            cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

    cv2.destroyAllWindows()
    return new_flatfield, new_background_values


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
            array = np.array(meta_dict[layer][key])

            mean, stddev = cv2.meanStdDev(array)

            axes[idx].grid()
            axes[idx].set_ylabel("counts")
            axes[idx].set_title(f"{layer} {key}")
            axes[idx].hist(
                array,
                bins=50,
                rwidth=0.8,
                label=f"n = {len(array)}\nmean = {mean[0,0]:.3g}\n$\sigma$ = {stddev[0,0]:.3g}",
            )
            axes[idx].legend()
        plt.savefig(os.path.join(hist_dir, f"{key}_hist.png"))

    flake_meta_path = os.path.join(scan_directory, "flake_histogram_data.json")
    with open(flake_meta_path, "w") as f:
        json.dump(meta_dict, f, sort_keys=True, indent=4)
