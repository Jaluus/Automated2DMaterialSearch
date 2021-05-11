import numpy as np

import cv2

from skimage.filters.rank import entropy
from skimage.morphology import disk

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (
    QMainWindow,
    QWidget,
    QPushButton,
    QHBoxLayout,
    QApplication,
    QSlider,
    QVBoxLayout,
    QLabel,
    QGridLayout,
    QFileDialog,
    QSpacerItem,
    QSizePolicy,
    QCheckBox,
    QSpinBox,
)
from PyQt5.QtGui import QIcon, QPixmap, QImage


class StartWindow(QMainWindow):
    def __init__(self, image_loader, edge_detector):
        super().__init__()

        self.w = 1280
        self.h = 720

        self.setFixedWidth(self.w)
        self.setFixedHeight(self.h)
        self.setWindowTitle("Testing Overlay")

        self.image_loader = image_loader
        self.edge_detector = edge_detector

        self.img = None

        self.initUI()

    def initUI(self):
        self.central_widget = QWidget()
        self.buttonPictureSelector = QPushButton(
            "Select Picture", self.central_widget)
        self.buttonPictureSelector.clicked.connect(self.changePicture)

        self.lowerTLabel = QLabel("Lower Threshold")
        self.upperTLabel = QLabel("Upper Threshold")
        self.alphaLabel = QLabel("Alpha")

        self.vSpacer1 = QSpacerItem(
            20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)
        self.vSpacer2 = QSpacerItem(
            20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.SBoxUpperT = QSpinBox()
        self.SBoxUpperT.setValue(self.edge_detector.getUpperThresh())
        self.SBoxUpperT.valueChanged.connect(self.changeUpperThresh)

        self.SBoxLowerT = QSpinBox()
        self.SBoxLowerT.setValue(self.edge_detector.getLowerThresh())
        self.SBoxLowerT.valueChanged.connect(self.changeLowerThresh)

        self.SBoxAlpha = QSpinBox()
        self.SBoxAlpha.setValue(self.edge_detector.getAlpha())
        self.SBoxAlpha.valueChanged.connect(self.changeAlpha)

        self.lableIterations = QLabel("Iterations")
        self.SBoxIterations = QSpinBox()
        self.SBoxIterations.setValue(self.edge_detector.getIterations())
        self.SBoxIterations.valueChanged.connect(self.changeIterations)

        self.labelFilterSize = QLabel("FilterSize")
        self.SBoxFilterSize = QSpinBox()
        self.SBoxFilterSize.setValue(self.edge_detector.getFilterSize())
        self.SBoxFilterSize.valueChanged.connect(self.changeFilterSize)

        self.morphCheckbox = QCheckBox()
        self.morphCheckbox.setChecked(True)
        self.morphCheckbox.setText("Fill Edges")
        self.morphCheckbox.stateChanged.connect(self.updateImage)

        self.entropyCheckbox = QCheckBox()
        self.entropyCheckbox.setChecked(False)
        self.entropyCheckbox.setText("Show Entropy")
        self.entropyCheckbox.stateChanged.connect(self.updateImage)

        self.overlayCheckbox = QCheckBox()
        self.overlayCheckbox.setChecked(True)
        self.overlayCheckbox.setText("Show Overlay")
        self.overlayCheckbox.stateChanged.connect(self.updateImage)

        self.image_label = QLabel(self)
        self.pixmap = QPixmap()
        self.image_label.setPixmap(self.pixmap)

        # ADD ELEMENTS TO LAYOUT

        self.gridLayout = QGridLayout(self.central_widget)
        self.gridLayout.setObjectName("gridLayout")

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")

        self.verticalLayout.addItem(self.vSpacer1)
        self.verticalLayout.addWidget(self.buttonPictureSelector)
        self.verticalLayout.addWidget(self.upperTLabel)
        self.verticalLayout.addWidget(self.SBoxUpperT)
        self.verticalLayout.addWidget(self.lowerTLabel)
        self.verticalLayout.addWidget(self.SBoxLowerT)
        self.verticalLayout.addWidget(self.alphaLabel)
        self.verticalLayout.addWidget(self.SBoxAlpha)
        self.verticalLayout.addWidget(self.overlayCheckbox)
        self.verticalLayout.addWidget(self.morphCheckbox)
        self.verticalLayout.addWidget(self.lableIterations)
        self.verticalLayout.addWidget(self.SBoxIterations)
        self.verticalLayout.addWidget(self.labelFilterSize)
        self.verticalLayout.addWidget(self.SBoxFilterSize)
        self.verticalLayout.addWidget(self.entropyCheckbox)
        self.verticalLayout.addItem(self.vSpacer2)

        self.horizontalLayout.addLayout(self.verticalLayout)
        self.horizontalLayout.addWidget(self.image_label)

        self.gridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)

        self.setCentralWidget(self.central_widget)

        self.loadImage()
        self.updateImage()

    def loadImage(self):
        self.img = self.image_loader.getImage()
        self.displayImage(self.img)

    def displayImage(self, img):
        height, width, channel = img.shape
        bytesPerLine = 3 * width
        qImg = QImage(img.data, width, height,
                      bytesPerLine, QImage.Format_RGB888)
        self.image_label.setPixmap(QPixmap(qImg).scaledToWidth(1000))

    def updateImage(self):

        if not self.overlayCheckbox.isChecked():
            self.displayImage(self.img)
            return

        edge_mask = self.edge_detector.detectEdges(self.img.copy())

        if self.morphCheckbox.isChecked():
            edge_mask, regions = self.edge_detector.fillEdges(edge_mask)

            if self.entropyCheckbox.isChecked():
                edge_mask = self.edge_detector.entropyImage(
                    self.img.copy(),
                    regions,
                    4,
                )

        overlay = self.edge_detector.overlayEdges(self.img.copy(), edge_mask)
        self.displayImage(overlay)

    def changeLowerThresh(self, value):
        self.edge_detector.setLowerThresh(value)
        self.updateImage()

    def changeUpperThresh(self, value):
        self.edge_detector.setUpperThresh(value)
        self.updateImage()

    def changeAlpha(self, value):
        self.edge_detector.setAlpha(value)
        self.updateImage()

    def changeIterations(self, value):
        self.edge_detector.setIterations(value)
        self.updateImage()

    def changeFilterSize(self, value):
        self.edge_detector.setFilterSize(value)
        self.updateImage()

    def changePicture(self):
        fPath = QFileDialog.getOpenFileName(self, "Open File")
        self.image_loader.loadImage(fPath[0])
        self.loadImage()
        self.updateImage()
