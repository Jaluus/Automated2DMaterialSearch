import win32com.client

# Create a nikonLV object
nikonLV = win32com.client.Dispatch("Nikon.LvMic.nikonLV")

nikonLV.EpiLamp.On()


def rotate_nosepiece(nikonLV):
    # Get the Current Position
    pos = nikonLV.Nosepiece.Position()
    print(pos)

    # Rotate
    nikonLV.Nosepiece.Reverse()

    # get the new Position
    pos = nikonLV.Nosepiece.Position()
    print(pos)


def get_current_properties(nikonLV):
    val_dict = {}
    val_dict["nosepiece"] = nikonLV.Nosepiece.Position()
    val_dict["aperture"] = nikonLV.EpiApertureStop.ApertureStop()
    val_dict["light"] = nikonLV.EpiLamp.Voltage()
    return val_dict


# {'nosepiece': 3, 'aperture': 2.3, 'light': 6.4}
print(nikonLV.Nosepiece.Value())

print(get_current_properties(nikonLV))
