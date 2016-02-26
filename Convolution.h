//
// Created by Samoilov Sergei on 25.02.2016.
//

#ifndef CVLAB1_CONVOLUTION_H
#define CVLAB1_CONVOLUTION_H

#include <QDebug>
#include <memory>

using namespace std;

enum class ConvolutionType { Test, SobelX, SobelY };
enum class NormingType { Dummy, Border, Mirror };

class Convolution {
private:
    size_t coreWidth, coreHeight;
    unique_ptr<double[]> core;
    NormingType normingType;
    double calculatePixel(const unique_ptr<double[]> &image, int x, int y, size_t width, size_t height);
public:
    Convolution(ConvolutionType convolutionType, NormingType normingType);
    unique_ptr<double []> calculate(const unique_ptr<double []> &inImage, size_t width, size_t height);
    int getIndex(int index, int width);
};


#endif //CVLAB1_CONVOLUTION_H
