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
    const int LAYERS_PER_OCTAVE = 4;
    const double BEGIN_SIGMA = 0.5, ZERO_SIGMA = 1.6;
    const double k = pow(2.0, 1.0 / LAYERS_PER_OCTAVE);
    const double base_sigma = sqrt(ZERO_SIGMA*ZERO_SIGMA - BEGIN_SIGMA*BEGIN_SIGMA);
public:
    vector<vector<Layer>> octaves;
    vector<vector<Layer>> diffs;
    Pyramid(const Image &image, int octaves_count);
    void calculateDiffs();
};


#endif //CVLAB1_PYRAMID_H
