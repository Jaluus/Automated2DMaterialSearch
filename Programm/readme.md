# Modules
## Motor Module
Running with the Tango DLL library

## Camera Module
Grabbing pictures from the Camera

## Microscope Module
Manipulating the Microscope via the Computer

## Detection Module
Rule based approch to find 2D flakes in images

## Databank Module
Storing the Image metadata in a PostGreSQL Databank

# Installation
## Python with Anaconda
1. Create a new Environment with Python 3.7 by running `conda create --name [myEnvName] python=3.7`
2. Activate the Environment with `activate [myEnvName]`, you might need to add Anaconda to path to do this in the CMD you can also do this in the Anaconda promt
3. Install all the Modules with `pip install -r requirements.txt` while in your env

Without Anaconda you can use venv or virtualenv, Step 3 is still the same

## Databank Module
Install PostgreSQL by downloading it from [the official website](https://www.postgresql.org/)

## Microscope Module
Install the NikonLV SDK, which should be automatically installed when you install NIS-Elements

# Possible errors:
- Problem: `pywin32` is not working on your machine while controlling the microscope
  - Fix: Download `pypiwin32` with `pip install pypiwin32`