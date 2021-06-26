import sys
import os
import cv2
import time
import json

scan_directory = r"C:\Users\duden\Desktop\Mikroskop Bilder\WSe2\FullScanE"

sys.path.append(os.path.join(os.path.dirname(__file__), "..", ".."))
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
    magnification: float = 2.5,
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

    # creates the folder stucture
    magnification_dir, picture_dir, meta_dir = _create_folder_structure(
        scan_directory, magnification
    )

    # Start at 00 and make sure to be in 2.5x Mag
    motor_driver.abs_move(0, 0)
    microscope_driver.set_mag(1)

    # Checks if it is actually at 00
    can_move_x = motor_driver.can_move(x_step, 0)
    can_move_y = motor_driver.can_move(0, y_step)

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
            cam_props = camera_driver.get_properties()
            mic_props = microscope_driver.get_properties()
            all_props = {**cam_props, **mic_props, "motor_pos": motor_pos}

            # Take image here wait 100ms to stabilze the camera
            time.sleep(wait_time)
            img = camera_driver.get_image()

            picture_path = os.path.join(picture_dir, f"{curr_idx}.png")
            cv2.imwrite(picture_path, img)

            # Save all the metadata in a JSON file
            json_path = os.path.join(meta_dir, f"{curr_idx}.json")
            with open(json_path, "w") as fp:
                json.dump(all_props, fp, sort_keys=True, indent=4)

            can_move_y = motor_driver.rel_move(0, y_step)

        curr_x, curr_y = motor_driver.get_pos()
        motor_driver.abs_move(curr_x, 0)
        can_move_x = motor_driver.rel_move(x_step, 0)
        can_move_y = motor_driver.can_move(0, y_step)
    return picture_dir, meta_dir


def raster_scan_area(
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
    real x view field 2.5x : 5.9048 mm\n
    real y view field 2.5x : 3.6905 mm\n
    Real 20x area (0.7380 mm x 0.4613 mm)\n
    @ resolution 2.5x of  3.0754 μm/px\n
    @ resolution 20x of ~ 0.3844 μm/px\n
    We have an offset of about (856 px x 379 px)\n
    """

    # creating the folder structure
    magnification_dir, picture_dir, meta_dir = _create_folder_structure(
        scan_directory, magnification
    )

    # go into the 20x scope
    microscope_driver.set_mag(3)

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

            img = camera_driver.get_image()

            picture_path = os.path.join(picture_dir, f"{x_idx}_{y_idx}.png")
            cv2.imwrite(picture_path, img)

            # Save all the metadata in a JSON file
            json_path = os.path.join(meta_dir, f"{x_idx}_{y_idx}.json")
            with open(json_path, "w") as fp:
                json.dump(all_props, fp, sort_keys=True, indent=4)
    return picture_dir, meta_dir