# JDE
Towards Real-Time Multi-Object Tracking

# 1. 训练步骤

以下操作步骤均以MOT16为例

## 1.1 准备数据集

* 从MOT挑战赛官网下载数据集并解压 <br>
wget https://motchallenge.net/data/MOT16.zip -P /data/tseng/dataset/jde <br>
cd /data/tseng/dataset/jde <br>
unzip MOT16.zip -d MOT16 <br>

* 创建MOT16任务的工作区, 并将MOT格式标注文件转换为需要格式的标注文件 <br>
git clone https://github.com/CnybTseng/JDE.git <br>
cd JDE <br>
python3 ./tools/getmotlabel.py --root-dir datasets/MOT16/train --save-dir datasets/MOT16/train <br>
mkdir -p workspace/mot16-20201011 <br>
sudo chmod +x ./tools/split_dataset.sh <br>
./tools/split_dataset.sh ./workspace/mot16-20201011 <br>
此时workspace/mot16-20201011目录下会生成train.txt <br>
python3 ./tools/calc_anchor.py --dataset workspace/mot16-20201011 --k 12 --max-iters 10000 --workspace workspace/mot16-20201011 <br>
cp workspace/mot16-20201011/log/anchors.txt workspace/mot16-20201011 <br>

## 1.2 从预训练模型导出参数生成JDE初始模型

* 从darknet官网下载darknet53预训练模型 <br>
wget https://pjreddie.com/media/files/darknet53.conv.74 -P ./workspace 或者直接使用自己以前下载好的<br>
python3 darknet2pytorch.py -pm ./workspace/mot16-20201011/jde.pth --dataset ./workspace/mot16-20201011 -dm /home/royzon/Documents/models/pre-trained/darknet/darknet53.conv.74 -lbo <br>
此时workspace/mot16-20201011目录下会生成初始模型jde.pth, 其骨干网已初始化为darnet53的参数 <br>

## 1.3 执行训练脚本

mv ./tools/train.sh ./train.sh <br>
sudo chmod +x ./train.sh <br>
根据需要修改, 然后运行训练脚本 <br>
./train.sh <br>

# 2. 测试
本项目实现了卡尔曼滤波的目标关联算法, 运行类似如下命令执行多目标跟踪 <br>
python3 tracker.py --img-path /home/royzon/Documents/datasets/opendatasets/MOT16/test/MOT16-03/img1 --model workspace/mot16-20201011/checkpoint/jde-ckpt-049.pth