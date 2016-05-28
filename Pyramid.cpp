//
// Created by Samoilov Sergei on 03.04.2016.
//

#include <qDebug>
#include "Pyramid.h"

Pyramid::Pyramid(const Image &image, int octaves_count) {
    for (int i = 0; i < octaves_count; ++i)
        octaves.emplace_back(vector<Layer>());

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
                layer.image = move(*octaves[i-1][LAYERS_PER_OCTAVE].image.scale());
                layer.global_sigma = octaves[i-1][0].global_sigma * 2;
                layer.local_sigma = octaves[i-1][0].local_sigma;
            } else {
                auto &prevLayer = octaves[i][j-1];
                layer.global_sigma = prevLayer.global_sigma * k;
                layer.local_sigma = prevLayer.local_sigma * k;
                double delta_sigma = sqrt(
                        layer.local_sigma*layer.local_sigma - prevLayer.local_sigma*prevLayer.local_sigma);
                auto layer_image = prevLayer.image.convolution(*KernelFactory::buildGaussX(delta_sigma), BorderType::Mirror);
                layer_image = layer_image->convolution(*KernelFactory::buildGaussY(delta_sigma), BorderType::Mirror);
                layer.image = move(*layer_image);
            }
            octaves[i].emplace_back(move(layer));
        }
    }
}

void Pyramid::calculateDiffs() {
    diffs.clear();
    for (int i = 0; i < octaves.size(); ++i) {
        diffs.emplace_back(vector<Layer>());
        for (int j = 0; j < octaves[i].size() - 1; ++j) {
            auto layer = Layer();
            layer.local_sigma = octaves[i][j].local_sigma;
            layer.global_sigma = octaves[i][j].global_sigma;
            layer.image = move(*octaves[i][j].image.calculateSubstitution(octaves[i][j+1].image));
            diffs[i].emplace_back(layer);
        }
    }
}
