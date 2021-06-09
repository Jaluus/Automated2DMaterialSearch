# Databank Module

### The Config File

You need to create a database.ini config file which looks like this for connecting to it via Python

    [postgresql]
    host=localhost
    database=mydatabase
    user=postgres
    password=postgres
    port=5432

    [mysql]
    host=localhost
    database=testing_flakes
    user=root
    port=3306

## Things you can search for

- Get all flakes with the given SCAN_ID
  - Get all flakes with size bigger than...
  - Get all flakes with size Thickness
- Get all flakes with the given CHIP_ID
  - Get all flakes with size bigger than...
  - Get all flakes with size Thickness
- Get all flakes which are younger than...

## What is being saved

Table Image

- ID
- Image Path
- Microscope Module
  - Lights
    - Aparture
    - Voltage
  - Nosepiece
    - Current Magnification
- Tango Module
  - Position XY
- Camera Module
  - Date taken
  - Resolution
  - Bit Depth
  - Size
  - White_balance_r
  - White_balance_g
  - White_balance_b
  - Gain
  - Exposure time
- Detection Module
  - Number Monolayer
  - Number Fewlayer
  - Number Thicklayer

Table Flake

- ID : bigserial
- Image_ID : bigserial
- Thickness : "mono" , "few" , "thick"
- Size : double
- Used : bool
- Position on the Picture ?