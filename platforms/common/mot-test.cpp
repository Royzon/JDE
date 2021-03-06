#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <opencv2/opencv.hpp>

#include "mot.h"
#include "SH_ImageAlgLogSystem.h"

static void log_fprintf(E_CommonLogLevel level, const char * p_log_info, const char * p_file, int line)
{
    fprintf(stderr, "level:[%d], file:[%s], line:[%d], info:[%s]\n", level, p_file, line, p_log_info);
}

int main(int argc, char *argv[])
{
    // 1. 注册日志回调函数, 详情请参考苏永生的日志模块接口文件SH_ImageAlgLogSystem.h
    ImgAlgRegisterLogSystemCallBack((PFUN_LogSystemCallBack)log_fprintf);
    if (argc < 3)
    {
        fprintf(stderr, "Usage:\n%s cfg_path images_path\n", argv[0]);
        return -1;
    }
    
    // 判断用户提供的图像文件目录是否合法
    struct stat statbuf;
    if (0 != stat(argv[2], &statbuf))
    {
        fprintf(stderr, "stat error: %d\n", errno);
        return -1;
    }
    
    if (!S_ISDIR(statbuf.st_mode))
    {
        fprintf(stderr, "%s is not a directory!\n", argv[2]);
        return -1;
    }
    
    // 创建存放结果的目录
    if (0 != access("./result", F_OK))
    {
        printf("create directory: result\n");
        if (-1 == system("mkdir ./result"))
        {
            fprintf(stderr, "mkdir fail\n");
            return -1;
        }
    }
    
    // 2. 加载MOT模型
    int ret = mot::load_mot_model(argv[1]);
    if (ret)
        return -1;
    
    // OpenCV绘图相关参数
    mot::MOT_Result result;
    int fontface = cv::FONT_HERSHEY_COMPLEX_SMALL;
    double fontscale = 1;
    int thickness = 1;
    
    // 读取目录中的文件
    dirent **dir = NULL;
    int num = scandir(argv[2], &dir, 0, alphasort);
    float duration = 0;
    for (int i = 0; i < num; ++i)
    {
        // 只处理jpg后缀文件
        if (DT_REG != dir[i]->d_type || !strstr(dir[i]->d_name, "jpg"))
            continue;

        char filein[128] = {0};
        strcat(filein, argv[2]);
        strcat(filein, dir[i]->d_name);
        
        // 读取图像文件和解码成BGR888
        cv::Mat bgr = cv::imread(filein);
        if (bgr.empty())
        {
            fprintf(stdout, "cv::imread(%s) fail!\n", filein);
            continue;
        }
        
        cv::Mat rgb;
        cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);  // BGR888转RGB888
        struct timeval t1, t2;
        gettimeofday(&t1, NULL);
        // 3. 执行推理, 检测和跟踪目标
        ret = mot::forward_mot_model(rgb.data, rgb.cols, rgb.rows, rgb.step, result);
        gettimeofday(&t2, NULL);
        duration += ((t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) * 0.001f);
        fprintf(stdout, "%s %fms\n", filein, duration / (i + 1));
        
        // 叠加检测和跟踪结果到图像上
        std::vector<mot::MOT_Track>::iterator riter;
        for (riter = result.begin(); riter != result.end(); riter++)
        {
            cv::Point pt1;
            std::deque<mot::MOT_Rect>::iterator iter;
            for (iter = riter->rects.begin(); iter != riter->rects.end(); iter++)
            {
                int l = static_cast<int>(iter->left);
                int t = static_cast<int>(iter->top);
                int r = static_cast<int>(iter->right);
                int b = static_cast<int>(iter->bottom);
                
                // 过滤无效的检测框
                if (l == 0 && t == 0 && r == 0 && b == 0)
                    break;
                
                // 画轨迹
                srand(riter->identifier);
                cv::Scalar color(rand() % 255, rand() % 255, rand() % 255);
                if (iter != riter->rects.begin())
                {
                    cv::Point pt2 = cv::Point((l + r) >> 1, b);
                    cv::line(bgr, pt1, pt2, color, 2);
                    pt1 = pt2;
                    continue;
                }
                
                // 画边框                
                cv::rectangle(bgr, cv::Point(l, t), cv::Point(r, b), color, 2);
                
                // 叠加轨迹ID号
                std::ostringstream oss;
                oss << riter->identifier;
                cv::String text = oss.str();
                
                int baseline;
                cv::Size tsize = cv::getTextSize(text, fontface, fontscale, thickness, &baseline);
                
                int x = std::min(std::max(l, 0), bgr.cols - tsize.width - 1);
                int y = std::min(std::max(b - baseline, tsize.height), bgr.rows - baseline - 1);
                cv::putText(bgr, text, cv::Point(x, y), fontface, fontscale, cv::Scalar(0,255,255), thickness);
                
                pt1 = cv::Point((l + r) >> 1, b);
            }
        }
        
        // 保存结果图像
        char fileout[128] = {0};
        strcat(fileout, "./result/");
        strcat(fileout, dir[i]->d_name);
        cv::imwrite(fileout, bgr);
        free(dir[i]);
    }
    
    free(dir);
    // 4. 卸载MOT模型
    mot::unload_mot_model();
}