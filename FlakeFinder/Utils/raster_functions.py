import sys
import os
import cv2
import time
import json
import numpy as np
from numpy.core.numeric import full

from Classes.detection_class import detector_class
from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class


def _create_folder_structure(
    scan_directory,
    magnification,
):
    magnification_path = os.path.join(scan_directory, f"{magnification}x")
    picture_path = os.path.join(magnification_path, "Pictures")
    meta_path = os.path.join(magnification_path, "Meta")

    if not os.path.exists(magnification_path):
        os.makedirs(magnification_path)

    if not os.path.exists(picture_path):
        os.makedirs(picture_path)

    if not os.path.exists(meta_path):
        os.makedirs(meta_path)

    return magnification_path, picture_path, meta_path


def raster_plate(
    scan_directory,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
    x_step: float = 5,
    y_step: float = 3.333,
    wait_time: float = 0.1,
):
    """
    scan_directory is the directory in which the scan is being saved\n
    magnification is used for naming\n
    returns the picture and meta directory\n

    real x view field : 5.9048 mm\n
    real y view field : 3.6905 mm\n
    => 18.1 % x-overlap\n
    => 10.7 % y-overlap\n
    => 21 rows\n
    => 31 columns\n
    """

    MAGNIFICATION = 2.5

    # creates the folder stucture
    magnification_dir, picture_dir, meta_dir = _create_folder_structure(
        scan_directory, MAGNIFICATION
    )

    # Start at 00 and make sure to be in 2.5x Mag
    motor_driver.abs_move(0, 0)

    # go into the 2.5x scope
    microscope_driver.set_mag(1)
    microscope_driver.set_default_values()
    camera_driver.set_default_properties(1)

    # Checks if it is actually at 00
    can_move_x = motor_driver.can_move(x_step, 0)
    can_move_y = motor_driver.can_move(0, y_step)

    # get the properties
    cam_props = camera_driver.get_properties()
    mic_props = microscope_driver.get_properties()

    curr_idx = 0

    while can_move_x:
        while can_move_y:
            curr_idx += 1

            print(
                f"\r{curr_idx}/651 scanned\r",
                end="",
                flush=True,
            )

            # # restart the AF when its off
            # if microscope_driver.get_af_status()() == 9:
            #     microscope_driver.find_af()
            #     print("searching...")
            #     time.sleep(1)

            # # Check if img is in focus, also abort if it takes to long to find the focus
            # af_ctr = 0
            # while (microscope_driver.get_af_status()() != 1) and (af_ctr <= 30):
            #     time.sleep(0.1)
            #     af_ctr += 1

            # get all the proeprties
            motor_pos = motor_driver.get_pos()
            all_props = {**cam_props, **mic_props, "motor_pos": motor_pos}

            # Save all the metadata in a JSON file
            json_path = os.path.join(meta_dir, f"{curr_idx}.json")
            with open(json_path, "w") as fp:
                json.dump(all_props, fp, sort_keys=True, indent=4)

            # Take image here wait 100ms to stabilze the camera
            time.sleep(wait_time)
            img = camera_driver.get_image()
            picture_path = os.path.join(picture_dir, f"{curr_idx}.png")
            cv2.imwrite(picture_path, img)

            can_move_y = motor_driver.rel_move(0, y_step)

        curr_x, curr_y = motor_driver.get_pos()
        motor_driver.abs_move(curr_x, 0)
        can_move_x = motor_driver.rel_move(x_step, 0)
        can_move_y = motor_driver.can_move(0, y_step)
    return picture_dir, meta_dir


def image_generator(
    area_map,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
    x_step: float = 0.7380,
    y_step: float = 0.4613,
    x_offset: float = 0,
    y_offset: float = 0,
    wait_time: float = 0.1,
) -> tuple(bool, np.ndarray, dict):
    """
    Image Generator\\
    Yields images taken from the Microscope\\
    first yields is always (None,None) as we need to move into position first
    
    Args:
        area_map (NxMx1 Array): A Map of the Chips with where to scan
        motor_driver (motor_driver_class): The Motordriver
        microscope_driver (microscope_driver_class): The Microscope Driver
        camera_driver (camera_driver_class): The Camera Driver
        x_step (float, optional): the x Dimension of the 20x Picture. Defaults to 0.7380.
        y_step (float, optional): the y Dimension of the 20x Picture. Defaults to 0.4613.
        x_offset (float, optional): The x offset of the Scan Area Map in mm. Defaults to 0.
        y_offset (float, optional): The y offset of the Scan Area Map in mm. Defaults to 0.
        wait_time (float, optional): The time to wait after moving before taking a picture in seconds. Defaults to 0.2.

    Returns:
        Tuple (Bool, NxMx3 Array, Dict): If you can keep yielding, the Image Returns None as the First image, the Prop dict
    """

    # go into the 20x scope
    microscope_driver.set_mag(3)
    microscope_driver.set_default_values()
    camera_driver.set_default_properties(3)

    # get the camera and microscope pros as these wont change
    cam_props = camera_driver.get_properties()
    mic_props = microscope_driver.get_properties()

    num_images = cv2.countNonZero(area_map)

    # Some Default Values
    curr_idx = 0
    image = None
    all_props = None

    for y_idx in range(area_map.shape[0]):

        # this implements a snake like pattern, its faster
        row = range(area_map.shape[1])
        if y_idx % 2 == 1:
            row = reversed(row)

        for x_idx in row:
            # Dont scan anything is the areamap is 0 as only 1s are beeing scanned
            if area_map[y_idx, x_idx] == 0:
                continue

            # Calculate the new Position on the plate
            # Round to get rid of floating point errors
            # if you want check out https://www.youtube.com/watch?v=s9F8pu5KfyM
            x_pos = round(x_step * x_idx - x_offset, 3)
            y_pos = round(y_step * y_idx - y_offset, 3)

            # move to the new Position
            motor_driver.abs_move(x_pos, y_pos)

            # Yields the Image
            yield image, all_props

            # get the motor props
            motor_pos = motor_driver.get_pos()
            all_props = {
                **cam_props,
                **mic_props,
                "motor_pos": motor_pos,
                "chip_id": int(area_map[y_idx, x_idx]),
            }

            # just for Logging
            curr_idx += 1
            print(
                f"\r{curr_idx}/{num_images} scanned\r",
                end="",
                flush=True,
            )

            # Take image here
            if wait_time > 0:
                time.sleep(wait_time)

            # take the image
            image = camera_driver.get_image()

    yield image, all_props


def raster_scan_area_map(
    scan_directory: str,
    area_map,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
):
    """
    Rasters the supplied scan Area Map\\
    Used to Create a Dataset as it saves all the taken Images

    Args:
        scan_directory (str): The Directory where the Scan is Located
        area_map (NxMx1 Array): A Map of the Chips with where to scan
        motor_driver (motor_driver_class): The Motordriver
        microscope_driver (microscope_driver_class): The Microscope Driver
        camera_driver (camera_driver_class): The Camera Driver

    Returns:
        Tuple: Returns the Picture Directory and the Meta Directory where the Image data is saved
    """

    MAGNIFICATION = 20

    # creating the folder structure
    magnification_dir, picture_dir, meta_dir = _create_folder_structure(
        scan_directory, MAGNIFICATION
    )

    image_gen = image_generator(
        area_map,
        motor_driver,
        microscope_driver,
        camera_driver,
    )

    idx = 0

    for image, prop_dict in image_gen:

        if image is None:
            continue

        # save the Image in its own Path
        picture_path = os.path.join(picture_dir, f"{idx}.png")
        cv2.imwrite(picture_path, image)

        # Save all the metadata in a JSON file
        json_path = os.path.join(meta_dir, f"{idx}.json")
        with open(json_path, "w") as fp:
            json.dump(prop_dict, fp, sort_keys=True, indent=4)

        idx += 1
    return picture_dir, meta_dir


def search_scan_area_map(
    scan_directory: str,
    area_map,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
    detector: detector_class,
):
    """
    Searches the supplied scan Area Map for Flakes\\
    Used to only extract the Flakes from the Scan

    Args:
        scan_directory (str): The Directory where the Scan is Located
        area_map (NxMx1 Array): A Map of the Chips with where to scan
        motor_driver (motor_driver_class): The Motordriver
        microscope_driver (microscope_driver_class): The Microscope Driver
        camera_driver (camera_driver_class): The Camera Driver
        x_dimension (float, optional): the x Dimension of the 20x Picture. Defaults to 0.7380.
        y_dimension (float, optional): the y Dimension of the 20x Picture. Defaults to 0.4613.
        x_offset (float, optional): The x offset of the Scan Area Map in mm. Defaults to 0.
        y_offset (float, optional): The y offset of the Scan Area Map in mm. Defaults to 0.
        magnification (float, optional): The Magification with which the Chips are getting scanned, only for File Naming. Defaults to 20.
        wait_time (float, optional): The time to wait after moving before taking a picture in seconds. Defaults to 0.2.
        detector (detector_class): The detector Object, initialized with values

    Returns:
        Tuple: Returns the Picture Directory and the Meta Directory where the Image data is saved
    """

    # 1. Scan the entire Area for flakes and save them in their respective folders
    # 2. Read back all the Metadata and retake images in 20x

    # Initiating the Generator, we fetch images from it
    image_gen = image_generator(
        area_map,
        motor_driver,
        microscope_driver,
        camera_driver,
    )

    # Autoincrementing Flake ID
    flake_id = {}

    # 1. Scan the entire Area for flakes and save them in their respective folders
    for image, prop_dict in image_gen:

        # take the next image if the gfotten image is invalid
        # Happends when its the first image take as we first need to move to the right position
        if image is None:
            continue

        # run the Detection Algorithm
        detected_flakes = detector.detect_flakes(image)

        # this is just ordering flakes into their repective folders
        if len(detected_flakes) != 0:

            # Create the Chip Directory for the Flake
            chip_id = prop_dict["chip_id"]
            chip_dir = os.path.join(scan_directory, f"Chip_{ chip_id }")
            if not os.path.exists(chip_dir):
                os.makedirs(chip_dir)

                # Init the flakes in chip
                flake_id[chip_id] = 0

            for flake in detected_flakes:
                flake_id[chip_id] += 1

                # this id is only! locally used to assign flakes to different folders
                local_flake_id = flake_id[chip_id]

                # create the flake directory
                flake_dir = os.path.join(chip_dir, f"Flake_{local_flake_id}")
                os.makedirs(flake_dir)

                flake_meta_data = create_flake_dict(prop_dict, flake)

                meta_path = os.path.join(flake_dir, "meta.json")
                # Now save the Flake Metadata in the Directory
                with open(meta_path, "w") as fp:
                    json.dump(flake_meta_data, fp, sort_keys=True, indent=4)

                #### ONLY FOR DEBUGGING
                if True:
                    picture_path = os.path.join(flake_dir, "eval_img.png")

                    x, y = flake_dir["position"]
                    w = flake_dir["width_bbox"]
                    h = flake_dir["height_bbox"]

                    debug_image = image.copy()

                    cv2.putText(
                        debug_image,
                        flake_dir["layer"],
                        (x, y),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1,
                        [0, 255, 0],
                        thickness=2,
                    )

                    cv2.rectangle(
                        debug_image,
                        (x, y),
                        (x + w, y + h),
                        color=[0, 255, 0],
                        thickness=2,
                    )

                    cv2.imwrite(picture_path, image)
                ####

    #### 2. Read back the Positions and raster in 20x,50x,100x,2.5x,5x in that order
    for mag in [3, 4, 5, 1, 2]:
        read_meta_and_center_flakes(
            scan_directory,
            motor_driver,
            microscope_driver,
            camera_driver,
            magnification=mag,
        )


def read_meta_and_center_flakes(
    scan_directory,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
    magnification: int = 3,
):
    """[summary]

    Args:
        scan_directory ([type]): [description]
        motor_driver (motor_driver_class): [description]
        microscope_driver (microscope_driver_class): [description]
        camera_driver (camera_driver_class): [description]
        magnification (int, optional): [description]. Defaults to 3.
    """
    
    microscope_driver.set_mag(magnification)
    microscope_driver.set_default_values()
    camera_driver.set_default_properties(magnification)

    image_keys = {
        1: "2.5x",
        2: "5x",
        3: "20x",
        4: "50x",
        5: "100x",
    }

    try:
        current_image_key = image_keys[magnification]
    except KeyError as e:
        print("wrong Magnification you need an int between 1 and 5, defaulting to 20x")
        current_image_key = "20x"

    # Extract all the Chip Directorys, not the images in the scan directory
    chip_directory_names = [
        chip_directory_name
        for chip_directory_name in os.listdir(scan_directory)
        if os.path.isdir(os.path.join(scan_directory, chip_directory_name))
    ]

    # iterate over all chip directory names
    for chip_directory_name in chip_directory_names:

        # get the full path to the chip dir
        chip_directory = os.path.join(scan_directory, chip_directory_name)

        # Extract all the Flake Directory names
        flake_directory_names = [
            flake_directory_name
            for flake_directory_name in os.listdir(chip_directory)
            if os.path.isdir(os.path.join(scan_directory, flake_directory_name))
        ]

        # iterate over all flake directory names
        for flake_directory_name in flake_directory_names:

            # get the full path to the flake dir
            flake_directory = os.path.join(chip_directory, flake_directory_name)

            # Define the image path
            image_path = os.path.join(flake_directory, f"{current_image_key}.png")

            # Open the metadata
            meta_path = os.path.join(flake_directory, "meta.json")
            with open(meta_path, "r") as f:
                meta_data = json.load(f)

            flake_position_x = meta_data["flake"]["position_x"]
            flake_position_y = meta_data["flake"]["position_y"]

            # now execute Movement
            motor_driver.abs_move(flake_position_x, flake_position_y)

            # get the properties relevant for the Image
            cam_props = camera_driver.get_properties()
            mic_props = microscope_driver.get_properties()
            all_props = {
                **cam_props,
                **mic_props,
            }

            # Take an image of the centered flake and write it
            image = camera_driver.get_image()
            cv2.imwrite(image_path, image)

            # Append more info to the metadata
            meta_data["images"][current_image_key] = all_props

            # rewrite the data to the metafile
            with open(meta_path, "w") as f:
                json.dump(meta_data, f, sort_keys=True, indent=4)


def create_flake_dict(image_dict, flake_dict):
    """Creates a Dict used to classify the Flakes from the Image dict, as well as from the Flake_dict

    Args:
        image_dict (dict): A dict containing metadata about the image
        flake_dict (dict): A dict containing metadata about the Flake

    Returns:
        dict: A dict with all the relevant keys needed to classify the Flake
    """
    # Possible error, swap posx and posy
    flake_position_x = (
        image_dict["motor_pos"][0] + flake_dict["position_x_micro"] / 1000
    )
    flake_position_y = (
        image_dict["motor_pos"][1] + flake_dict["position_y_micro"] / 1000
    )
    flake_size = flake_dict["size_micro"]
    flake_thickness = flake_dict["layer"]
    flake_entropy = flake_dict["entropy"]
    flake_proximity = flake_dict["proximity_stddev"]

    image_chip_id = image_dict["chip_id"]

    new_flake_dict = {
        "chip_id": image_chip_id,
        "position_x": flake_position_x,
        "position_y": flake_position_y,
        "size": flake_size,
        "thickness": flake_thickness,
        "entropy": flake_entropy,
        "proximity_stddev": flake_proximity,
    }

    full_meta_dict = {"flake": new_flake_dict, "images": {}}

    return full_meta_dict


def raster_scan_area_map_legacy(
    scan_directory: str,
    area_map,
    motor_driver: motor_driver_class,
    microscope_driver: microscope_driver_class,
    camera_driver: camera_driver_class,
    x_dimension: float = 0.7380,
    y_dimension: float = 0.4613,
    x_offset: float = 0,
    y_offset: float = 0,
    magnification: float = 20,
    wait_time: float = 0.2,
):
    """
    Rasters the supplied scan Area Map\\
    Used to Create a Dataset as it saves all the taken Images

    Args:
        scan_directory (str): The Directory where the Scan is Located
        area_map (NxMx1 Array): A Map of the Chips with where to scan
        motor_driver (motor_driver_class): The Motordriver
        microscope_driver (microscope_driver_class): The Microscope Driver
        camera_driver (camera_driver_class): The Camera Driver
        x_dimension (float, optional): the x Dimension of the 20x Picture. Defaults to 0.7380.
        y_dimension (float, optional): the y Dimension of the 20x Picture. Defaults to 0.4613.
        x_offset (float, optional): The x offset of the Scan Area Map in mm. Defaults to 0.
        y_offset (float, optional): The y offset of the Scan Area Map in mm. Defaults to 0.
        magnification (float, optional): The Magification with which the Chips are getting scanned, only for File Naming. Defaults to 20.
        wait_time (float, optional): The time to wait after moving before taking a picture in seconds. Defaults to 0.2.

    Returns:
        Tuple: Returns the Picture Directory and the Meta Directorey where the Image data is saved
    """

    # creating the folder structure
    magnification_dir, picture_dir, meta_dir = _create_folder_structure(
        scan_directory, magnification
    )

    # go into the 20x scope
    microscope_driver.set_mag(3)
    camera_driver.set_default_properties(3)
    microscope_driver.set_default_values()

    # get the camera and microscope pros as these wont change
    cam_props = camera_driver.get_properties()
    mic_props = microscope_driver.get_properties()

    num_images = cv2.countNonZero(area_map)

    curr_idx = 0

    for y_idx in range(area_map.shape[0]):

        # this implements a snake like pattern, its faster
        row = range(area_map.shape[1])
        if y_idx % 2 == 1:
            row = reversed(row)

        for x_idx in row:
            # Dont scan anything is the areamap is 0 as only 1s are beeing scanned
            if area_map[y_idx, x_idx] == 0:
                continue

            curr_idx += 1

            print(
                f"\r{curr_idx}/{num_images} scanned\r",
                end="",
                flush=True,
            )

            # Calculate the new Posiiton on the plate
            # Round to get rid of floating point errors
            # if you want check out https://www.youtube.com/watch?v=s9F8pu5KfyM
            x_pos = round(x_dimension * x_idx - x_offset, 3)
            y_pos = round(y_dimension * y_idx - y_offset, 3)

            # move to the new Position
            motor_driver.abs_move(x_pos, y_pos)

            # get the motor props
            motor_pos = motor_driver.get_pos()
            all_props = {
                **cam_props,
                **mic_props,
                "motor_pos": motor_pos,
                "chip_id": int(area_map[y_idx, x_idx]),
            }

            # Take image here
            if wait_time > 0:
                time.sleep(wait_time)

            image = camera_driver.get_image()

            # Do all Loads of stuff with the image
            # Run detection algorithm
            # If it detects something save the Flake positions in a Metadata File
            # as well as all metadate belonging to the image

            picture_path = os.path.join(picture_dir, f"{x_idx}_{y_idx}.png")
            cv2.imwrite(picture_path, image)

            # Save all the metadata in a JSON file
            json_path = os.path.join(meta_dir, f"{x_idx}_{y_idx}.json")
            with open(json_path, "w") as fp:
                json.dump(all_props, fp, sort_keys=True, indent=4)
    return picture_dir, meta_dir
