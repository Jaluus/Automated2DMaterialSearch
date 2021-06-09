import PIL
import numpy as np
import matplotlib.pyplot as plt
import mysql.connector
from config import config
import sys
import os

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

    def insert_image(self, chip_id, img_path):
        """
        inserts an image into the database, returns the generated id of the image
        """
        query = """
        INSERT INTO image (chip_id,path)
        VALUES (%s,%s)
        """
        self.cur.execute(
            query,
            (
                chip_id,
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

    def create_relation(self, flake_id, image_id):
        """
        creates a relation between a flake and an image via ID
        """
        query = """
        INSERT INTO relation_table (image_id,flake_id)
        VALUES (%s,%s)
        """
        self.cur.execute(
            query,
            (image_id, flake_id),
        )

        # makes sure the databank is consistent
        self.conn.commit()

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
    flake_arr = []
    for i in range(np.random.randint(6)):
        flake_dict = {}
        flake_dict["chip_id"] = chip_id
        flake_dict["thickness"] = np.random.randint(1, 4)
        flake_dict["size"] = round(np.random.rand() * 300, 2)
        flake_dict["used"] = np.random.randint(2) == 1
        flake_arr.append(flake_dict)
    return flake_arr


def write_mock_data_to_db(db):
    for pic in os.listdir(picture_path):

        chip_id = np.random.randint(2) + 1
        image_id = db.insert_image(chip_id, os.path.join(picture_path, pic))

        flake_arr = create_random_flakes(chip_id)

        for flake in flake_arr:
            flake_id = db.insert_flake(**flake)
            db.create_relation(flake_id, image_id)


if __name__ == "__main__":
    db = dbConnection()

    write_mock_data_to_db(db)
