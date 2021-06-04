import sys
import os
import matplotlib.pyplot as plt
import cv2
import time
import json

# Rastering the plate at 2.5 X

file_path = os.path.dirname(__file__)

os.chdir(os.path.dirname(__file__))

# Camera_Class
cam_path = os.path.abspath(r"..\Camera Module")
sys.path.append(cam_path)

# microscope_class
micro_path = os.path.abspath(r"..\Microscope Module")
sys.path.append(micro_path)

# motor_class
motor_path = os.path.abspath(r"..\Motor Module")
sys.path.append(motor_path)

from camera_class import microscope_cam
from microscope_class import microscope_control
from tango_model import tango_controller


motor = tango_controller()
microscope = microscope_control()
camera = microscope_cam()

x_dist = 4.5024
y_dist = 2.8140

can_move_x = motor.can_move(x_dist, 0)
can_move_y = motor.can_move(0, y_dist)

motor.abs_move(0, 0)

while can_move_x:
    while can_move_y:

        # Take image here

        # Check if img is in focus, also abort if it takes to long to find the focus
        af_ctr = 0
        while microscope.get_af_status() != 1 or af_ctr < 30:
            time.sleep(0.01)
            af_ctr += 1

        img = camera.get_image()

        # get all the proeprties
        motor_pos = motor.get_pos()
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props, "motor_pos": motor_pos}

        picture_path = os.path.join(file_path, f"Pictures\\{motor_pos}.png")
        cv2.imwrite(picture_path, img)

        json_path = os.path.join(file_path, f"Meta\\{motor_pos}.json")
        with open(json_path, "w") as fp:
            json.dump(all_props, fp, sort_keys=True, indent=4)

        # Image aquired

        can_move_y = motor.rel_move(0, y_dist)
    curr_x, curr_y = motor.get_pos()
    motor.abs_move(curr_x, 0)
    can_move_x = motor.rel_move(x_dist, 0)
    can_move_y = motor.can_move(0, y_dist)
