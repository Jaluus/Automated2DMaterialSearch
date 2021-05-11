import cv2
import numpy as np

# MultiThreading Madness
import concurrent.futures

from skimage.feature import canny
from skimage.filters.rank import entropy
from skimage.morphology import disk


class edgeDetector:
    def __init__(self):
        self.upperThresh = 4
        self.lowerThresh = 0
        self.alpha = 2

        self.iterations = 2
        self.filter_size = 2

    def detectEdges(self, img):
        """Detects Edges in the Image and returns and edgeMask"""

        image_gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)

        edge_mask = canny(
            image_gray,
            sigma=self.alpha,
            low_threshold=self.lowerThresh,
            high_threshold=self.upperThresh,
        )

        # Convert to gray array with vals between 0 and 255
        img = np.zeros_like(image_gray)
        img[edge_mask == 1] = 255
        edge_mask = img

        # Dilate the edges to get better Contours
        edge_mask = cv2.morphologyEx(
            src=edge_mask,
            op=cv2.MORPH_DILATE,
            kernel=disk(self.filter_size),
            iterations=self.iterations,
        )

        return edge_mask

    def fillEdges(self, edge_mask):

        kernel = disk(self.filter_size)
        # kernel = np.ones((self.filter_size, self.filter_size))

        # Find the Contours
        contours, hierarchy = cv2.findContours(
            edge_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE
        )

        contour_mask = np.zeros_like(edge_mask)
        contour_mask_all = []
        # Draw The Countours
        for i in range(len(contours)):
            # Only draw Outer Contours
            if hierarchy[0][i][3] != -1:
                continue
            blank_img = np.zeros_like(edge_mask)
            filled_contour = cv2.drawContours(blank_img, contours, i, 255, thickness=-1)

            # get rid of the Dilation we did to expand the Contours -> Makes the area bigger than it is, als gets rid of single lines
            eroded_contour = cv2.morphologyEx(
                src=filled_contour,
                op=cv2.MORPH_ERODE,
                kernel=kernel,
                iterations=self.iterations + 3,
            )
            # Puff it up to original size
            eroded_contour = cv2.morphologyEx(
                src=eroded_contour,
                op=cv2.MORPH_DILATE,
                kernel=kernel,
                iterations=3,
            )

            # Dont append empty contours
            if np.max(eroded_contour) == 0:
                continue

            contour_mask_all.append(eroded_contour)
            contour_mask[eroded_contour == 255] = 255

        contour_mask_all = np.array(contour_mask_all)

        return contour_mask, contour_mask_all

    def overlayEdges(self, img, edge_mask):
        """Overlays RGB Image With The Mask"""
        overlay = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        overlay = cv2.cvtColor(overlay, cv2.COLOR_GRAY2RGB)
        img[edge_mask == 255] = [255, 0, 0]
        return img

    def entropyImage(self, img, region_mask, threshold=np.Infinity):
        image_gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        blank_mask = np.zeros_like(region_mask[0])
        num_regions = len(region_mask)
        for idx, region in enumerate(region_mask):
            region_entropy = entropy(image_gray, np.ones((9, 9)), mask=region)
            print(idx / num_regions)
            if np.max(region_entropy) > threshold:
                continue
            blank_mask[region == 255] = 255

        return blank_mask

    def setLowerThresh(self, lt):
        self.lowerThresh = lt

    def setUpperThresh(self, ht):
        self.upperThresh = ht

    def setAlpha(self, val):
        self.alpha = val

    def setIterations(self, val):
        self.iterations = val

    def setFilterSize(self, val):
        self.filter_size = val

    def getFilterSize(self):
        return self.filter_size

    def getIterations(self):
        return self.iterations

    def getAlpha(self):
        return self.alpha

    def getUpperThresh(self):
        return self.upperThresh

    def getLowerThresh(self):
        return self.lowerThresh


if __name__ == "__main__":
    cannyTest = edgeDetector()
    cannyTest.circularKernel(4)