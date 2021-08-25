"""
This is a debug file, only run it if you know what you are doing.
"""
import os
import Utils.upload_functions as uploader

SERVER_URL = "http://134.61.6.112:5000/upload"
IMAGE_DIRECTORY = r"C:\Users\Transfersystem User\Documents\Mic_bilder"
SCAN_NAME = "graphene_210823"

scan_directory = os.path.join(IMAGE_DIRECTORY, SCAN_NAME)

uploader.upload_directory(scan_directory, SERVER_URL)
