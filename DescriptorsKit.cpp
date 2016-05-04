//
// Created by Samoilov Sergei on 04.05.2016.
//

#include "DescriptorsKit.h"

double DescriptorsKit::gauss(int x, int y) {
    const double sigma = 2;
    return exp(-(x*x + y*y)/(2*sigma*sigma)) / sqrt(2 * M_PI * sigma * sigma);
}

DescriptorsKit::DescriptorsKit(const Image &image, BorderType borderType) : image(image), borderType(borderType) {
    auto sobelX = image.convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = image.convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    auto searcher = make_unique<InterestingPointsSearcher>(image, InterestingPointsMethod::Harris, BorderType::Mirror);
    searcher->adaptiveNonMaximumSuppression(COUNT_POINTS);
    for (auto &point : searcher->getPoints()) {
        auto desc = Desc();
        desc.x = point.x;
        desc.y = point.y;
        desc.size = HISTOGRAMS_COUNT * HISTOGRAMS_COUNT * ANGLES_COUNT;
        desc.data = calculateData(*sobelX, *sobelY, point, HISTOGRAMS_COUNT, HISTOGRAM_SIZE, ANGLES_COUNT);
        normalize(desc);
        for (int i = 0; i < desc.size; ++i)
            desc.data[i] = min(NORMALIZE_THRESHOLD, desc.data[i]);
        normalize(desc);
        descs.emplace_back(move(desc));
    }
}

unique_ptr<double[]> DescriptorsKit::calculateData(const Image &sobelX, const Image &sobelY,
                                                   const InterestingPoint &point,
                                                   int histogramsCount, int histogramSize, int anglesCount) {
    auto data = make_unique<double []>(histogramsCount * histogramsCount * anglesCount);
    int size = histogramsCount * histogramSize;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            double dx = sobelX.get(point.x + x - size/2, point.y + y - size/2, borderType);
            double dy = sobelY.get(point.x + x - size/2, point.y + y - size/2, borderType);
            int histogramNumber = x / histogramSize * histogramsCount + y / histogramSize;
            double angleNumber = (atan2(dy, dx) / M_PI + 1) * anglesCount / 2;
            double weight = sqrt(dx*dx + dy*dy) * gauss(x - size/2, y - size/2);
            double value = weight * ((int)angleNumber - angleNumber + 1);
            data[histogramNumber * anglesCount + angleNumber] += value;
            value = weight * (angleNumber - (int)angleNumber);
            data[(int)(histogramNumber * anglesCount + angleNumber + 1) % anglesCount] += value;
        }
    }
    return data;
}

void DescriptorsKit::normalize(Desc &desc) {
    double sum = 0;
    for (int i = 0; i < desc.size; ++i)
        sum += desc.data[i] * desc.data[i];
    sum = sqrt(sum);
    for (int i = 0; i < desc.size; ++i)
        desc.data[i] = desc.data[i] / sum;
}

vector<array<int, 4>> DescriptorsKit::findMatches(DescriptorsKit &other) {
    vector<array<int, 4>> result;
    auto distances = make_unique<double []>(descs.size() * other.descs.size());
    for (int i = 0; i < descs.size(); ++i) {
        for (int j = 0; j < other.descs.size(); ++j) {
            double distance = 0;
            for (int k = 0; k < descs[i].size; ++k)
                distance += (descs[i].data[k] - other.descs[j].data[k]) * (descs[i].data[k] - other.descs[j].data[k]);
            distances[i * other.descs.size() + j] =  sqrt(distance);
        }
    }

    for (int i = 0; i < descs.size(); ++i) {
        double firstBest, secondBest;
        if (distances[i * other.descs.size()] < distances[i * other.descs.size() + 1]) {
            firstBest = 0;
            secondBest = 1;
        } else {
            firstBest = 1;
            secondBest = 0;
        }
        for (int j = 2; j < other.descs.size(); ++j) {
            if (distances[i * other.descs.size() + secondBest] > distances[i * other.descs.size() + j]) {
                if (distances[i * other.descs.size() + firstBest] > distances[i * other.descs.size() + j]) {
                    secondBest = firstBest;
                    firstBest = j;
                } else {
                    secondBest = j;
                }
            }
        }
        if (abs(distances[i * other.descs.size() + firstBest] - distances[i * other.descs.size() + secondBest]) > MATCH_THRESHOLD)
            result.emplace_back(array<int, 4>{descs[i].x, descs[i].y, other.descs[firstBest].x, other.descs[firstBest].y});
    }
    return result;
}
