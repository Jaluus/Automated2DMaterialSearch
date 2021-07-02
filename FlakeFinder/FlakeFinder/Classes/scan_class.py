from Drivers.Microscope_Driver.microscope_class import microscope_driver_class
from Drivers.Motor_Driver.tango_class import motor_driver_class
from Drivers.Camera_Driver.camera_class import camera_driver_class


class scanner_class:
    """
    # The Scanner Class

    Usage:
    - Abstraction to create a mask from 2.5x Zoom images

    Functions:
    - create_overview
    -

    """

    def __init__(
        self,
        motor_driver: motor_driver_class,
        microscope_driver: microscope_driver_class,
        camera_driver: camera_driver_class,
    ):
        self.motor = motor_driver
        self.microscope = microscope_driver
        self.camera = camera_driver

    def create_overview(self):
        """
        Creates an Overview Image
        """
        pass

    def fine_raster(self):
        pass
