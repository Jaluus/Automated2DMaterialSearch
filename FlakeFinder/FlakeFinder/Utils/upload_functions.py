import os
import cv2
import requests
from FlakeFinder.Utils.raster_functions import get_flake_directorys
import json
import os
import time
import shutil


def upload_directory_legacy(scan_dir, url):
    flake_directorys = get_flake_directorys(scan_dir)

    start = time.time()

    # Sending Scan Metadata
    file_names = [
        file_name
        for file_name in os.listdir(scan_dir)
        if os.path.isfile(os.path.join(scan_dir, file_name))
    ]

    files = {
        file_name: open(os.path.join(scan_dir, file_name), "rb")
        for file_name in file_names
    }

    # Extract the Shortend Scan Directory
    path = os.path.normpath(scan_dir)
    short_scan_directory = "/".join(path.split(os.sep)[-1])

    r = requests.post(
        url,
        files=files,
        data={"path": short_scan_directory},
    )

    # Sending all the flakes
    for flake_directory in flake_directorys:
        # Extract the Flake Path and where to save it
        path = os.path.normpath(flake_directory)
        flake_path = "/".join(path.split(os.sep)[-4:])

        file_names = os.listdir(flake_directory)
        files = {
            file_name: open(os.path.join(flake_directory, file_name), "rb")
            for file_name in file_names
        }

        r = requests.post(
            url,
            files=files,
            data={"path": flake_path},
        )

    print(time.time() - start)


def upload_directory(scan_dir, url):
    """Creates a Zipfile which is being uploaded

    Args:
        scan_dir ([type]): [description]
        url ([type]): [description]
    """
    shutil.make_archive(scan_dir, "zip", scan_dir)

    with open(scan_dir + ".zip", "rb") as f:
        requests.post(url, files={"zip": f})
