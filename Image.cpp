//
// Created by Samoilov Sergei on 03.03.2016.
//

#include "Image.h"

Image::Image(const int width, const int height) : width(width), height(height) {
    data = make_unique<double []>((size_t) (width * height));
}

double Image::get(const int x, const int y) const {
    return data[(y * width) + x];
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
