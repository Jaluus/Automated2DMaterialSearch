import sys

sys.path.append(
    r"C:\Users\duden\Desktop\UniRepos\BachelorThesis\Programm\Detection Module\Classes"
)
import os
from EdgeDetectorModell import edgeDetector
from ImageLoaderModell import imageLoader
import json


class detector:
    def __init__(self, config_path):
        # Default Params
        self.param_dict = {
            "alpha": 4,
            "upper_thresh": 4,
            "lower_thresh": 0,
            "entropy_threshold": 0,
            "morph_iterations": 1,
            "morph_kernel_size": 2,
        }

        self._load_config(config_path)

        print("Detector loaded with the following parameters")
        for key in self.param_dict.keys():
            print(f"{key} : {self.param_dict[key]}")

    def _load_config(self, config_path):
        # The config has to lie in a child or directly in the dict of the detection
        path = os.path.join(os.path.dirname(__file__), config_path)
        try:
            file = open(path)
            print("File found, loading parameters...")
            self.param_dict = json.load(file)

        except FileNotFoundError:
            print(f"Path '{path}' not found, keeping default parameters")

    def get_config(self):
        return self.param_dict

    def detect_image(self):

        regions = []

        if not self.overlayCheckbox.isChecked():
            self.displayImage(self.img)
            return

        edge_mask = self.edge_detector.detectEdges(self.img.copy())

        if self.fillRegionCheckbox.isChecked():
            edge_mask, regions = self.edge_detector.fillEdges(edge_mask)

            if self.entropyCheckbox.isChecked():
                edge_mask = self.edge_detector.entropyImage(
                    self.img.copy(),
                    regions,
                )
            if self.outlineCheckbox.isChecked():
                edge_mask, regions = self.edge_detector.finalOutline(edge_mask)

        if len(regions) == 0:
            overlay = self.edge_detector.overlayEdges(self.img.copy(), edge_mask)
        else:
            overlay = self.img.copy()
            for region in regions:
                overlay = self.edge_detector.overlayEdges(overlay, region)
        self.displayImage(overlay)
        self.detectedImage = overlay.copy()


test = detector("test_config.json")
