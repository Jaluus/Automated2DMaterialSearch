import tisgrabber as IC
import cv2
import numpy as np
import time
import os
import json


def init_camera(cam_name="DFK 33UX174 38020321"):
    """
    Initiates the Camera Object and starts the Live Video\nreturns the Camera Object
    """
    # About 0.84 seconds per init
    # sometime 10.4 seconds

    # Create the camera object.
    Camera = IC.TIS_CAM()

    # Open a device with hard coded unique name:
    Camera.open(cam_name)

    # Set a video format
    Camera.SetVideoFormat("RGB24 (1920x1200)")

    # Set a frame rate
    Camera.SetFrameRate(13.5)

    # Start the live video stream, but show no own live video window. We will use OpenCV for this.
    Camera.StartLive(0)
    return Camera


def set_properties(Camera):
    """
    Sets currently only default values
    """
    # In order to set a fixed exposure time, the Exposure Automatic must be disabled first.
    # Using the IC Imaging Control VCD Property Inspector, we know, the item is "Exposure", the
    # element is "Auto" and the interface is "Switch". Therefore we use for disabling:

    # Disable all automatic changes
    # "0" is off, "1" is on.
    Camera.SetPropertySwitch("Exposure", "Auto", 0)
    Camera.SetPropertySwitch("Gain", "Auto", 0)
    Camera.SetPropertySwitch("WhiteBalance", "Auto", 0)

    # Set an absolute exposure time
    Camera.SetPropertyAbsoluteValue("Exposure", "Value", 0.030)

    Camera.SetPropertyValue("Gain", "Value", 0)

    Camera.SetPropertyValue("Gamma", "Value", 100)

    Camera.SetPropertyValue("WhiteBalance", "White Balance Red", 127)
    Camera.SetPropertyValue("WhiteBalance", "White Balance Green", 64)
    Camera.SetPropertyValue("WhiteBalance", "White Balance Blue", 90)
    return Camera


def get_properties(Camera):
    val_dict = {}
    # get the Gain
    val_dict["gain"] = Camera.GetPropertyValue("Gain", "Value")

    ExposureTime = [0]
    Camera.GetPropertyAbsoluteValue("Exposure", "Value", ExposureTime)
    val_dict["exposure"] = ExposureTime[0]

    val_dict["gamma"] = Camera.GetPropertyValue("Gamma", "Value")

    # Same goes with white balance. We make a complete red image:
    WBr = Camera.GetPropertyValue("WhiteBalance", "White Balance Red")
    WBg = Camera.GetPropertyValue("WhiteBalance", "White Balance Green")
    WBb = Camera.GetPropertyValue("WhiteBalance", "White Balance Blue")

    val_dict["white_balance_r"] = WBr
    val_dict["white_balance_g"] = WBg
    val_dict["white_balance_b"] = WBb

    return val_dict


# WBr:  127
# WBg:  64
# WBb:  90
# Gain: 0
# Exposure: -5
# Gamma: 100
# Offset: 0
# WBr: 127
# WBg: 64
# WBb: 90


Camera = init_camera()
print(get_properties(Camera))
Camera = set_properties(Camera)
print(get_properties(Camera))

while True:

    # Snap an image
    Camera.SnapImage()
    # Get the image
    image = Camera.GetImage()
    prop_dict = get_properties(Camera)
    # Apply some OpenCV function on this image

    # image = cv2.morphologyEx()

    image = cv2.flip(image, 0)

    image_small = cv2.resize(image.copy(), (960, 600))
    ######

    cv2.imshow("Window", image_small)

    key = cv2.waitKey(1)
    if key == ord("q"):
        break
    elif key == ord("s"):
        img_id = f"{time.time():.0f}"
        pic_path = os.path.join(os.path.dirname(__file__), "Pictures", f"{img_id}.png")
        # Write this data to the DB
        print("saving img... ")
        cv2.imwrite(pic_path, image)
    elif key == ord("e"):
        pass

Camera.StopLive()
cv2.destroyAllWindows()
