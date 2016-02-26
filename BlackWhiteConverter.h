//
// Created by Samoilov Sergei on 26.02.2016.
//

#ifndef CVLAB1_BLACKWHITECONVERTER_H
#define CVLAB1_BLACKWHITECONVERTER_H

#include <QtGui>
#include <memory>

using namespace std;

class BlackWhiteConverter {
private:
    double red, green, blue;
public:
    BlackWhiteConverter();
    unique_ptr<double []> convert(const QImage &qImage, size_t width, size_t height);
};


#endif //CVLAB1_BLACKWHITECONVERTER_H
