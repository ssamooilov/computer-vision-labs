//
// Created by Samoilov Sergei on 25.02.2016.
//


#include "Convolution.h"

using namespace std;

Convolution::Convolution(ConvolutionType convolutionType, NormingType normingType_) {
    normingType = normingType_;
    switch (convolutionType) {
        case ConvolutionType::Test: {
            coreWidth = 3;
            coreHeight = 3;
            core = make_unique<double []>(coreWidth * coreHeight);
            core[0] = 1./9; core[1] = 1./9; core[2] = 1./9;
            core[3] = 1./9; core[4] = 1./9; core[5] = 1./9;
            core[6] = 1./9; core[7] = 1./9; core[8] = 1./9;
        } break;
        case ConvolutionType::SobelX: {
            coreWidth = 3;
            coreHeight = 3;
            core = make_unique<double []>(coreWidth * coreHeight);
            core[0] = 1; core[1] = 0; core[2] = -1;
            core[3] = 2; core[4] = 0; core[5] = -2;
            core[6] = 1; core[7] = 0; core[8] = -1;
        } break;
        case ConvolutionType::SobelY: {
            coreWidth = 3;
            coreHeight = 3;
            core = make_unique<double []>(coreWidth * coreHeight);
            core[0] = 1; core[1] = 2; core[2] = 1;
            core[3] = 0; core[4] = 0; core[5] = 0;
            core[6] = -1; core[7] = -2; core[8] = -1;
        } break;
    }
}

unique_ptr<double[]> Convolution::calculate(const unique_ptr<double[]> &inImage, size_t width, size_t height) {
    auto outImage = make_unique<double []>(width * height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            outImage[(y * width) + x] = calculatePixel(inImage, x, y, width, height);
        }
    }
    return outImage;
}

double Convolution::calculatePixel(const unique_ptr<double[]> &image, int x, int y, size_t width, size_t height) {
    double result = 0;
    for (int coreX = 0; coreX < coreWidth; ++coreX) {
        for (int coreY = 0; coreY < coreHeight; ++coreY) {
            result += image[(getIndex(y + coreY - coreHeight / 2, height) * width)
                            + getIndex(x + coreX - coreWidth / 2, width)] * core[(coreY * coreWidth) + coreX];
        }
    }
    return result;
}

int Convolution::getIndex(int index, int width) {
    switch (normingType) {
        case NormingType::Dummy: {
            return index < 0 || index >= width ? 0 : index;
        }
        case NormingType::Border: {
            return min(max(index, 0),width - 1);
        }
        case NormingType::Mirror: {
            index = abs(index);
            return index >= width ? width * 2 - index - 2 : index;
        }
    }

}
