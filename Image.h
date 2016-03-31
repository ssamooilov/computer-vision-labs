//
// Created by Samoilov Sergei on 03.03.2016.
//

#ifndef CVLAB1_IMAGE_H
#define CVLAB1_IMAGE_H

#include <memory>
#include <algorithm>
#include <math.h>
#include "KernelFactory.h"

using namespace std;

enum class NormingType { Dummy, Border, Mirror, Cylinder };

class Image {
private:
    const int width, height;
    unique_ptr<double[]> data;
    double _convolutionCell(int x, int y, const Kernel &kernel, NormingType normingType) const;
    int _convolutionIndex(int index, int size, NormingType normingType) const;
public:
    Image(const int width, const int height);
    double get(const int x, const int y) const;
    void set(const int x, const int y, const double value);
    void set(const int x, const int y, const int red, const int green, const int blue);
    int getWidth() const;
    int getHeight() const;
    unique_ptr<Image> calculateHypotenuse(const Image &image) const;
    unique_ptr<Image> normalize() const;
    unique_ptr<Image> convolution(const Kernel &kernel, NormingType normingType) const;
    unique_ptr<Image> scale() const;
};


#endif //CVLAB1_IMAGE_H
