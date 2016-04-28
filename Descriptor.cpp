//
// Created by Samoilov Sergei on 28.04.2016.
//

#include "Descriptor.h"


Descriptor::Descriptor(const Image &sobelX, const Image &sobelY, const InterestingPoint &point, BorderType borderType) {
    data.fill(0); // надо?
    auto kernel = KernelFactory::buildGaussX(WINDOW_SIGMA);
    int dk = kernel->width / 2 + 1;
    for (int y = -WINDOW_SHIFT; y < WINDOW_SHIFT; ++y) {
        for (int x = -WINDOW_SHIFT; x < WINDOW_SHIFT; ++x) {
            double dx = sobelX.get(point.x + x, point.y + y, borderType);
            double dy = sobelY.get(point.x + x, point.y + y, borderType);
            int histogramNumber = 2 * (y < 0) + (x < 0);
            double angleNumber = (atan2(dy, dx) / M_PI + 1) * (ANGLES_COUNT / 2);
            double value = sqrt(dx*dx + dy*dy) * kernel->data[dk + abs(x) + abs(y)];
            data[histogramNumber * ANGLES_COUNT + angleNumber] += value;
        }
    }
    normalize();
    for (int i = 0; i < data.size(); ++i)
        data[i] = min(NORMALIZE_THRESHOLD, data[i]);
    normalize();
}

void Descriptor::normalize() {
    double sum = 0;
    for (auto value : data)
        sum += value;
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] / sum;
}

const array<double, 32> &Descriptor::getData() const {
    return data;
}
