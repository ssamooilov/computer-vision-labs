//
// Created by Samoilov Sergei on 03.03.2016.
//

#include "Image.h"

Image::Image(const int width, const int height) {
    this->width = width;
    this->height = height;
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