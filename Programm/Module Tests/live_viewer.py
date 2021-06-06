import sys
import os
import matplotlib.pyplot as plt
import cv2

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

while True:

    img = camera.get_image()

    img_small = cv2.resize(img, (960, 600))

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
        picture_path = os.path.join(file_path, f"Pictures\\{motor_pos}.png")
        cv2.imwrite(picture_path, img)
        # Save image to server and write the path and the metadata to DB

    elif key == ord("e"):
        microscope.rotate_nosepiece_forward()

    elif key == ord("r"):
        microscope.rotate_nosepiece_backward()
