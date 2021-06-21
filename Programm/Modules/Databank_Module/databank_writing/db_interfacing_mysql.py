import PIL
import numpy as np
import matplotlib.pyplot as plt
import mysql.connector
from config import config
import sys
import os
import time

picture_path = os.path.join(os.path.dirname(__file__), "Pictures")


class dbConnection:
    def __init__(self):
        try:
            self.params = config()
            print(self.params)

            # connect to the MySQL server
            print("Connecting to the MySQL database...")
            self.conn = mysql.connector.connect(**self.params)

            # create a cursor
            self.cur = self.conn.cursor()
        except (Exception) as error:
            print(error)
            sys.exit(0)

    def insert_scan(self):
        """
        inserts a scan into the database, returns the generated id of the scan
        """

        curr_time = int(time.time())

        query = """
        INSERT INTO scan (time)
        VALUES (%s)
        """
        self.cur.execute(
            query,
            (curr_time,),
        )

        # gets the last image_id
        # makes sure the databank is consistent
        self.conn.commit()

        scan_id = self.cur.lastrowid
        return scan_id

    def insert_chip(self, scan_id, material):
        """
        inserts a scan into the database, returns the generated id of the scan
        """

        query = """
        INSERT INTO chip (scan_id,material)
        VALUES (%s,%s)
        """
        self.cur.execute(
            query,
            (scan_id, material),
        )

        # gets the last image_id
        # makes sure the databank is consistent
        self.conn.commit()

        chip_id = self.cur.lastrowid
        return chip_id

    def insert_image(self, flake_id, img_path):
        """
        inserts an image into the database, returns the generated id of the image
        """
        query = """
        INSERT INTO image (flake_id,path)
        VALUES (%s,%s)
        """
        self.cur.execute(
            query,
            (
                flake_id,
                img_path,
            ),
        )

        # gets the last image_id
        # makes sure the databank is consistent
        self.conn.commit()

        image_id = self.cur.lastrowid

        return image_id

    def insert_flake(self, chip_id, thickness, size, used):
        """
        inserts a flake into the database
        """
        query = """
        INSERT INTO flake (chip_id,thickness,size,used)
        VALUES (%s,%s,%s,%s)
        """
        self.cur.execute(
            query,
            (
                chip_id,
                thickness,
                size,
                used,
            ),
        )
        # gets the last image_id
        # makes sure the databank is consistent
        self.conn.commit()

        flake_id = self.cur.lastrowid

        return flake_id

    def get_image(self, image_id):
        query = """
        SELECT * FROM image
        JOIN relation_table ON image.id = relation_table.image_id
        JOIN flake on flake.id = relation_table.flake_id
        WHERE image.id = %s;
        """

        self.return_cursor.execute(
            query,
            (image_id,),
        )
        # gets the last image_id
        image_dict = self.return_cursor.fetchall()
        # makes sure the databank is consistent
        self.conn.commit()

        return image_dict

    def __delete__(self):
        if self.conn is not None:
            self.conn.close()
            print("Database connection closed.")


def create_random_flakes(chip_id):
    """
    chreats an array of up to 6 random flakes
    """
    flake_arr = []
    for i in range(np.random.randint(6)):
        flake_arr.append(create_random_flake(chip_id))
    return flake_arr


def create_random_flake(chip_id):
    flake_dict = {}
    flake_dict["chip_id"] = chip_id
    flake_dict["thickness"] = np.random.randint(1, 4)
    flake_dict["size"] = round(np.random.rand() * 300, 2)
    flake_dict["used"] = np.random.randint(2) == 1
    return flake_dict


def write_mock_data_to_db(db):
    chip_materials = ["C", "WSe2", "WTe2"]

    magnification = [20, 50, 100]

    scan_id = db.insert_scan()

    chip_ids = []
    for i in range(np.random.randint(2, 5)):

        material = str(np.random.choice(chip_materials))
        chip_ids.append(db.insert_chip(scan_id, material))

    img_paths = os.listdir(picture_path)
    num_images = len(img_paths)
    img_idx = 0

    while True:
        chip_id = int(np.random.choice(chip_ids))
        flake = create_random_flake(chip_id)

        flake_id = db.insert_flake(**flake)

        images_for_flake = np.random.randint(1, 4)
        if img_idx + images_for_flake >= num_images:
            break

        for i in range(images_for_flake):
            db.insert_image(flake_id, os.path.join(picture_path, img_paths[img_idx]))
            img_idx += 1


def write_mock_data_to_db_legecy(db):
    chip_materials = ["C", "WSe2", "WTe2"]

    scan_id = db.insert_scan()

    chip_ids = []
    for i in range(np.random.randint(2, 5)):

        material = str(np.random.choice(chip_materials))
        chip_ids.append(db.insert_chip(scan_id, material))

    for pic in os.listdir(picture_path):

        chip_id = int(np.random.choice(chip_ids))
        image_id = db.insert_image(chip_id, os.path.join(picture_path, img_paths))
        flake_arr = create_random_flakes(chip_id)

        for flake in flake_arr:
            flake_id = db.insert_flake(**flake)
            db.create_relation(flake_id, image_id)


if __name__ == "__main__":
    db = dbConnection()

    print("Writing Mock Data...")
    write_mock_data_to_db(db)
