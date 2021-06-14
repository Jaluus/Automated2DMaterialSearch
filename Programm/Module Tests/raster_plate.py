import sys
import os
import matplotlib.pyplot as plt
import cv2
import time
import json

file_path = os.path.dirname(__file__)

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from Modules.Camera_Module.camera_class import microscope_cam
from Modules.Microscope_Module.microscope_class import microscope_control
from Modules.Motor_Module.tango_class import tango_controller

motor = tango_controller()
microscope = microscope_control()
camera = microscope_cam()

# real x view field : 5.9048 mm
# real y view field : 3.6905 mm

x_dist = 5
y_dist = 3.333

# => 18.1 % x-overlap
# => 10.7 % y-overlap

# => 21 rows
# => 31 columns

motor.abs_move(0, 0)

can_move_x = motor.can_move(x_dist, 0)
can_move_y = motor.can_move(0, y_dist)

curr_idx = 0

microscope.set_mag(1)

while can_move_x:
    while can_move_y:
        curr_idx += 1
        print("{}/651 scanned".format(curr_idx))
        # # restart the AF when its off
        # if microscope.get_af_status()() == 9:
        #     microscope.find_af()
        #     print("searching...")
        #     time.sleep(1)

        # # Check if img is in focus, also abort if it takes to long to find the focus
        # af_ctr = 0
        # while (microscope.get_af_status()() != 1) and (af_ctr <= 30):
        #     time.sleep(0.1)
        #     af_ctr += 1

        # Take image here
        time.sleep(0.1)
        img = camera.get_image()

        # get all the proeprties
        motor_pos = motor.get_pos()
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props, "motor_pos": motor_pos}

        picture_path = os.path.join(file_path, "Pictures", f"{curr_idx}.png")
        cv2.imwrite(picture_path, img)

        # Save all the metadata in a JSON file
        json_path = os.path.join(file_path, "Meta", f"{curr_idx}.json")
        with open(json_path, "w") as fp:
            json.dump(all_props, fp, sort_keys=True, indent=4)

        # Image aquired

        can_move_y = motor.rel_move(0, y_dist)
    curr_x, curr_y = motor.get_pos()
    motor.abs_move(curr_x, 0)
    can_move_x = motor.rel_move(x_dist, 0)
    can_move_y = motor.can_move(0, y_dist)
