import json
import os
from sys import prefix
from FlakeFinder.Utils.raster_functions import get_flake_directorys

# How to send to DB
# 1. Create a new Scan in the DB and get the ID
# 2. Create a new Chip
# 3. Order the flakes into the chip
# go to step 2. for the next chip


def kek():
    print("lel")


scan_name = "Eikes_Flocken_All"
material = "Graphene"

image_dir = r"C:\Users\duden\Desktop\Mikroskop Bilder"
scan_directory = os.path.join(image_dir, material, scan_name)

flake_directorys = get_flake_directorys(
    scan_directory, callback_function=lambda: print("Hallo")
)

for flake_directory in flake_directorys:
    meta_path = os.path.join(flake_directory, "meta.json")

    with open(meta_path) as f:
        meta_data = json.load(f)

    print(meta_data["flake"]["chip_id"])
