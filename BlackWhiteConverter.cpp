//
// Created by Samoilov Sergei on 26.02.2016.
//

#include "BlackWhiteConverter.h"

BlackWhiteConverter::BlackWhiteConverter() {
    red = 0.299;
    green = 0.587;
    blue = 0.114;
}

unique_ptr<double []> BlackWhiteConverter::convert(const QImage & qImage, size_t width, size_t height) {
    auto image = make_unique<double []>(width * height);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            image[(j * width) + i] = min(1.0, (
                    red * qRed(qImage.pixel(i, j))
                    + green * qGreen(qImage.pixel(i, j))
                    + blue * qBlue(qImage.pixel(i, j))) / 255.);
        }
    }
    return image;
}
