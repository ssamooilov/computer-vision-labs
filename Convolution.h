//
// Created by Samoilov Sergei on 25.02.2016.
//

#ifndef CVLAB1_CONVOLUTION_H
#define CVLAB1_CONVOLUTION_H

#include <memory>
#include "Image.h"

using namespace std;

enum class ConvolutionType { Test, SobelX, SobelY };
enum class NormingType { Dummy, Border, Mirror, Cylinder };

class Convolution {
private:
    size_t kernelWidth, kernelHeight;
    unique_ptr<double[]> kernel;
    NormingType normingType;
    double calculatePixel(const Image *image, int x, int y) const;
    int calculateIndex(int index, int size) const;
public:
    Convolution(ConvolutionType convolutionType, NormingType normingType);
    unique_ptr<Image> calculate(const Image *image) const;
};


#endif //CVLAB1_CONVOLUTION_H
