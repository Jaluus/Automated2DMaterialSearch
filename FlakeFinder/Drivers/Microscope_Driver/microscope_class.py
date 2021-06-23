import win32com.client


class microscope_driver_class:
    """
    an easy way to control the microscope
    """

    def __init__(self):
        # Creates an microscope object, only possible if NIS is closed and no other application is using the LV
        self.micro = win32com.client.Dispatch("Nikon.LvMic.nikonLV")
        # this line is shit, took 4 hours

        self.set_default_values()

    def set_default_values(self):
        self.lamp_on()
        self.set_lamp_voltage(6.4)
        self.set_lamp_aperture_stop(2.3)

    def get_microscope_object(self):
        return self.micro

    def set_z_height(self, height):
        """
        Sets the Height in µm\n
        Only works if the AF is not on\n
        Has small protection by only setting height between 3500 and 6500 µm
        """
        try:
            if 3500 < height < 6500:
                rescaled_pulses = height * 20
                self.micro.ZDrive.MoveAbsolute(rescaled_pulses)
        except:
            print("Already in Focus!")

    def get_z_height(self):
        height = self.micro.ZDrive.Value()
        return height

    def lamp_on(self):
        self.micro.EpiLamp.On()

    def lamp_off(self):
        self.micro.EpiLamp.Off()

    def rotate_nosepiece_forward(self):
        self.micro.Nosepiece.Forward()

    def rotate_nosepiece_backward(self):
        self.micro.Nosepiece.Reverse()

    def set_lamp_voltage(self, voltage: float):
        self.micro.EpiLamp.Voltage = voltage

    def set_mag(self, mag_idx: int):
        """
        Swaps the Position of the Nosepiece\n
        1 : 2.5x\n
        2 : 5x\n
        3 : 20x\n
        4 : 50x\n
        5 : 100x\n
        """
        height = 5500
        if mag_idx == 1:
            height = 5500
        elif mag_idx == 2:
            height = 4300
        elif mag_idx == 3:
            height = 3930
        elif mag_idx == 4:
            height = 3900
        elif mag_idx == 5:
            height = 3900
        else:
            print("Wrong Idx, need values between 1 and 5")
            return
        self.micro.Nosepiece.Position = mag_idx
        self.set_z_height(height)

    def set_lamp_aperture_stop(self, aperture_stop: float):
        self.micro.EpiApertureStop.ApertureStop = aperture_stop

    def get_af_status(self):
        """
        Return Codes:\n
        AfStatusUnknown     : -1\n
        AfStatusJustFocus   : 1\n
        AfStatusUnderFocus  : 2\n
        AfStatusOverFocus   : 3\n
        AfStatusOutOfRange  : 9
        """
        return self.micro.ZDrive.AfStatus()

    def is_af_searching(self):
        return self.micro.ZDrive.AfSearchMode()

    def find_af(self, mode: int = 2):
        """
        There are 2 modes, 1 and 2, i dont know the difference
        """
        self.micro.ZDrive.SearchAF(0)

    def get_properties(self):
        """
        Returns the current properties of the microscope\n
        dict keys:
        'nosepiece' : positon of the nosepiece
        'aparture'  : current ApertureStop of the EpiLamp
        'voltage'   : current Voltage of the EpiLamp in Volts
        """
        val_dict = {}
        val_dict["z_height"] = self.get_z_height()
        val_dict["nosepiece"] = self.micro.Nosepiece.Position()
        val_dict["aperture"] = self.micro.EpiApertureStop.ApertureStop()
        val_dict["light"] = self.micro.EpiLamp.Voltage()
        return val_dict


if __name__ == "__main__":
    micro = microscope_driver_class()
    micro.set_mag(3)
    props = micro.get_properties()
    print(props)
