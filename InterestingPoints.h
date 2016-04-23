//
// Created by Samoilov Sergei on 23.04.2016.
//

#ifndef CVLAB1_INTERESTINGPOINTS_H
#define CVLAB1_INTERESTINGPOINTS_H

#include "Image.h"
#include "math.h"

enum class InterestingPointsMethod { Moravek, Harris };

struct InterestingPoint {
    int x, y;
    double weight;
};

class InterestingPointsSearcher {
private:
    const int WINDOW_SHIFT = 2, EXTRACT_SHIFT = 2, POINT_SHIFT = 2;
    const double MORAVEK_THRESHOLD = 0.01, HARRIS_THRESHOLD = 1e-17, FILTER_FACTOR = 0.9;
    Image image;
    vector<InterestingPoint> points;
    void extractInterestingPoints(Image &contrast, double threshold, BorderType borderType);
public:
    InterestingPointsSearcher(const Image &image, InterestingPointsMethod method, BorderType borderType);
    void moravek(BorderType borderType);
    void harris(BorderType borderType);
    void adaptiveNonMaximumSuppression(const int countPoints);
    void output(QString fileName) const;
};


#endif //CVLAB1_INTERESTINGPOINTS_H
