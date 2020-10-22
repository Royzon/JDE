# -*- coding:utf-8 -*-
import os
import cv2

'''
显示跟踪训练数据集标注
'''
root_path = "/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-04"
img_dir = "img1"
label_dir = "labels_with_ids"

imgs = sorted(os.listdir(root_path + "/" + img_dir))
for i, img in enumerate(imgs):
    img_name = img[:-4]
    label_f = open(root_path + "/" + label_dir + "/" + img_name + ".txt", "r")
    lines = label_f.readlines()
    img_data = cv2.imread(root_path + "/" + img_dir + "/" + img)
    H, W, C = img_data.shape
    for line in lines:
        line_list = line.strip().split()
        class_num = int(line_list[0])  # 类别号
        obj_ID = int(line_list[1])  # 目标ID
        x, y, w, h = line_list[2:]  # 中心坐标，宽高（经过原图宽高归一化后）
        x = int(float(x) * W)
        y = int(float(y) * H)
        w = int(float(w) * W)
        h = int(float(h) * H)
        left = int(x - w / 2)
        top = int(y - h / 2)
        right = left + w
        bottom = top + h
        cv2.circle(img_data, (x, y), 3, (255, 0, 255), 3)
        cv2.rectangle(img_data, (left, top), (right, bottom), (0, 255, 0), 2)
        cv2.putText(img_data, str(obj_ID), (left, top), cv2.FONT_HERSHEY_COMPLEX, 1.5, (0, 0, 255), 2)
    resized_img = cv2.resize(img_data, (800, 416))
    cv2.imshow("label", resized_img)
    cv2.waitKey(1)

