//
// Created by Samoilov Sergei on 03.04.2016.
//

#include <qDebug>
#include "Pyramid.h"

Pyramid::Pyramid(const Image &image) {
    double k = pow(2.0, 1.0 / layers_per_octave);

    int octaves_count = 0, size = min(image.getWidth(), image.getHeight());
    for (; size > min_size; octaves_count++)
        size /= 2;
    sigmas = make_unique<double []>((size_t) size * layers_per_octave);
    sigmas[0] = zero_sigma;

    double base_sigma = sqrt(zero_sigma*zero_sigma - begin_sigma*begin_sigma);
    auto layer = image.convolution(*KernelFactory::buildGaussX(base_sigma), NormingType::Mirror);
    layer = layer->convolution(*KernelFactory::buildGaussY(base_sigma), NormingType::Mirror);
    layers.push_back(*layer);

    for (int i = 1; i < octaves_count * layers_per_octave; ++i) {
        sigmas[i] = sigmas[i-1] * k;
        double delta_sigma = sqrt(sigmas[i]*sigmas[i] - sigmas[i-1]*sigmas[i-1]);
        layer = layer->convolution(*KernelFactory::buildGaussX(delta_sigma), NormingType::Mirror);
        layer = layer->convolution(*KernelFactory::buildGaussY(delta_sigma), NormingType::Mirror);
        if (i % octaves_count == 0) layer = layer->scale();
        layers.push_back(*layer);
    }
}
