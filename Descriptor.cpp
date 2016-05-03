//
// Created by Samoilov Sergei on 28.04.2016.
//

#include "Descriptor.h"

double gauss(int x, int y) {
    const double sigma = 2;
    return exp(-(x*x + y*y)/(2*sigma*sigma)) / sqrt(2 * M_PI * sigma * sigma);
}

Descriptor::Descriptor(const Image &sobelX, const Image &sobelY, const InterestingPoint &point, BorderType borderType) :
        x(point.x), y(point.y) {
    data.fill(0);
    int size = HISTOGRAMS_COUNT * HISTOGRAM_SIZE;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            double dx = sobelX.get(point.x + x - size/2, point.y + y - size/2, borderType);
            double dy = sobelY.get(point.x + x - size/2, point.y + y - size/2, borderType);
            int histogramNumber = x / HISTOGRAM_SIZE * HISTOGRAMS_COUNT + y / HISTOGRAM_SIZE;
            double angleNumber = (atan2(dy, dx) / M_PI + 1) * ANGLES_COUNT / 2;
            double weight = sqrt(dx*dx + dy*dy) * gauss(x - size/2, y - size/2);
            double value = weight * ((int)angleNumber - angleNumber + 1);
            data[histogramNumber * ANGLES_COUNT + angleNumber] += value;
            value = weight * (angleNumber - (int)angleNumber);
            data[(int)(histogramNumber * ANGLES_COUNT + angleNumber + 1) % ANGLES_COUNT] += value;
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
        sum += value * value;
    sum = sqrt(sum);
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] / sum;
}

double Descriptor::calculateDistance(const Descriptor &other) {
    double result = 0;
    for (int i = 0; i < data.size(); ++i)
        result += (data[i] - other.data[i]) * (data[i] - other.data[i]);
    return sqrt(result);
}

int Descriptor::getX() const {
    return x;
}

int Descriptor::getY() const {
    return y;
}