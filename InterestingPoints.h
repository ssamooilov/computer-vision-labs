//
// Created by Samoilov Sergei on 23.04.2016.
//

#ifndef CVLAB1_INTERESTINGPOINTS_H
#define CVLAB1_INTERESTINGPOINTS_H

#include "Image.h"
#include "math.h"
#include "Pyramid.h"

enum class InterestingPointsMethod { Moravek, Harris, Blob, Blob2
};

struct InterestingPoint {
    int global_x, global_y, octave, layer;
    double weight, local_x, local_y, local_sigma, global_sigma, orientation;
};

class InterestingPointsSearcher {
private:
    const int WINDOW_SHIFT = 2, WINDOW_SIGMA = 1, EXTRACT_SHIFT = 2;
    const double MORAVEK_THRESHOLD = 0.01, HARRIS_THRESHOLD = 0.01, FILTER_FACTOR = 0.9;
    Image image;
    unique_ptr<Pyramid> pyramid = nullptr;
    vector<InterestingPoint> points;
    void extractInterestingPoints(Image &contrast, double threshold, BorderType borderType);
    bool checkOnePoint(Pyramid &pyramid, BorderType borderType, int pointX, int pointY, int octaveIndex,
                       int layerIndex, double centerValue) const;
public:
    InterestingPointsSearcher(const Image &image, InterestingPointsMethod method, BorderType borderType);
    void moravek(BorderType borderType);
    void harris(BorderType borderType);
    void adaptiveNonMaximumSuppression(const int countPoints);
    void output(QString fileName, bool withScale) const;
    vector<InterestingPoint> & getPoints();
    void blob(BorderType borderType);
    bool contains(InterestingPoint & point) const;

    const Pyramid &getPyramid() const {
        return *pyramid;
    }

    void blob2(BorderType borderType);

    Image getHarris(const Image &image, const BorderType &borderType) const;
};


#endif //CVLAB1_INTERESTINGPOINTS_H
