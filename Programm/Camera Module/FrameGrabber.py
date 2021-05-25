import tisgrabber as IC
import cv2
import numpy as np
import time


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


Camera = init_camera()

while (True):

    # Snap an image
    Camera.SnapImage()
    # Get the image
    image = Camera.GetImage()

    # Apply some OpenCV function on this image

    #image = cv2.morphologyEx()

    image = cv2.flip(image, 0)

    ######

    cv2.imshow('Window', image)

    key = cv2.waitKey(1)
    if key == ord("q"):
        break
    elif key == ord("s"):
        cv2.imwrite(f"Pictures/{time.time():.0f}.png", image)

Camera.StopLive()
cv2.destroyAllWindows()
