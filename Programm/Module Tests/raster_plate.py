import sys
import os
import matplotlib.pyplot as plt
import cv2
import time
import json

scan_path = r"C:\Users\duden\Desktop\Mikroskop Bilder\WSe2\FullScanE"

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from Modules.Camera_Module.camera_class import camera_driver_class
from Modules.Microscope_Module.microscope_class import microscope_driver_class
from Modules.Motor_Module.tango_class import motor_driver_class


def raster_plate(
    scan_path,
    motor_driver,
    microscope_driver,
    camera_driver,
):
    """
    Scan path is the directory in which the scan is being saved
    """

    # real x view field : 5.9048 mm
    # real y view field : 3.6905 mm

    x_dist = 5
    y_dist = 3.333

    # => 18.1 % x-overlap
    # => 10.7 % y-overlap

    # => 21 rows
    # => 31 columns

    # Start at 00 and make sure to be in 2.5x Mag
    motor_driver.abs_move(0, 0)
    microscope_driver.set_mag(1)

    # Checks if it is actually at 00
    can_move_x = motor_driver.can_move(x_dist, 0)
    can_move_y = motor_driver.can_move(0, y_dist)

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
            time.sleep(0.1)
            img = camera_driver.get_image()

            picture_path = os.path.join(
                scan_path, "2.5x", "Pictures", f"{curr_idx}.png"
            )
            cv2.imwrite(picture_path, img)

            # Save all the metadata in a JSON file
            json_path = os.path.join(scan_path, "2.5x", "Meta", f"{curr_idx}.json")
            with open(json_path, "w") as fp:
                json.dump(all_props, fp, sort_keys=True, indent=4)

            can_move_y = motor_driver.rel_move(0, y_dist)

        curr_x, curr_y = motor_driver.get_pos()
        motor_driver.abs_move(curr_x, 0)
        can_move_x = motor_driver.rel_move(x_dist, 0)
        can_move_y = motor_driver.can_move(0, y_dist)


if __name__ == "__main__":

    motor_driver = motor_driver_class()
    camera_driver = camera_driver_class()
