import PIL
import numpy as np
import matplotlib.pyplot as plt
import psycopg2
from config import config
import sys
import os

picture_path = os.path.join(os.path.dirname(__file__), "pictures")

flake_1 = {"thickness": "mono", "size": 42}
flake_2 = {"thickness": "few", "size": 54}
flake_3 = {"thickness": "thick", "size": 32}

flake_arr1 = [flake_1, flake_2, flake_3]
flake_arr2 = [flake_3]
flake_arr3 = []

all_flakes = [flake_arr1, flake_arr2, flake_arr3]


class dbConnection:
    def __init__(self):
        try:
            self.params = config()

            # connect to the PostgreSQL server
            print("Connecting to the PostgreSQL database...")
            self.conn = psycopg2.connect(**self.params)

            # create a cursor
            self.cur = self.conn.cursor()

        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            sys.exit(0)

    def insert_image(self, img_path, flake_array):
        query = """
        INSERT INTO image (path)
        VALUES (%s)
        RETURNING id
        """
        self.cur.execute(query, (img_path,))

        # gets the last image_id
        image_id = self.cur.fetchone()

        # unpacks the values of the flakes
        for flake in flake_array:
            self._insert_flake(image_id, **flake)

        print(f"inserted image and {len(flake_array)} flakes")

        # makes sure the databank is consistent
        self.conn.commit()

    def _insert_flake(self, image_id, thickness, size):
        query = """
        INSERT INTO flake (image_id,thickness,size)
        VALUES (%s,%s,%s)
        """
        self.cur.execute(
            query,
            (
                image_id,
                thickness,
                size,
            ),
        )

        # makes sure the databank is consistent
        self.conn.commit()

    def __delete__(self):
        if self.conn is not None:
            self.conn.close()
            print("Database connection closed.")


if __name__ == "__main__":
    db = dbConnection()

    for pic in os.listdir(picture_path):
        db.insert_image(pic, all_flakes[np.random.randint(3)])
