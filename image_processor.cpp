// image_processor.cpp
#include "image_processor.h"
#include <QDebug>


cv::Mat ImageProcessor::processNode(Node *node, const cv::Mat &inputImage)
{
    if (!node || inputImage.empty())
        return inputImage;

    cv::Mat resultImage = inputImage.clone();
    const QString nodeType = node->getType();

    if (nodeType == "Blur")
    {
        int radius = node->getProperty("radius")->getValue().toInt();
        QString blurType = node->getProperty("blurType")->getValue().toString();
        return applyBlur(resultImage, radius, blurType);
    }
    else if (nodeType == "Brightness")
    {
        int brightness = node->getProperty("brightness")->getValue().toInt();
        int contrast = node->getProperty("contrast")->getValue().toInt();
        return applyBrightnessContrast(resultImage, brightness, contrast);
    }
    else if (nodeType == "Grayscale")
    {
        QString method = node->getProperty("method")->getValue().toString();
        return applyGrayscale(resultImage, method);
    }
    else if (nodeType == "Sharpen")
    {
        int amount = node->getProperty("amount")->getValue().toInt();
        double contrast = node->getProperty("Contrast")->getValue().toDouble();
        resultImage = applySharpen(resultImage, amount);

        // Apply contrast after sharpening
        cv::Mat contrastImage;
        resultImage.convertTo(contrastImage, -1, contrast, 0);
        return contrastImage;
    }

    return resultImage;
}

cv::Mat ImageProcessor::applyBlur(const cv::Mat &inputImage, int radius, const QString &blurType)
{
    cv::Mat outputImage;

    if (blurType == "Uniform")
    {
        cv::GaussianBlur(inputImage, outputImage, cv::Size(2 * radius + 1, 2 * radius + 1), 0);
    }
    else if (blurType == "Directional")
    {
        // For directional blur, apply motion blur in horizontal direction
        cv::Mat kernel = cv::Mat::zeros(cv::Size(2 * radius + 1, 2 * radius + 1), CV_32F);
        kernel.row(radius).setTo(1.0 / kernel.cols);
        cv::filter2D(inputImage, outputImage, -1, kernel);
    }

    return outputImage;
}

cv::Mat ImageProcessor::applyBrightnessContrast(const cv::Mat &inputImage, int brightness, int contrast)
{
    cv::Mat outputImage;
    double alpha = 1.0 + contrast / 100.0; // Contrast factor
    double beta = brightness;              // Brightness offset

    inputImage.convertTo(outputImage, -1, alpha, beta);
    return outputImage;
}

cv::Mat ImageProcessor::applyGrayscale(const cv::Mat &inputImage, const QString &method)
{
    cv::Mat outputImage;

    if (method == "Average")
    {
        cv::cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR); // Convert back to 3 channels
    }
    else if (method == "Luminosity")
    {
        cv::cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(outputImage, outputImage, cv::COLOR_GRAY2BGR);
    }
    else if (method == "Lightness")
    {
        // Lightness method: (max(R,G,B) + min(R,G,B)) / 2
        std::vector<cv::Mat> channels;
        cv::split(inputImage, channels);

        cv::Mat maxVal, minVal;
        cv::max(channels[0], channels[1], maxVal);
        cv::max(maxVal, channels[2], maxVal);

        cv::min(channels[0], channels[1], minVal);
        cv::min(minVal, channels[2], minVal);

        cv::Mat lightness = (maxVal + minVal) / 2;
        cv::cvtColor(lightness, outputImage, cv::COLOR_GRAY2BGR);
    }

    return outputImage;
}

cv::Mat ImageProcessor::applySharpen(const cv::Mat &inputImage, int amount)
{
    cv::Mat outputImage;

    // Create sharpening kernel
    float sharpenFactor = amount / 50.0;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -sharpenFactor, 0,
                      -sharpenFactor, 1 + 4 * sharpenFactor, -sharpenFactor,
                      0, -sharpenFactor, 0);

    cv::filter2D(inputImage, outputImage, -1, kernel);
    return outputImage;
}

cv::Mat ImageProcessor::QImageToCvMat(const QImage &image)
{
    switch (image.format())
    {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat mat(image.height(), image.width(), CV_8UC4, (void *)image.constBits(), image.bytesPerLine());
        cv::Mat matRGB;
        cv::cvtColor(mat, matRGB, cv::COLOR_BGRA2BGR);
        return matRGB;
    }
    case QImage::Format_RGB888:
    {
        cv::Mat mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
        cv::Mat matRGB;
        cv::cvtColor(mat, matRGB, cv::COLOR_RGB2BGR);
        return matRGB;
    }
    default:
        // Convert to RGB32 first if not in a directly compatible format
        QImage converted = image.convertToFormat(QImage::Format_RGB32);
        cv::Mat mat(converted.height(), converted.width(), CV_8UC4, (void *)converted.constBits(), converted.bytesPerLine());
        cv::Mat matRGB;
        cv::cvtColor(mat, matRGB, cv::COLOR_BGRA2BGR);
        return matRGB;
    }
}

QImage ImageProcessor::CvMatToQImage(const cv::Mat &mat)
{
    // Handle different types of OpenCV images
    if (mat.type() == CV_8UC1)
    {
        // Grayscale image
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return image.copy();
    }
    else if (mat.type() == CV_8UC3)
    {
        // BGR color image - needs conversion to RGB
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        QImage image(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
        return image.copy(); // Copy for safety as mat.data will be deallocated
    }
    else if (mat.type() == CV_8UC4)
    {
        // BGRA image
        cv::Mat rgbaMat;
        cv::cvtColor(mat, rgbaMat, cv::COLOR_BGRA2RGBA);
        QImage image(rgbaMat.data, rgbaMat.cols, rgbaMat.rows, rgbaMat.step, QImage::Format_RGBA8888);
        return image.copy();
    }

    return QImage(); // Return empty image if format not supported
}