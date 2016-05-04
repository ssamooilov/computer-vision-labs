//
// Created by Samoilov Sergei on 04.05.2016.
//

#ifndef CVLAB1_DESCRIPTORSKIT_H
#define CVLAB1_DESCRIPTORSKIT_H

#include "Image.h"
#include "InterestingPoints.h"

static double MATCH_THRESHOLD = 0.2, NORMALIZE_THRESHOLD = 0.2;
static const int HISTOGRAM_SIZE = 4, HISTOGRAMS_COUNT = 4, ANGLES_COUNT = 8;
static const int COUNT_POINTS = 500;

struct Desc {
    int x, y, size;
    unique_ptr<double []> data = nullptr;
};

class DescriptorsKit {
private:
    Image image;
    BorderType borderType;
    vector<Desc> descs;
    unique_ptr<double[]> calculateData(const Image &sobelX, const Image &sobelY,
                                       const InterestingPoint &point,
                                       int histogramsCount, int histogramSize, int anglesCount);
    void normalize(Desc &desc);
    double gauss(int x, int y);
public:
    DescriptorsKit(const Image &image, BorderType borderType);
    vector<array<int, 4>> findMatches(DescriptorsKit &other);
    const vector<Desc> &getDescs() const {
        return descs;
    }
};


#endif //CVLAB1_DESCRIPTORSKIT_H
