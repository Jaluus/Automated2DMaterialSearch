# Databank Module

## First Use

### Database creation

1. Open the psql shell and login, or use a Database manager of your choice (like Datagrep).
2. Create a new Database with `CREATE DATABASE mydatabase`.
3. Create 2 new Tables, I may implement this in python directly.

### The Config File

You need to create a database.ini config file which looks like this for connecting to it via Python

    [postgresql]
    host=localhost
    database=mydatabase
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