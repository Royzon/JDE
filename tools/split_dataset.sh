#!/bin/sh

echo "dataset directory: $1"
python3 tools/split_dataset.py --path \
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-02/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-04/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-05/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-09/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-10/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-11/img1,\
/home/royzon/Documents/datasets/opendatasets/MOT16/train/MOT16-13/img1 \
-tr 1 \
--save-path $1