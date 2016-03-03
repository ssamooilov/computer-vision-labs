//
// Created by Samoilov Sergei on 03.03.2016.
//

#ifndef CVLAB1_IMAGE_H
#define CVLAB1_IMAGE_H

#include <memory>

using namespace std;

class Image {
private:
    int width, height;
    unique_ptr<double[]> data;
public:
    Image(const int width, const int height);
    double get(const int x, const int y) const;
    void set(const int x, const int y, const double value);
    void set(const int x, const int y, const int red, const int green, const int blue);
    int getWidth() const;
    int getHeight() const;
};


#endif //CVLAB1_IMAGE_H
