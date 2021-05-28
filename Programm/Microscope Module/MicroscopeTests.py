import win32com.client

# Create a nikonLV object
nikonLV = win32com.client.Dispatch("Nikon.LvMic.nikonLV")

nikonLV.EpiLamp.On()

print(nikonLV.EpiLamp.Value())


def rotate_nosepiece(nikonLV):
    # Get the Current Position
    pos = nikonLV.Nosepiece.Position()
    print(pos)

    # Rotate
    r = nikonLV.Nosepiece.Forward()
    print(r)

    # get the new Position
    pos = nikonLV.Nosepiece.Position()
    print(pos)
