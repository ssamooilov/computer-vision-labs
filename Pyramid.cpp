//
// Created by Samoilov Sergei on 03.04.2016.
//

#include <qDebug>
#include "Pyramid.h"

Pyramid::Pyramid(const Image &image, int octaves_count) {
    for (int i = 0; i < octaves_count; ++i)
        octaves.push_back(vector<Layer>());

    auto first_layer_image = image.convolution(*KernelFactory::buildGaussX(base_sigma), BorderType::Mirror);
    first_layer_image = first_layer_image->convolution(*KernelFactory::buildGaussY(base_sigma), BorderType::Mirror);
    auto first_layer = Layer();
    first_layer.global_sigma = first_layer.local_sigma = ZERO_SIGMA;
    first_layer.image = move(*first_layer_image);
    octaves[0].emplace_back(first_layer);

    for (int i = 0; i < octaves_count; ++i) {
        for (int j = 0; j < LAYERS_PER_OCTAVE + 3; ++j) {
            auto layer = Layer();
            if (j == 0) {
                if (i == 0) continue;
                layer.global_sigma = octaves[i-1][0].global_sigma * 2;
                auto old_sigma = octaves[i-1][LAYERS_PER_OCTAVE-1].local_sigma;
                layer.local_sigma = old_sigma * k;
                double delta_sigma = sqrt(layer.local_sigma*layer.local_sigma - old_sigma*old_sigma);
                auto layer_image = octaves[i-1][LAYERS_PER_OCTAVE-1].image
                        .convolution(*KernelFactory::buildGaussX(delta_sigma), BorderType::Mirror);
                layer_image = layer_image->convolution(*KernelFactory::buildGaussY(delta_sigma), BorderType::Mirror);
                layer_image = layer_image->scale();
                layer.image = move(*layer_image);
                layer.local_sigma /= 2;
            } else {
                layer.global_sigma = octaves[i][j-1].global_sigma * k;
                layer.local_sigma = octaves[i][j-1].local_sigma * k;
                double delta_sigma = sqrt(
                        layer.local_sigma*layer.local_sigma - octaves[i][j-1].local_sigma*octaves[i][j-1].local_sigma);
                auto layer_image = octaves[i][j-1].image.convolution(*KernelFactory::buildGaussX(delta_sigma), BorderType::Mirror);
                layer_image = layer_image->convolution(*KernelFactory::buildGaussY(delta_sigma), BorderType::Mirror);
                layer.image = move(*layer_image);
            }
            octaves[i].emplace_back(move(layer));
        }
    }
}
