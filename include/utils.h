#pragma once
#include <codecvt>
#include <fstream>
#include <opencv2/opencv.hpp>


struct Detection
{
    cv::Rect box;//矩形框
    float conf{};//置信度
    int classId{};//类别
};

namespace utilss
{
    size_t vectorProduct(const std::vector<int64_t>& vector);
    std::wstring charToWstring(const char* str);//字符转字符串
    std::vector<std::string> loadNames(const std::string& path);//加载className
    void visualizeDetection(cv::Mat& image, std::vector<Detection>& detections,
                            const std::vector<std::string>& classNames);//画目标框

    //将原图变成预定大小的图片 640X640 方便模型进行预测
    void letterbox(const cv::Mat& image, cv::Mat& outImage,
                   const cv::Size& newShape,
                   const cv::Scalar& color,
                   bool auto_,
                   bool scaleFill,
                   bool scaleUp,
                   int stride);

    void scaleCoords(const cv::Size& imageShape, cv::Rect& box, const cv::Size& imageOriginalShape); //缩放坐标

    template <typename T>
    T clip(const T& n, const T& lower, const T& upper);
}
