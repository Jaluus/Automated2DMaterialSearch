import os
import cv2
import time

from Detector.detector_functions import remove_vignette

from Drivers.Camera_Driver.camera_class import camera_driver_class
from Drivers.Microscope_Driver.microscope_class import (
    microscope_driver_class,
)

file_path = os.path.dirname(os.path.abspath(__file__))
ff_path = r"FlakeFinder\Parameters\Flatfields\graphene_90nm_20x.png"

# motor = motor_driver_class()
microscope = microscope_driver_class()
camera = camera_driver_class()

VOLTAGE = 6.3
APERTURE = 3
EXPOSURE = 0.05
GAIN = 100
WHITE_BALANCE = (127, 64, 90)
GAMMA = 100
CALIBRATION_TARGET_GRAY_VALUE = 213

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
    exposure=EXPOSURE,
    gain=GAIN,
    white_balance=WHITE_BALANCE,
    gamma=GAMMA,
)

cv2.namedWindow("Live Viewer Window")
curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
cv2.setWindowTitle("Live Viewer Window", f"Live Viewer Window: {curr_mag}")

use_ff = True

while True:

    img = camera.get_image()

    if use_ff:
        img = remove_vignette(img, flat_field)

    img_small = cv2.resize(img, (960, 600))

    cv2.circle(
        img_small,
        (480, 300),
        10,
        color=[255, 0, 0],
        thickness=3,
    )

    cv2.imshow("Live Viewer Window", img_small)

    key = cv2.waitKey(1)
    if key == ord("q"):
        break

    # Get the current information about the image
    elif key == ord("i"):
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props}
        print("Properties of the camera and microscope")
        print(all_props)
        new_img = camera.get_image()
        value_img = cv2.cvtColor(new_img, cv2.COLOR_BGR2HSV)[:, :, 2]
        mean = cv2.mean(value_img)
        print("Mean gray value of image")
        print(round(mean[0], 2))

    elif key == ord("k"):
        use_ff = not use_ff

    elif key == ord("s"):
        cam_props = camera.get_properties()
        mic_props = microscope.get_properties()
        all_props = {**cam_props, **mic_props}
        print(all_props)

        picture_path = os.path.join(
            file_path,
            f"{VOLTAGE:.1f}_{APERTURE:.1f}_{EXPOSURE:.2f}_{GAIN:.0f}_{int(time.time())}.png",
        )
        cv2.imwrite(picture_path, img)

    elif key == ord("e"):
        microscope.rotate_nosepiece_forward()
        microscope.set_lamp_voltage(VOLTAGE)
        microscope.set_lamp_aperture_stop(APERTURE)
        curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
        cv2.setWindowTitle("Live Viewer Window", f"Live Viewer Window: {curr_mag}")

    elif key == ord("c"):
        time.sleep(0.5)
        current_iteration = 0
        max_iter = 40

        # get an image and check the current Gray Value
        new_img = camera.get_image()
        value_img = cv2.cvtColor(new_img, cv2.COLOR_BGR2HSV)[:, :, 2]
        mean_gray_value = round(cv2.mean(value_img)[0], 0)

        while (
            mean_gray_value != CALIBRATION_TARGET_GRAY_VALUE
            and current_iteration < max_iter
        ):
            current_iteration += 1
            delta_gray = round(CALIBRATION_TARGET_GRAY_VALUE - mean_gray_value, 0)
            print(
                f"\r\x1b[KThe current graydelta is {delta_gray} after {current_iteration} steps",
                end="",
            )

            # Scale the steps acording to the delta of the target and the current values
            # This leads to an incremental approach
            EXPOSURE += 0.0001 * delta_gray  # * (1.1 - current_iteration / max_iter)
            camera.set_properties(exposure=EXPOSURE)
            time.sleep(0.5)

            new_img = camera.get_image()
            value_img = cv2.cvtColor(new_img, cv2.COLOR_BGR2HSV)[:, :, 2]
            mean_gray_value = round(cv2.mean(value_img)[0], 0)

        print(f"\r\x1b[KFinished | Calibrated Exposure: {round(EXPOSURE,4)}s")

    elif key == ord("o"):
        EXPOSURE += 0.01
        camera.set_properties(exposure=EXPOSURE)
        time.sleep(0.5)
        new_img = camera.get_image()
        value_img = cv2.cvtColor(new_img, cv2.COLOR_BGR2HSV)[:, :, 2]
        mean = cv2.mean(value_img)
        print(mean)

    elif key == ord("l"):
        EXPOSURE -= 0.01
        camera.set_properties(exposure=EXPOSURE)
        time.sleep(0.5)
        new_img = camera.get_image()
        value_img = cv2.cvtColor(new_img, cv2.COLOR_BGR2HSV)[:, :, 2]
        mean = cv2.mean(value_img)
        print(mean)

    elif key == ord("r"):
        microscope.rotate_nosepiece_backward()
        microscope.set_lamp_voltage(VOLTAGE)
        microscope.set_lamp_aperture_stop(APERTURE)
        curr_mag = MAG_KEYS[microscope.get_properties()["nosepiece"]]
        cv2.setWindowTitle("Live Viewer Window", f"Live Viewer Window: {curr_mag}")


cv2.destroyAllWindows()
