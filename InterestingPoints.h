//
// Created by Samoilov Sergei on 23.04.2016.
//

#ifndef CVLAB1_INTERESTINGPOINTS_H
#define CVLAB1_INTERESTINGPOINTS_H

#include "Image.h"
#include "math.h"

enum class InterestingPointsMethod { Moravek, Harris };

struct InterestingPoint {
    const int x, y;
    const double weight;

    InterestingPoint(const int x, const int y, const double weight) : x(x), y(y), weight(weight) {}
};

class InterestingPointsSearcher {
private:
    const int WINDOW_SHIFT = 2, LOCAL_SHIFT = 2, POINT_SHIFT = 2;
    const double THRESHOLD = 0.01;
    Image image;
    vector<InterestingPoint> points;
public:
    InterestingPointsSearcher(const Image &image, InterestingPointsMethod method, BorderType borderType);
    void moravek(BorderType borderType);
    void harris();
    void output(QString fileName) const;
};


#endif //CVLAB1_INTERESTINGPOINTS_H
