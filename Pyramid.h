//
// Created by Samoilov Sergei on 03.04.2016.
//

#ifndef CVLAB1_PYRAMID_H
#define CVLAB1_PYRAMID_H

#include <vector>
#include "Image.h"

using namespace std;

struct Layer {
    Image image;
    double local_sigma, global_sigma;
};

class Pyramid {
    const int min_size = 64, layers_per_octave = 4;
    const double begin_sigma = 0.5, zero_sigma = 1.6;
    const double k = pow(2.0, 1.0 / layers_per_octave);
    const double base_sigma = sqrt(zero_sigma*zero_sigma - begin_sigma*begin_sigma);
public:
    unique_ptr<double []> sigmas;
    vector<vector<Layer>> octaves;
    Pyramid(const Image &image, int octaves_count);
};


#endif //CVLAB1_PYRAMID_H
