//
// Created by Samoilov Sergei on 25.02.2016.
//

#ifndef CVLAB1_CONVOLUTION_H
#define CVLAB1_CONVOLUTION_H

#include <QDebug>
#include <memory>

using namespace std;

enum class ConvolutionType { Test, SobelX, SobelY };

class Convolution {
private:
    size_t coreWidth, coreHeight;
    unique_ptr<double[]> core;
    double calculatePixel(const unique_ptr<double[]> &image, int x, int y, size_t width, size_t height);
public:
    Convolution(ConvolutionType type);
    unique_ptr<double []> calculate(const unique_ptr<double []> &inImage, size_t width, size_t height);
    int getIndex(int index, size_t width);
};


#endif //CVLAB1_CONVOLUTION_H
