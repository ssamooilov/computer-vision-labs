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
    int anglesDataSize = BIG_HISTOGRAMS_COUNT * BIG_HISTOGRAMS_COUNT * BIG_ANGLES_COUNT;
    for (auto &point : searcher->getPoints()) {
        auto anglesData = calculateData(*sobelX, *sobelY, point, 0, BIG_HISTOGRAMS_COUNT, HISTOGRAM_SIZE, BIG_ANGLES_COUNT);

        int firstBest, secondBest;
        if (anglesData[0] > anglesData[1]) {
            firstBest = 0;
            secondBest = 1;
        } else {
            firstBest = 1;
            secondBest = 0;
        }
        for (int i = 0; i < anglesDataSize; ++i)
            if (anglesData[secondBest] < anglesData[i])
                if (anglesData[firstBest] < anglesData[i]) {
                    secondBest = firstBest;
                    firstBest = i;
                } else secondBest = i;

        descs.emplace_back(move(buildDesc(sobelX, sobelY, point, anglesData, firstBest)));
        if (anglesData[firstBest] * 0.8 < anglesData[secondBest])
            descs.emplace_back(move(buildDesc(sobelX, sobelY, point, anglesData, secondBest)));
    }
}

Desc DescriptorsKit::buildDesc(const unique_ptr<Image> &sobelX, const unique_ptr<Image> &sobelY,
                               const InterestingPoint &point, const unique_ptr<double[]> &anglesData, int indexOfMax) {
    int anglesDataSize = BIG_HISTOGRAMS_COUNT * BIG_HISTOGRAMS_COUNT * BIG_ANGLES_COUNT;
    double y1 = anglesData[(indexOfMax - 1 + anglesDataSize) % anglesDataSize];
    double y2 = anglesData[indexOfMax];
    double y3 = anglesData[(indexOfMax + 1) % anglesDataSize];
    double a = (y1 - 2 * y2 + y3) / 2;
    double b = (y3 - y2) - a * (2*indexOfMax + 1);

    double angle = -b / a / anglesDataSize * M_PI;
    auto desc = Desc();
    desc.x = point.x;
    desc.y = point.y;
    desc.size = HISTOGRAMS_COUNT * HISTOGRAMS_COUNT * ANGLES_COUNT;
    desc.data = calculateData(*sobelX, *sobelY, point, angle, HISTOGRAMS_COUNT, HISTOGRAM_SIZE, ANGLES_COUNT);
    normalize(desc);
    for (int i = 0; i < desc.size; ++i)
                desc.data[i] = min(NORMALIZE_THRESHOLD, desc.data[i]);
    normalize(desc);
    return desc;
}

unique_ptr<double[]> DescriptorsKit::calculateData(const Image &sobelX, const Image &sobelY,
                                                   const InterestingPoint &point, double rotateAngle,
                                                   int histogramsCount, int histogramSize, int anglesCount) {
    auto data = make_unique<double []>(histogramsCount * histogramsCount * anglesCount);
    int size = histogramsCount * histogramSize;
    double angleCos = cos(rotateAngle);
    double angleSin = sin(rotateAngle);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int rotatedX = (int)((x - size/2) * angleCos - (y - size/2) * angleSin);
            int rotatedY = (int)((x - size/2) * angleSin + (y - size/2) * angleCos);
            double dx = sobelX.get(point.x + rotatedX, point.y + rotatedY, borderType);
            double dy = sobelY.get(point.x + rotatedX, point.y + rotatedY, borderType);
            int histogramNumber = x / histogramSize * histogramsCount + y / histogramSize;

            double angle = atan2(dy, dx) + M_PI - rotateAngle - 0.001;
            if (angle < 0) angle += 2 * M_PI;
            if (angle > 2 * M_PI) angle -= 2 * M_PI;
            double angleNumber = angle / M_PI / 2 * anglesCount;
            Q_ASSERT((int)angleNumber >= 0 && (int)angleNumber < anglesCount);
            double weight = sqrt(dx*dx + dy*dy) * gauss(x - size/2, y - size/2);
            double value = weight * ((int)angleNumber - angleNumber + 1);
            data[histogramNumber * anglesCount + (int)angleNumber] += value;
            value = weight * (angleNumber - (int)angleNumber);
            data[(histogramNumber * anglesCount + (int)angleNumber + 1) % anglesCount] += value;
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
