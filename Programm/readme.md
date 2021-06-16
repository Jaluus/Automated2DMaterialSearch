# Modules
## Motor Module (Finished)
Running with the Tango DLL library

## Camera Module (Finished)
Grabbing pictures from the Camera

## Microscope Module (Finished)
Manipulating the Microscope via the Computer

## Detection Module (Unused)
Rule based approch to find 2D flakes in Images

## Databank Module (Needs Tweaking)
Storing the Image metadata in a MySQL Databank

# Installation
## Python with Anaconda
1. Create a new Environment with Python 3.7 by running `conda create --name [myEnvName] python=3.7`
2. Activate the Environment with `activate [myEnvName]`, you might need to add Anaconda to path to do this in the CMD you can also do this in the Anaconda promt
3. Install all the Modules with `pip install -r requirements.txt` while in your env

Without Anaconda you can use venv or virtualenv, Step 3 is still the same

## Databank Module
Install MySQL by downloading it from [the official website](https://www.mysql.com/de/)

Current Version: 8.0.25 

## Microscope Module
Install the NikonLV SDK, which should be automatically installed when you install NIS-Elements

# Possible errors:
- Problem: `pywin32` is not working on your machine while controlling the microscope
  - Fix: Download `pypiwin32` with `pip install pypiwin32`

# ToDo:
1. Setup Apache Webserver to serve Images locally
2. Write a Python-Flask CRUD API for the DB
3. Write a Frontend to talk to the API