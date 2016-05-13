//
// Created by Samoilov Sergei on 04.05.2016.
//

#ifndef CVLAB1_DESCRIPTORSKIT_H
#define CVLAB1_DESCRIPTORSKIT_H

#include "Image.h"
#include "InterestingPoints.h"

static double MATCH_THRESHOLD = 0.15, NORMALIZE_THRESHOLD = 0.2;
static const int HISTOGRAM_SIZE = 4, HISTOGRAMS_COUNT = 4, ANGLES_COUNT = 8;
static const int BIG_HISTOGRAM_SIZE = 16, BIG_HISTOGRAMS_COUNT = 1, BIG_ANGLES_COUNT = 36;
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
    unique_ptr<double[]> calculateData(const Pyramid &pyramid,
                                       const InterestingPoint &point, double rotateAngle,
                                       int histogramsCount, int histogramSize, int anglesCount);
    void normalize(Desc &desc);
    double gauss(int x, int y);
public:
    DescriptorsKit(const Image &image, BorderType borderType);
    vector<array<int, 4>> findMatches(DescriptorsKit &other);
    const vector<Desc> &getDescs() const {
        return descs;
    }

    Desc buildDesc(const Pyramid &pyramid, const InterestingPoint &point,
                   const unique_ptr<double[], default_delete<double[]>> &anglesData, int indexOfMax);
};


#endif //CVLAB1_DESCRIPTORSKIT_H
