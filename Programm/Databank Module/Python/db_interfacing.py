import PIL
import numpy as np
import matplotlib.pyplot as plt
import psycopg2
import psycopg2.extras
from config import config
import sys
import os

picture_path = os.path.join(os.path.dirname(__file__), "Pictures")


class dbConnection:
    def __init__(self):
        try:
            self.params = config()

            # connect to the PostgreSQL server
            print("Connecting to the PostgreSQL database...")
            self.conn = psycopg2.connect(**self.params)

            # create a cursor
            self.cur = self.conn.cursor()
            self.return_cursor = self.conn.cursor(
                cursor_factory=psycopg2.extras.RealDictCursor
            )
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            sys.exit(0)

    def insert_image(self, img_path):
        """
        inserts an image into the database
        """
        query = """
        INSERT INTO image (path)
        VALUES (%s)
        RETURNING id
        """
        self.cur.execute(query, (img_path,))

        # gets the last image_id
        image_id = self.cur.fetchone()
        # makes sure the databank is consistent
        self.conn.commit()

        return image_id

    def insert_flake(self, thickness, size):
        """
        inserts a flake into the database
        """
        query = """
        INSERT INTO flake (thickness,size)
        VALUES (%s,%s)
        RETURNING id
        """
        self.cur.execute(
            query,
            (
                thickness,
                size,
            ),
        )
        # gets the last image_id
        flake_id = self.cur.fetchone()
        # makes sure the databank is consistent
        self.conn.commit()

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


def create_random_flakes():
    flake_arr = []
    for i in range(np.random.randint(6)):
        flake_dict = {}
        flake_dict["thickness"] = np.random.randint(1, 4)
        flake_dict["size"] = round(np.random.rand() * 300, 2)
        flake_arr.append(flake_dict)
    return flake_arr


def write_to_db_test(db):
    for pic in os.listdir(picture_path):
        image_id = db.insert_image(os.path.join(picture_path, pic))

        flake_arr = create_random_flakes()

        for flake in flake_arr:
            flake_id = db.insert_flake(**flake)
            db.create_relation(flake_id, image_id)


if __name__ == "__main__":
    db = dbConnection()

    # write_to_db_test(db)

    flakes = db.get_image(4030)
    for flake in flakes:
        print(flake["id"])
