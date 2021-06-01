# Databank Module

## The Config File

You need to create a database.ini config file which looks like this

    [postgresql]
    host=localhost
    database=testing
    user=postgres
    password=postgres
    port=5432


## What is being saved
Table Image
- ID
- Image Path
- Detection Module
  - Number Monolayer
  - Number Fewlayer
  - Number Thicklayer
- Microscope Module
  - Lights
    - Aparture
    - Voltage
    - Light Type?
  - Nosepiece
    - Current Magnification
- Tango Module
  - Position XY
- Detection Module
  - Layers in the pictures
- Camera Module
  - Date taken
  - Resolution
  - Bit Depth
  - Size
  - RBG-composition
  - Gain
  - Exposure time

Table Flake
- ID
- Image_ID
- Thickness
- Size
- Position on the Picture