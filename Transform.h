//
// Created by Samoilov Sergei on 16.10.2016.
//

#ifndef CVLAB1_TRANSFORM_H
#define CVLAB1_TRANSFORM_H

#include <vector>
#include <array>
#include <QGlobal.h>
#include <QTime>
#include <qDebug>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

using namespace std;

class Transform {
private:
    const double INLINERS_THRESHOLD = 5, ITERATIONS_COUNT = 10000;
    vector<int> & getRandomIndexes(int size, int count);
    array<double, 9> & getTransform(vector<array<int, 4>> matches);
public:
    array<double, 9> & ransac(vector<array<int, 4>> matches);
};


#endif //CVLAB1_TRANSFORM_H
