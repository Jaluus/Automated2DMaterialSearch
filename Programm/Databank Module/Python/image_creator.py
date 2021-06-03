import cv2
import numpy as np
import matplotlib.pyplot as plt
import os

file_path = os.path.dirname(__file__)

curr_id = 0

for i in range(1000):

    img = np.zeros((1000, 1000, 3))

    for j in range(np.random.randint(5, 30)):
        xy1 = tuple(np.random.randint(1000, size=2))
        xy2 = tuple(np.random.randint(1000, size=2))
        color = tuple(np.random.rand(3) * 255)
        cv2.rectangle(img, xy1, xy2, color, thickness=-1)

    cv2.imwrite(os.path.join(file_path, f"Pictures/{curr_id}.jpg"), img)
    curr_id += 1
    print(f"\r current img: {i}\r", end="", flush=True)
