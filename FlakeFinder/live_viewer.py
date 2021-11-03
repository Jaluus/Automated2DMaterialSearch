import sys
import os
import matplotlib.pyplot as plt
import cv2
import time
from Detector.detector_functions import remove_vignette

from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import (
    microscope_driver_class,
)
from Drivers.Motor_Driver.tango_class import motor_driver_class

file_path = os.path.dirname(os.path.abspath(__file__))
ff_path = r"FlakeFinder\Parameters\Flatfields\wse2_90nm_50x.png"

# motor = motor_driver_class()
microscope = microscope_driver_class()
camera = camera_driver_class()

cam_params = {
    "exposure": 0.05,
    "gain": 100,
    "white_balance": [127, 64, 90],
    "gamma": 100,
}

VOLTAGE = 6.3
APERTURE = 3

MAG_KEYS = {
    1: "2.5x",
    2: "5x",
    3: "20x",
    4: "50x",
    5: "100x",
}

flat_field = cv2.imread(ff_path)

microscope.set_lamp_voltage(VOLTAGE)
microscope.set_lamp_aperture_stop(APERTURE)

camera.set_properties(
    exposure=0.05,
    gain=100,
    white_balance=(127, 64, 90),
    gamma=100,
)

cv2.namedWindow("Calibration Window")
curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

while True:

    img = camera.get_image()

    img = remove_vignette(img, flat_field)

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
        break

    # Save the Image
    elif key == ord("i"):
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props}
        print(all_props)

    elif key == ord("s"):
        # motor_pos = motor.get_pos()
        # print(motor_pos)
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props}
        print(all_props)

        picture_path = os.path.join(file_path, f"{time.time()}.png")
        cv2.imwrite(picture_path, img)

    elif key == ord("e"):
        microscope.rotate_nosepiece_forward()
        microscope.set_lamp_voltage(VOLTAGE)
        microscope.set_lamp_aperture_stop(APERTURE)
        curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
        cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")

    elif key == ord("r"):
        microscope.rotate_nosepiece_backward()
        microscope.set_lamp_voltage(VOLTAGE)
        microscope.set_lamp_aperture_stop(APERTURE)
        curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
        cv2.setWindowTitle("Calibration Window", f"Calibration Window: {curr_mag}")


cv2.destroyAllWindows()
