import sys
import os
import matplotlib.pyplot as plt
import cv2
import time

from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class

file_path = os.path.dirname(__file__)

motor = motor_driver_class()
microscope = microscope_driver_class()
camera = camera_driver_class()

while True:

    img = camera.get_image()

    img_small = cv2.resize(img, (960, 600))

    # status = microscope.get_af_status()

    # print(status)
    # if status == 9:
    #     print("searc")
    #     microscope.find_af()

    # print(all_props)
    cv2.imshow("test", img_small)

    key = cv2.waitKey(1)
    if key == ord("q"):
        break

    # Save the Image
    elif key == ord("s"):
        motor_pos = motor.get_pos()
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props, "motor_pos": motor_pos}

        print(all_props)
        picture_path = os.path.join(file_path, f"{motor_pos}.png")
        cv2.imwrite(picture_path, img)
        # Save image to server and write the path and the metadata to DB

    elif key == ord("e"):
        microscope.rotate_nosepiece_forward()

    elif key == ord("r"):
        microscope.rotate_nosepiece_backward()