import PIL
import numpy as np
import matplotlib.pyplot as plt
import mysql.connector
from Drivers.Databank_Driver.databank_writing.config import config
import sys
import os
import time

picture_path = os.path.join(os.path.dirname(__file__), "Pictures")


class dbConnection:
    def __init__(self) -> None:
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

    def insert_scan(
        self,
        time: float,
        user: str,
        name: str,
    ) -> int:
        """
        inserts a scan into the database, returns the generated id of the scan
        """

        query = """
        INSERT INTO scan (time,user,name)
        VALUES (%s,%s,%s)
        """
        self.cur.execute(
            query,
            (time, user, name),
        )

        # makes sure the databank is consistent
        self.conn.commit()

        # gets the last scan_id
        scan_id = self.cur.lastrowid
        return scan_id

    def insert_chip(
        self,
        scan_id: int,
        exfoliated_material: str,
        chip_thickness: int,
    ) -> int:
        """
        inserts a scan into the database, returns the generated id of the scan
        """

        query = """
        INSERT INTO chip (scan_id,exfoliated_material,chip_thickness)
        VALUES (%s,%s,%s)
        """
        self.cur.execute(
            query,
            (scan_id, exfoliated_material, chip_thickness),
        )

        # makes sure the databank is consistent
        self.conn.commit()

        # gets the last chip_id
        chip_id = self.cur.lastrowid
        return chip_id

    def insert_flake(
        self,
        chip_id: int,
        thickness: str,
        size: float,
        used: bool,
        entropy: float,
    ) -> int:
        """
        inserts a flake into the database
        """

        query = """
        INSERT INTO flake (chip_id,thickness,size,used,entropy)
        VALUES (%s,%s,%s,%s,%s)
        """
        self.cur.execute(
            query,
            (
                chip_id,
                thickness,
                size,
                used,
                entropy,
            ),
        )
        # makes sure the databank is consistent
        self.conn.commit()

        # gets the last flake_id
        flake_id = self.cur.lastrowid

        return flake_id

    def insert_image(
        self,
        flake_id: int,
        path: str,
        aperture: float,
        light_voltage: float,
        magnification: float,
        white_balance_r: int,
        white_balance_g: int,
        white_balance_b: int,
        gain: float,
        exposure_time: float,
    ) -> int:
        """
        inserts an image into the database, returns the generated id of the image
        """
        query = """
        INSERT INTO image (flake_id,path,aperture,light_voltage,magnification,white_balance_r,white_balance_g,white_balance_b,gain,exposure_time)
        VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)
        """
        self.cur.execute(
            query,
            (
                flake_id,
                path,
                aperture,
                light_voltage,
                magnification,
                white_balance_r,
                white_balance_g,
                white_balance_b,
                gain,
                exposure_time,
            ),
        )

        # makes sure the databank is consistent
        self.conn.commit()

        # gets the last image_id
        image_id = self.cur.lastrowid

        return image_id

    def __delete__(self):
        if self.conn is not None:
            self.conn.close()
            print("Database connection closed.")


if __name__ == "__main__":
    db = dbConnection()

    print("Writing Mock Data...")
