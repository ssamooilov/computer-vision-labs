//
// Created by Samoilov Sergei on 18.03.2016.
//

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
