﻿#pragma once
#include <opencv2/opencv.hpp>
struct ResNetResult
{
    int classId;  //分类ID
    cv::Rect box;  //区域位置
};
