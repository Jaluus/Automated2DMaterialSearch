# Databank Module

## First Use

### Database creation

1. Open the psql shell and login, or use a Database manager of your choice (like Datagrep).
2. Create a new Database with `CREATE DATABASE mydatabase`.
3. Create 3 new Tables, I may implement this in python directly.
4. Dont forget the rights managment

image_table

    create table image
    (
        id   bigserial not null
            constraint image_pk
                primary key,
        path text      not null
    );

flake_table

    create table flake
    ( 
        id        bigserial        not null
            constraint flake_pk
                primary key,
        size      double precision not null,
        thickness integer          not null
    );

relation_table

    create table relation_table
    (
        image_id bigserial not null
            constraint relation_table_image_id_fk
                references image,
        flake_id bigserial not null
            constraint relation_table_flake_id_fk
                references flake
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