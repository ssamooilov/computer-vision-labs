//
// Created by Samoilov Sergei on 18.03.2016.
//

#ifndef CVLAB1_KERNELFACTORY_H
#define CVLAB1_KERNELFACTORY_H

#include <memory>

using namespace std;

struct Kernel {
    int width, height;
    unique_ptr<double[]> data;
};

class KernelFactory {
public:
    static unique_ptr<Kernel> buildSobelX();
    static unique_ptr<Kernel> buildSobelY();
    static unique_ptr<Kernel> buildGaussX(double sigma);
    static unique_ptr<Kernel> buildGaussY(double sigma);
};


#endif //CVLAB1_KERNELFACTORY_H
