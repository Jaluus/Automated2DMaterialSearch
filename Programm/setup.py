from setuptools import setup

setup(
    name="AutomaticFlakeFinder",
    version="0.1.0",
    description="A Small Library to facilitate the Automation of finding Flakes",
    author="Jan-Lucas Uslu",
    author_email="jan-lucas.uslu@rwth-aachen.de",
    install_requires=[
        "numpy",
        "opencv-python",
        "scikit-image",
        "matplotlib",
        "pywin32",
        "mysql-connector-python",
    ],
)
