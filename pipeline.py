import os
from tqdm import tqdm
import numpy as np
import cv2
from PIL import Image

# create INT8_Calib_Unlable_Dataset
root = "/media/rjg/T7 Shield/Orin_T906G/dataset_SunAndShadow/images/val"
dirs = os.listdir(root)
for image in tqdm(dirs):
    path = os.path.join(root, image)
    with open('calib_dataset.txt','a') as f:
        f.write(f'{path}\n')


# create valid_label.text
# /home/rjg/dataset2/images/val/IMG_20221013_125157_aug3.jpg

root1 = "/media/rjg/T7 Shield/Orin_T906G/dataset_SunAndShadow/images/val"
root2 = "/media/rjg/T7 Shield/Orin_T906G/dataset_SunAndShadow/masks"
dirs = os.listdir(root1)
for img_name in tqdm(dirs):
    if img_name.split('.')[0][-4:-1] == 'aug':
        mask_name = img_name.split('.')[0][:-5] + '.png'
    else:
        mask_name = img_name.split('.')[0] + '.png'
    with open('valid_label.txt','a') as f:
        f.write(f'{os.path.join(root2, mask_name)}\n')

# src = cv2.imread("/home/rjg/dataset2/masks/IMG_20221006_123003.png")
# src1 = Image.open("/home/rjg/dataset2/masks/IMG_20221006_123003.png")
# arr1 = np.array(src1)
# print(np.unique(arr1))
# src1.show()

# arr = np.array(src)
# print(np.unique(arr))
# cv2.imshow("frame", src)
# cv2.waitKey(0)