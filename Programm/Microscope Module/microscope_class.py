import win32com.client


class microscope_control:
    """
    an easy way to control the microscope
    """

    def __init__(self):
        # Creates an microscope object, onbly possible if NIS is closed and no other application is using the LV
        self.micro = win32com.client.Dispatch("Nikon.LvMic.nikonLV")

    def get_microscope_object(self):
        return self.micro

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

    def set_lamp_aperture_stop(self, aperture_stop: float):
        self.micro.EpiApertureStop.ApertureStop = aperture_stop

    def get_current_properties(self):
        """
        Returns the current properties of the microscope\n
        dict keys:
        'nosepiece' : positon of the nosepiece
        'aparture'  : current ApertureStop of the EpiLamp
        'voltage'   : current Voltage of the EpiLamp in Volts
        """
        val_dict = {}
        val_dict["nosepiece"] = self.micro.Nosepiece.Position()
        val_dict["aperture"] = self.micro.EpiApertureStop.ApertureStop()
        val_dict["light"] = self.micro.EpiLamp.Voltage()
        return val_dict


if __name__ == "__main__":
    micro = microscope_control()
    props = micro.get_current_properties()
    print(props)
