//
// Created by Samoilov Sergei on 03.04.2016.
//

#ifndef CVLAB1_PYRAMID_H
#define CVLAB1_PYRAMID_H

#include <vector>
#include "Image.h"

using namespace std;

class Pyramid {
    const int min_size = 64, layers_per_octave = 4;
    const double begin_sigma = 0.5, zero_sigma = 1.6;
public:
    unique_ptr<double []> sigmas;
    vector<Image> layers;
    Pyramid(const Image &image);
};


#endif //CVLAB1_PYRAMID_H
