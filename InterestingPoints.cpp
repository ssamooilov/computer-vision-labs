//
// Created by Samoilov Sergei on 23.04.2016.
//

#include <queue>
#include "InterestingPoints.h"


InterestingPointsSearcher::InterestingPointsSearcher(const Image &image, InterestingPointsMethod method, BorderType borderType)
        : image(image) {
    switch (method) {
        case InterestingPointsMethod::Moravek:
            moravek(borderType);
            break;
        case InterestingPointsMethod::Harris:
            harris(borderType);
            break;
        case InterestingPointsMethod::Blob:
            blob(borderType);
            break;
        case InterestingPointsMethod::Blob2:
            blob2(borderType);
            break;
    }
}

void InterestingPointsSearcher::blob2(BorderType borderType) {
    vector<int> dx, dy, dz;
    for (int i = 0; i < 3 * 3 * 3; ++i) {
        int x = i % 3 - 1;
        int y = i / 3 % 3 - 1;
        int z = i / (3*3) % 3 - 1;
        if (x != 0 || y != 0 || z != 0) {
            dx.emplace_back(x);
            dy.emplace_back(y);
            dz.emplace_back(z);
        }
    }

    pyramid = make_unique<Pyramid>(image, 5);
    pyramid->calculateDiffs();
    for (int octaveIndex = 0; octaveIndex < pyramid->diffs.size(); ++octaveIndex) {
        for (int layerIndex = 1; layerIndex < pyramid->diffs[octaveIndex].size() - 1; ++layerIndex) {
            auto harris = getHarris(pyramid->diffs[octaveIndex][layerIndex].image, borderType);
            for (int y = 0; y < pyramid->diffs[octaveIndex][layerIndex].image.getHeight(); ++y) {
                for (int x = 0; x < pyramid->diffs[octaveIndex][layerIndex].image.getWidth(); ++x) {
                    bool isMax = true, isMin = true;
                    for (int i = 0; i < 3 * 3 * 3 - 1; ++i) {
                        if (pyramid->diffs[octaveIndex][layerIndex + dz[i]].image.get(x + dx[i], y + dy[i], borderType)
                                > pyramid->diffs[octaveIndex][layerIndex].image.get(x, y, borderType))
                            isMax = false;
                        if (pyramid->diffs[octaveIndex][layerIndex + dz[i]].image.get(x + dx[i], y + dy[i], borderType)
                                < pyramid->diffs[octaveIndex][layerIndex].image.get(x, y, borderType))
                            isMin = false;
                    }
                    if ((isMax || isMin) && harris.get(x, y, borderType) > 1e-3) {
                        auto point = InterestingPoint();
                        point.local_x = x;
                        point.local_y = y;
                        point.global_x = (x + 0.5) * pow(2, octaveIndex) - 0.5;
                        point.global_y = (y + 0.5) * pow(2, octaveIndex) - 0.5;
                        point.local_sigma = pyramid->diffs[octaveIndex][layerIndex].local_sigma;
                        point.global_sigma = pyramid->diffs[octaveIndex][layerIndex].global_sigma;
                        point.octave = octaveIndex;
                        point.layer = layerIndex;
                        points.emplace_back(point);
                    }
                }
            }
        }
    }
}

void InterestingPointsSearcher::moravek(BorderType borderType) {
    auto contrast = Image(image.getWidth(), image.getHeight());
    for (int y = 0; y < image.getHeight(); ++y) {
        for (int x = 0; x < image.getWidth(); ++x) {
            double minValue = INFINITY;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    double value = 0;
                    for (int v = -WINDOW_SHIFT; v <= WINDOW_SHIFT; ++v) {
                        for (int u = -WINDOW_SHIFT; u <= WINDOW_SHIFT; ++u) {
                            auto tmp = image.get(x+u, y+v, borderType) - image.get(x+u+dx, y+v+dy, borderType);
                            value += tmp * tmp;
                        }
                    }
                    minValue = min(minValue, value);
                }
            }
            contrast.set(x, y, minValue);
        }
    }
    extractInterestingPoints(contrast, MORAVEK_THRESHOLD, borderType);
}

void InterestingPointsSearcher::harris(BorderType borderType) {
    Image contrast = getHarris(image, borderType);
    extractInterestingPoints(contrast, HARRIS_THRESHOLD, borderType);
}

Image InterestingPointsSearcher::getHarris(const Image &image, const BorderType &borderType) const {
    auto contrast = Image(image.getWidth(), image.getHeight());
    auto sobelX = image.convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = image.convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    auto kernel = KernelFactory::buildGauss(WINDOW_SIGMA);
    int size = kernel->width / 2;
    for (int y = 0; y < image.getHeight(); ++y) {
        for (int x = 0; x < image.getWidth(); ++x) {
            double a = 0, b = 0, c = 0;
            for (int v = 0; v < kernel->height; ++v) {
                for (int u = 0; u < kernel->width; ++u) {
                    a += kernel->data[u*kernel->height + v]
                         * sobelX->get(x+u-size, y+v-size, borderType) * sobelX->get(x+u-size, y+v-size, borderType);
                    b += kernel->data[u*kernel->height + v]
                         * sobelX->get(x+u-size, y+v-size, borderType) * sobelY->get(x+u-size, y+v-size, borderType);
                    c += kernel->data[u*kernel->height + v]
                         * sobelY->get(x+u-size, y+v-size, borderType) * sobelY->get(x+u-size, y+v-size, borderType);
                }
            }
            double d = sqrt((a-c) * (a-c) + 4*b*b);
            double lambda1 = (a + c - d) / 2;
            double lambda2 = (a + c + d) / 2;
            contrast.set(x, y, min(abs(lambda1), abs(lambda2)));
        }
    }
    return contrast;
}

void InterestingPointsSearcher::extractInterestingPoints(Image &contrast, double threshold, BorderType borderType) {
    points.clear();
    for (int y = 0; y < contrast.getHeight(); ++y) {
        for (int x = 0; x < contrast.getWidth(); ++x) {
            if (contrast.get(x, y) < threshold) continue;
            bool next = false;
            for (int dy = -EXTRACT_SHIFT; dy < EXTRACT_SHIFT; ++dy) {
                for (int dx = -EXTRACT_SHIFT; dx < EXTRACT_SHIFT; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    if (contrast.get(x+dx, y+dy, borderType) >= contrast.get(x, y)) next = true;
                }
            }
            if (next) continue;
            auto interestingPoint = InterestingPoint();
            interestingPoint.global_x = x;
            interestingPoint.global_y = y;
            interestingPoint.weight = contrast.get(x, y);
            points.emplace_back(interestingPoint);
        }
    }
}

void InterestingPointsSearcher::output(QString fileName, bool withScale) const {
    QImage qImage = QImage(image.getWidth(), image.getHeight(), QImage::Format_RGB32);
    for (int i = 0; i < image.getHeight(); ++i) {
        for (int j = 0; j < image.getWidth(); ++j) {
            int color = (int) (image.get(j, i) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    QPainter painter(&qImage);
    for (auto &point : points) {
        painter.setPen(QColor(255, 0, 0));
        if (withScale) {
            int r = (int) round(point.global_sigma * sqrt(2));
            painter.drawEllipse(point.global_x - r, point.global_y - r, r * 2, r * 2);
            painter.drawLine(point.global_x,
                             point.global_y,
                             point.global_x + r * cos(point.orientation),
                             point.global_y + r * sin(point.orientation));
        } else painter.drawEllipse(point.global_x, point.global_y, 3, 3);
    }

    qImage.save("C:\\AltSTU\\computer-vision\\" + fileName, "jpg");
}

void InterestingPointsSearcher::adaptiveNonMaximumSuppression(const int countPoints) {
    for (int r = 0; r < image.getWidth() + image.getHeight() && points.size() > countPoints; ++r) {
        for (int i = 0; i < points.size(); ++i) {
            for (int j = i+1; j < points.size(); ++j) {
                if (sqrt((points[i].global_x - points[j].global_x) * (points[i].global_x - points[j].global_x)
                         + (points[i].global_y - points[j].global_y) * (points[i].global_y - points[j].global_y)) <= r
                    && FILTER_FACTOR * points[i].weight > points[j].weight) {
                    points.erase(points.begin() + j);
                    j--;
                    break;
                }
            }
        }
    }
}

vector<InterestingPoint> &InterestingPointsSearcher::getPoints() {
    return points;
}

bool InterestingPointsSearcher::checkOnePoint(Pyramid &pyramid, BorderType borderType, int pointX, int pointY,
                                              int octaveIndex, int layerIndex, double centerValue) const {
    for (int dz = -1; dz < 1; ++dz) {
        if (dz == 0) continue;
        double curValue = pyramid.diffs[octaveIndex][layerIndex + dz].image.get(
                pointX, pointY, octaveIndex, borderType);
        if (curValue > centerValue) return false;
    }
    return true;
}

void InterestingPointsSearcher::blob(BorderType borderType) {
    pyramid = make_unique<Pyramid>(image, 7);
    pyramid->calculateDiffs();
    list<unique_ptr<InterestingPointsSearcher>> searchers;
    for (int octaveIndex = 0; octaveIndex < pyramid->octaves.size(); ++octaveIndex) {
        for (int layerIndex = 0; layerIndex < pyramid->octaves[octaveIndex].size(); ++layerIndex) {
            qDebug() << "octaveIndex: " << octaveIndex << " layerIndex: " << layerIndex;
            auto curSearcher = make_unique<InterestingPointsSearcher>(
                    pyramid->octaves[octaveIndex][layerIndex].image, InterestingPointsMethod::Harris, borderType);
            curSearcher->output(QString::number(octaveIndex) + "_" + QString::number(layerIndex) + QString(".jpg"), false);
            if (layerIndex >= 3) {
                for (auto point : curSearcher->points) {
                    bool contains = true;
                    for (auto &searcher : searchers)
                        if (!searcher->contains(point))
                            contains = false;
                    if (contains) {
                        double centerValue = pyramid->diffs[octaveIndex][layerIndex-2].image.get(
                                point.global_x, point.global_y, octaveIndex, borderType);
                        if (checkOnePoint(*pyramid, borderType, point.global_x, point.global_y, octaveIndex, layerIndex-2, centerValue)) {
                            point.global_x *= round(pow(2, octaveIndex));
                            point.global_y *= round(pow(2, octaveIndex));
                            point.octave = octaveIndex;
                            point.layer = layerIndex-2;
                            point.local_sigma = pyramid->diffs[octaveIndex][layerIndex-2].local_sigma;
                            point.global_sigma = pyramid->diffs[octaveIndex][layerIndex-2].global_sigma;
                            points.emplace_back(point);
                        }
                    }
                }
            }
            searchers.emplace_back(move(curSearcher));
            if (searchers.size() > 3) searchers.pop_front();
        }
    }
}

bool InterestingPointsSearcher::contains(InterestingPoint &point) const {
    for (int i = 0; i < points.size(); ++i)
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (point.global_x == points[i].global_x+dx && point.global_y == points[i].global_y+dy)
                    return true;
            }
        }
    return false;
}
