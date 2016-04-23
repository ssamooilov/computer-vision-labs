//
// Created by Samoilov Sergei on 03.03.2016.
//

#ifndef CVLAB1_IMAGE_H
#define CVLAB1_IMAGE_H

#include <memory>
#include <algorithm>
#include <math.h>
#include <QtGui>
#include "KernelFactory.h"

using namespace std;

enum class BorderType { Dummy, Border, Mirror, Cylinder };

class Image {
private:
    int width, height;
    unique_ptr<double[]> data;
    double _convolutionCell(int x, int y, const Kernel &kernel, BorderType borderType) const;
public:
    Image();
    Image(const int width, const int height);
    Image(const Image &image);
    Image(Image &&image);
    Image& operator=(Image&& other);
    operator bool () const noexcept;
    double get(const int x, const int y) const;
    double get(const int x, const int y, const BorderType borderType) const;
    double get(const int x, const int y, const int begin_sigma, int sigma) const;
    void set(const int x, const int y, const double value);
    void set(const int x, const int y, const int red, const int green, const int blue);
    int getWidth() const;
    int getHeight() const;
    void output(QString fileName) const;
    unique_ptr<Image> calculateHypotenuse(const Image &image) const;
    unique_ptr<Image> normalize() const;
    unique_ptr<Image> convolution(const Kernel &kernel, BorderType normingType) const;
    unique_ptr<Image> scale() const;
};


#endif //CVLAB1_IMAGE_H
