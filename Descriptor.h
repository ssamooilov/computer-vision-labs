//
// Created by Samoilov Sergei on 28.04.2016.
//

#ifndef CVLAB1_DESCRIPTOR_H
#define CVLAB1_DESCRIPTOR_H


#include "InterestingPoints.h"
#include "Image.h"

class Descriptor {
private:
    static const int WINDOW_SHIFT = 4, WINDOW_SIGMA = 2, ANGLES_COUNT = 8;
    double NORMALIZE_THRESHOLD = 0.2;
    int x, y;
    array<double, ANGLES_COUNT * 4> data;
    void normalize();
public:
    Descriptor(const Image &sobelX, const Image &sobelY, const InterestingPoint &point, BorderType borderType);
    double calculateDistanse(const Descriptor & other);
    int getX() const;
    int getY() const;
    const array<double, 32> & getData() const;
};


#endif //CVLAB1_DESCRIPTOR_H
