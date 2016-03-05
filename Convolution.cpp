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
            double kernelArray[9] = {
                    1. / 9, 1. / 9, 1. / 9,
                    1. / 9, 1. / 9, 1. / 9,
                    1. / 9, 1. / 9, 1. / 9
            };
            copy(begin(kernelArray), end(kernelArray), kernel.get());
        } break;
        case ConvolutionType::SobelX: {
            kernelWidth = 3;
            kernelHeight = 3;
            kernel = make_unique<double []>(kernelWidth * kernelHeight);
            double kernelArray[9] = {
                    -1, 0, 1,
                    -2, 0, 2,
                    -1, 0, 1
            };
            copy(begin(kernelArray), end(kernelArray), kernel.get());
        } break;
        case ConvolutionType::SobelY: {
            kernelWidth = 3;
            kernelHeight = 3;
            kernel = make_unique<double []>(kernelWidth * kernelHeight);
            double kernelArray[9] = {
                    -1, -2, -1,
                    0, 0, 0,
                    1, 2, 1
            };
            copy(begin(kernelArray), end(kernelArray), kernel.get());
        } break;
    }
}

unique_ptr<Image> Convolution::calculate(const Image &image) const {
    auto result = make_unique<Image>(image.getWidth(), image.getHeight());
    for (int i = 0; i < image.getHeight(); ++i) {
        for (int j = 0; j < image.getWidth(); ++j) {
            result->set(j, i, calculateCell(image, j, i));
        }
    }
    return result;
}

double Convolution::calculateCell(const Image &image, int x, int y) const {
    double result = 0;
    for (int kernelY = 0; kernelY < kernelHeight; ++kernelY) {
        for (int kernelX = 0; kernelX < kernelWidth; ++kernelX) {
            int indexX = calculateIndex(x - kernelX + kernelWidth / 2, image.getWidth());
            int indexY = calculateIndex(y - kernelY + kernelHeight / 2, image.getHeight());
            double value = indexX == -1 || indexY == -1 ? 0 : image.get(indexX, indexY);
            result += value * kernel[(kernelY * kernelWidth) + kernelX];
        }
    }
    return result;
}

int Convolution::calculateIndex(int index, int size) const {
    switch (normingType) {
        case NormingType::Dummy: {
            return index < 0 || index >= size ? -1 : index;
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
