# Automated 2D Material Searching Algorithm

A contrast-based approach to detect 2D Materials

## Installation

### Python with Anaconda

1. Create a new Environment with Python 3.7 by running `conda create --name [myEnvName] python=3.7`
2. Activate the Environment with `activate [myEnvName]`, you might need to add Anaconda to path to do this in the CMD you can also do this in the Anaconda promt
3. Install all the Modules with `pip install -r requirements.txt` while in your env

Without Anaconda you can use venv or virtualenv, Step 3 is still the same

<!-- ### Databank Driver

Install MySQL by downloading it from [the official website](https://www.mysql.com/de/)

__Used Version__ : 8.0.25

### Databank Interface on Windows

1. Download Apache Lounge form [here](<https://www.apachelounge.com/download/>). I used version `httpd-2.4.48-win64-VS16.zip`
2. Unzip the file
3. Move the `Apache24` folder to `C:\Apache24`
4. Open the CMD __*with*__ admin rights
5. Run `cd C:\Apache24\bin httpd -k install` to install the service and allow all communication
6. Open Services on windows and search for Apache24 and start the service
7. Yeay, you now have a running webserver!
8. (Optional) Run `ipconfig` in the CMD to find your Local IP address and access the server -->

### Microscope Driver

Install the NikonLV SDK, which should be automatically installed when you install NIS-Elements

## TODO

- Drivers
  - Read camera parameters
  - Interface with the autofocus
- Program
  - Improve centering of flakes
- GUI
  - Add the ability to choose the magnification the to UI
  - Overhaul the UI?
- General
  - Add the front- and backend to the repo
  - General Errorlogging into a txt
  - Write more documentation
  - Add a wiki

## Possible errors

- Problem: `pywin32` is not working on your machine while controlling the microscope
  - Fix: Download `pypiwin32` with `pip install pypiwin32`
