import cv2


class imageLoader:
    def __init__(self):
        self.image = None
        self.filePath = None

    def loadImage(self, fPath):
        self.filePath = fPath
        self.image = cv2.cvtColor(cv2.imread(self.filePath), cv2.COLOR_BGR2RGB)

    def getImage(self):
        return self.image

    def setFPath(self, fPath):
        self.filePath = fPath


if __name__ == "__main__":
    pass
