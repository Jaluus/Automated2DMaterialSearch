# Drivers
## Motor Driver (Finished)
Running with the Tango DLL library

## Camera Driver (Finished)
Grabbing pictures from the Camera

## Microscope Driver (Finished)
Manipulating the Microscope via the Computer

## Databank Driver (In the Works)
Storing the Image metadata in a MySQL Databank

# Installation
## Python with Anaconda
1. Create a new Environment with Python 3.7 by running `conda create --name [myEnvName] python=3.7`
2. Activate the Environment with `activate [myEnvName]`, you might need to add Anaconda to path to do this in the CMD you can also do this in the Anaconda promt
3. Install all the Modules with `pip install -r requirements.txt` while in your env

Without Anaconda you can use venv or virtualenv, Step 3 is still the same

## Databank Driver
Install MySQL by downloading it from [the official website](https://www.mysql.com/de/)

Current Version: 8.0.25 

## Databank Interface on Windows
1. Download Apache Lounge form [here]{https://www.apachelounge.com/download/}. I used version `httpd-2.4.48-win64-VS16.zip`
2. Unzip the file
3. Move the `Apache24` folder to `C:\Apache24`
4. Open the CMD __*with*__ admin rights
5. Run `cd C:\Apache24\bin httpd -k install` to install the service and allow all communication
6. Open Services on windows and search for Apache24 and start the service
7. Yeay, you now have a running webserver!
8. (Optional) Run `ipconfig` in the CMD to find your Local IP address and access the server


## Microscope Module
Install the NikonLV SDK, which should be automatically installed when you install NIS-Elements

# Possible errors:
- Problem: `pywin32` is not working on your machine while controlling the microscope
  - Fix: Download `pypiwin32` with `pip install pypiwin32`

# ToDo:
1. Setup Apache Webserver to serve Images locally
2. Write a Python-Flask CRUD API for the DB
3. Write a Frontend to talk to the API