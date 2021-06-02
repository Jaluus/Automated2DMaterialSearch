# Databank Module

## First Use

### Database creation

1. Open the psql shell and login, or use a Database manager of your choice (like Datagrep).
2. Create a new Database with `CREATE DATABASE mydatabase`.
3. Create 2 new Tables, I may implement this in python directly.

Image_table

    create table image
    (
      id bigserial not null
        constraint image_pk
          primary key,
      path varchar(500) not null,
      light_aparture double precision not null,
      light_voltage double precision not null,
      nosepiece_magnification double precision not null,
      position_x double precision not null,
      position_y double precision not null,
      taken timestamp not null,
      size_mb double precision,
      resolution_y int default 1200,
      resolution_x int default 1920,
      bit_depth int default 24,
      r_value int,
      g_value int,
      b_value int,
      gain double precision,
      exposure_time_ms double precision
    );

flake table

    create table flake
    (
      id bigserial not null
        constraint flake_pk
          primary key,
      image_id bigserial not null
        constraint flake_image_id_fk
          references image,
      thickness int not null,
      size double precision not null
    );


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