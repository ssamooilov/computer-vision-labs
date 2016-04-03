//
// Created by Samoilov Sergei on 18.03.2016.
//

#include <math.h>
#include "KernelFactory.h"

unique_ptr<Kernel> KernelFactory::buildSobelX() {
    auto kernel = make_unique<Kernel>();
    kernel->width = 3;
    kernel->height = 3;
    kernel->data = make_unique<double []>(9);
    double kernelArray[9] = {
            -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1
    };
    copy(begin(kernelArray), end(kernelArray), kernel->data.get());
    return kernel;
}

unique_ptr<Kernel> KernelFactory::buildSobelY() {
    auto kernel = make_unique<Kernel>();
    kernel->width = 3;
    kernel->height = 3;
    kernel->data = make_unique<double []>(9);
    double kernelArray[9] = {
            -1, -2, -1,
            0, 0, 0,
            1, 2, 1
    };
    copy(begin(kernelArray), end(kernelArray), kernel->data.get());
    return kernel;
}

unique_ptr<Kernel> KernelFactory::buildGaussX(double sigma) {
    auto kernel = make_unique<Kernel>();
    int size = (int) round(sigma * 3);
    kernel->width = size * 2 + 1;
    kernel->height = 1;
    kernel->data = make_unique<double []>(kernel->width);
    for (int i = -size; i <= size; ++i)
        kernel->data[i+size] = exp(-i*i/(2*sigma*sigma)) / (sqrt(2 * M_PI) * sigma);
    return kernel;
}

unique_ptr<Kernel> KernelFactory::buildGaussY(double sigma) {
    auto kernel = make_unique<Kernel>();
    int size = (int) round(sigma * 3);
    kernel->width = 1;
    kernel->height = size * 2 + 1;
    kernel->data = make_unique<double []>(kernel->height);
    for (int i = -size; i <= size; ++i)
        kernel->data[i+size] = exp(-i*i/(2*sigma*sigma)) / sqrt(2 * M_PI * sigma * sigma);
    return kernel;
}
