//
// Created by Samoilov Sergei on 03.03.2016.
//

#include "Image.h"

Image::Image(const int width, const int height) : width(width), height(height) {
    data = make_unique<double []>((size_t) (width * height));
}

Image::Image(const Image &image) : width(image.getWidth()), height(image.getHeight()) {
    data = make_unique<double []>((size_t) (width * height));
    for (int i = 0; i < width * height; ++i)
        data[i] = image.data[i];
}

Image::Image(const Image &&image) : width(image.getWidth()), height(image.getHeight()) {
    data = move(image.data);
    image.data = nullptr;
}

double Image::get(const int x, const int y) const {
    return data[(y * width) + x];
}

double Image::get(const int x, const int y, const int begin_sigma, int sigma) const {
    int layer = 0;
    while (sigma > begin_sigma)
        sigma /= 2;
    return data[(y / (2*layer) * width) + x / (2*layer)];;
}

void Image::set(const int x, const int y, const double value) {
    data[(y * width) + x] = value;
}

void Image::set(const int x, const int y, const int red, const int green, const int blue) {
    data[(y * width) + x] = min(1.0, (red*0.299 + green*0.587 + blue*0.114) / 255.);
}

int Image::getWidth() const {
    return width;
}

int Image::getHeight() const {
    return height;
}

unique_ptr<Image> Image::calculateHypotenuse(const Image &image) const {
    auto result = make_unique<Image>(width, height);
    for (int i = 0; i < width * height; ++i) {
        result->data[i] = sqrt(data[i] * data[i] + image.data[i] * image.data[i]);
    }
    return result;
}

unique_ptr<Image> Image::normalize() const {
    auto result = make_unique<Image>(width, height);
    double minValue = *min_element(&data[0], &data[width * height]);
    double maxValue = *max_element(&data[0], &data[width * height]);
    double range = maxValue - minValue;
    range = range == 0 ? 1 : range;
    for (int i = 0; i < width * height; ++i) {
        result->data[i] = (data[i] - minValue) / range;
    }
    return result;
}

unique_ptr<Image> Image::convolution(const Kernel &kernel, NormingType normingType) const {
    auto result = make_unique<Image>(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result->set(j, i, _convolutionCell(j, i, kernel, normingType));
        }
    }
    return result;
}

double Image::_convolutionCell(int x, int y, const Kernel &kernel, NormingType normingType) const {
    double result = 0;
    for (int kernelY = 0; kernelY < kernel.height; ++kernelY) {
        for (int kernelX = 0; kernelX < kernel.width; ++kernelX) {
            int indexX = _convolutionIndex(x - kernelX + kernel.width / 2, width, normingType);
            int indexY = _convolutionIndex(y - kernelY + kernel.height / 2, height, normingType);
            double value = indexX == -1 || indexY == -1 ? 0 : get(indexX, indexY);
            result += value * kernel.data[(kernelY * kernel.width) + kernelX];
        }
    }
    return result;
}

int Image::_convolutionIndex(int index, int size, NormingType normingType) const {
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

unique_ptr<Image> Image::scale() const {
    auto result = make_unique<Image>(width / 2, height / 2);
    for (int i = 0; i < result->getHeight(); ++i)
        for (int j = 0; j < result->getWidth(); ++j)
            result->set(i, j, (get(i*2, j*2) + get(i*2+1, j*2) + get(i*2, j*2+1) + get(i*2+1, j*2+1)) / 4);
    return result;
}
