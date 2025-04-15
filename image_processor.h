// image_processor.h
#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <QString>
#include "node.h"

class ImageProcessor
{
public:
    // Apply image processing based on the node type and properties
    static cv::Mat processNode(Node *node, const cv::Mat &inputImage);

    // Convert between Qt and OpenCV image formats
    static cv::Mat QImageToCvMat(const QImage &image);
    static QImage CvMatToQImage(const cv::Mat &mat);

    // Process blur operation
    static cv::Mat applyBlur(const cv::Mat &inputImage, int radius, const QString &blurType);

    // Process brightness/contrast adjustment
    static cv::Mat applyBrightnessContrast(const cv::Mat &inputImage, int brightness, int contrast);

    // Process grayscale conversion
    static cv::Mat applyGrayscale(const cv::Mat &inputImage, const QString &method);

    // Process sharpen operation
    static cv::Mat applySharpen(const cv::Mat &inputImage, int amount);
    // Process color channel splitting operation
    static cv::Mat applyChannelSplit(const cv::Mat &inputImage, int channelIndex, bool grayscale);
};

#endif // IMAGE_PROCESSOR_H
