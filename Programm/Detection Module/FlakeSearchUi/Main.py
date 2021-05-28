import sys
import os

module_path = os.path.join(os.path.dirname(__file__), "..", "Classes")

sys.path.append(module_path)

from GUI import StartWindow
from EdgeDetectorModell import edgeDetector
from ImageLoaderModell import imageLoader
from PyQt5.QtWidgets import QApplication


# Getting the path to the Testimage
test_img_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "testImg.jpg")

if __name__ == "__main__":
    app = QApplication([])

    image_loader = imageLoader()
    image_loader.loadImage(test_img_path)
    edge_detector = edgeDetector()

    window = StartWindow(image_loader, edge_detector)
    window.show()
    app.exit(app.exec_())
