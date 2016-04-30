//
// Created by Samoilov Sergei on 28.04.2016.
//

#ifndef CVLAB1_DESCRIPTOR_H
#define CVLAB1_DESCRIPTOR_H


#include "InterestingPoints.h"
#include "Image.h"

class Descriptor {
private:
    static const int HISTOGRAMS_COUNT = 4, ANGLES_COUNT = 8, HISTOGRAM_SIZE = 4;
    double NORMALIZE_THRESHOLD = 0.2;
    int x, y;
    array<double, ANGLES_COUNT * HISTOGRAMS_COUNT * HISTOGRAMS_COUNT> data;
    void normalize();
public:
    Descriptor(const Image &sobelX, const Image &sobelY, const InterestingPoint &point, BorderType borderType);
    double calculateDistance(const Descriptor &other);
    int getX() const;
    int getY() const;
    const array<double, ANGLES_COUNT * HISTOGRAMS_COUNT * HISTOGRAMS_COUNT> & getData() const {
        return data;
    };
};


#endif //CVLAB1_DESCRIPTOR_H
