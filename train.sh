#!/bin/bash

python3 train.py \
    --in-size 320 576 \
    --checkpoint workspace/mot16-20201011/jde.pth \
    --dataset workspace/mot16-20201011/ \
    --scale-step 224 512 10 480 768 \
    --rescale-freq 99999999 \
    --workers 8 \
    --epochs 50 \
    --lr 0.01 \
    --milestones 16625 24937 \
    --weight-decay 0.0001 \
    --savename jde \
    --pin \
    --workspace workspace/mot16-20201011/