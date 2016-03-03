//
// Created by Samoilov Sergei on 25.02.2016.
//


#include "Convolution.h"

using namespace std;

Convolution::Convolution(ConvolutionType convolutionType, NormingType normingType) {
    this->normingType = normingType;
    switch (convolutionType) {
        case ConvolutionType::Test: {
            kernelWidth = 3;
            kernelHeight = 3;
            kernel = make_unique<double []>(kernelWidth * kernelHeight);
            kernel[0] = 1. / 9; kernel[1] = 1. / 9; kernel[2] = 1. / 9;
            kernel[3] = 1. / 9; kernel[4] = 1. / 9; kernel[5] = 1. / 9;
            kernel[6] = 1. / 9; kernel[7] = 1. / 9; kernel[8] = 1. / 9;
        } break;
        case ConvolutionType::SobelX: {
            kernelWidth = 3;
            kernelHeight = 3;
            kernel = make_unique<double []>(kernelWidth * kernelHeight);
            kernel[0] = 1; kernel[1] = 0; kernel[2] = -1;
            kernel[3] = 2; kernel[4] = 0; kernel[5] = -2;
            kernel[6] = 1; kernel[7] = 0; kernel[8] = -1;
        } break;
        case ConvolutionType::SobelY: {
            kernelWidth = 3;
            kernelHeight = 3;
            kernel = make_unique<double []>(kernelWidth * kernelHeight);
            kernel[0] = 1; kernel[1] = 2; kernel[2] = 1;
            kernel[3] = 0; kernel[4] = 0; kernel[5] = 0;
            kernel[6] = -1; kernel[7] = -2; kernel[8] = -1;
        } break;
    }
}

unique_ptr<Image> Convolution::calculate(const Image *image) const {
    unique_ptr<Image> result = make_unique<Image>(image->getWidth(), image->getHeight());
    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            result->set(j, i, calculatePixel(image, j, i));
        }
    }
    return result;
}

double Convolution::calculatePixel(const Image *image, int x, int y) const {
    double result = 0;
    for (int kernelY = 0; kernelY < kernelHeight; ++kernelY) {
        for (int kernelX = 0; kernelX < kernelWidth; ++kernelX) {
            result += image->get(calculateIndex(x + kernelX - kernelWidth / 2, image->getWidth()),
                                 calculateIndex(y + kernelY - kernelHeight / 2, image->getHeight()))
                                 * kernel[(kernelY * kernelWidth) + kernelX];
        }
    }
    return result;
}

int Convolution::calculateIndex(int index, int size) const {
    switch (normingType) {
        case NormingType::Dummy: {
            return index < 0 || index >= size ? 0 : index;
        }
        case NormingType::Border: {
            return min(max(index, 0), size - 1);
        }
        case NormingType::Mirror: {
            index = abs(index);
            return index >= size ? size * 2 - index - 2 : index;
        }
        case NormingType::Cylinder: {
            return (index + size) % size;
        }
    }
}
