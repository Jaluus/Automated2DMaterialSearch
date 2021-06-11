import sys
import os
import matplotlib.pyplot as plt
import cv2
import time
import json

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
from Modules.Camera_Module.camera_class import microscope_cam
from Modules.Microscope_Module.microscope_class import microscope_control
from Modules.Motor_Module.tango_class import tango_controller

motor = tango_controller()
microscope = microscope_control()
camera = microscope_cam()

file_path = os.path.dirname(__file__)
area_map = cv2.imread(os.path.join(file_path, "scan_area_labeled.png"), 0)


# real x view field 2.5x : 5.9048 mm
# real y view field 2.5x : 3.6905 mm
# Real 20x area (0.7280 x 0.4613)

x_offset = 1
y_offset = 0

x_factor = 0.7280
y_factor = 0.4613

motor.abs_move(0, 0)

curr_idx = 0
file_path = os.path.dirname(__file__)

for y_idx in range(area_map.shape[0]):
    for x_idx in range(area_map.shape[1]):
        if area_map[y_idx, x_idx] == 0:
            continue
        curr_idx += 1
        print(curr_idx)
        x_pos = x_factor * x_idx - x_offset
        y_pos = y_factor * y_idx - y_offset

        motor.abs_move(x_pos, y_pos)

        # get all the proeprties
        motor_pos = motor.get_pos()
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {
            **cam_props,
            **mic_props,
            "motor_pos": motor_pos,
            "chip_id": int(area_map[y_idx, x_idx]),
        }

        # Take image here
        time.sleep(0.2)
        img = camera.get_image()

        picture_path = os.path.join(file_path, "20xPictures", f"{x_idx}_{y_idx}.png")
        cv2.imwrite(picture_path, img)

        # Save all the metadata in a JSON file
        json_path = os.path.join(file_path, "20xMeta", f"{x_idx}_{y_idx}.json")
        with open(json_path, "w") as fp:
            json.dump(all_props, fp, sort_keys=True, indent=4)
