# -*- coding: utf-8 -*-
"""
Created on Mon Nov 21 09:46:46 2016

Sample for tisgrabber to OpenCV Sample 2

Open a camera by name
Set a video format hard coded (not recommended, but some peoples insist on this)
Set properties exposure, gain, whitebalance
"""
import ctypes as C
import tisgrabber as IC
import cv2
import numpy as np
import sys

lWidth = C.c_long()
lHeight = C.c_long()
iBitsPerPixel = C.c_int()
COLORFORMAT = C.c_int()


def set_properties(Camera):
    ExposureAuto = [1]

    Camera.GetPropertySwitch("Exposure", "Auto", ExposureAuto)
    print("Exposure auto : ", ExposureAuto[0])

    # In order to set a fixed exposure time, the Exposure Automatic must be disabled first.
    # Using the IC Imaging Control VCD Property Inspector, we know, the item is "Exposure", the
    # element is "Auto" and the interface is "Switch". Therefore we use for disabling:
    Camera.SetPropertySwitch("Exposure", "Auto", 0)
    # "0" is off, "1" is on.

    ExposureTime = [0]
    Camera.GetPropertyAbsoluteValue("Exposure", "Value", ExposureTime)
    print("Exposure time abs: ", ExposureTime[0])

    # Set an absolute exposure time, given in fractions of seconds. 0.0303 is 1/30 second:
    Camera.SetPropertyAbsoluteValue("Exposure", "Value", 0.0303)

    # Proceed with Gain, since we have gain automatic, disable first. Then set values.
    Gainauto = [0]
    Camera.GetPropertySwitch("Gain", "Auto", Gainauto)
    print("Gain auto : ", Gainauto[0])

    Camera.SetPropertySwitch("Gain", "Auto", 0)
    Camera.SetPropertyValue("Gain", "Value", 10)

    WhiteBalanceAuto = [0]
    # Same goes with white balance. We make a complete red image:
    Camera.SetPropertySwitch("WhiteBalance", "Auto", 1)
    Camera.GetPropertySwitch("WhiteBalance", "Auto", WhiteBalanceAuto)
    print("WB auto : ", WhiteBalanceAuto[0])

    Camera.SetPropertySwitch("WhiteBalance", "Auto", 0)
    Camera.GetPropertySwitch("WhiteBalance", "Auto", WhiteBalanceAuto)
    print("WB auto : ", WhiteBalanceAuto[0])

    Camera.SetPropertyValue("WhiteBalance", "White Balance Red", 64)
    Camera.SetPropertyValue("WhiteBalance", "White Balance Green", 64)
    Camera.SetPropertyValue("WhiteBalance", "White Balance Blue", 64)
    return Camera


# Create the camera object.
Camera = IC.TIS_CAM()
# Open a device with hard coded unique name:
Camera.open("DFK 33UX174 38020321")

# or show the IC Imaging Control device page:
# Camera.ShowDeviceSelectionDialog()

if Camera.IsDevValid() != 1:
    print("No device selected")
    sys.exit(0)

# Set a video format
Camera.SetVideoFormat("RGB24 (1920x1200)")

# Set a frame rate of 30 frames per second
Camera.SetFrameRate(13.5)

# Start the live video stream, but show no own live video window. We will use OpenCV for this.
Camera.StartLive(0)

try:
    while (True):
        # Snap an image
        Camera.SnapImage()
        # Get the image
        image = Camera.GetImage()

        # Apply some OpenCV function on this image

        image = cv2.flip(image, 0)
        # image = cv2.morphologyEx(
        #    image, cv2.MORPH_GRADIENT, np.ones((3, 3)))

        cv2.imshow('Window', image)

        key = cv2.waitKey(1)
        if key == ord("q"):
            raise KeyboardInterrupt

except KeyboardInterrupt:
    Camera.StopLive()
    cv2.destroyAllWindows()
