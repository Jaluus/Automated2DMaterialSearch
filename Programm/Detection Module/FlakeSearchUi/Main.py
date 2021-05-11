from PyQt5.QtWidgets import QApplication

from ImageLoaderModell import imageLoader
from EdgeDetector import edgeDetector
from GUI import StartWindow

if __name__ == "__main__":
    app = QApplication([])

    image_loader = imageLoader()
    image_loader.loadImage("test_img.jpg")
    edge_detector = edgeDetector()

    window = StartWindow(image_loader, edge_detector)
    window.show()
    app.exit(app.exec_())
