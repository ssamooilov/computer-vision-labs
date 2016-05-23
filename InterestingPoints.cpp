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
    extractInterestingPoints(contrast, HARRIS_THRESHOLD, borderType);
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
            interestingPoint.x = x;
            interestingPoint.y = y;
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
            painter.drawEllipse(point.x, point.y, r, r);
        } else painter.drawEllipse(point.x, point.y, 3, 3);
    }

    qImage.save("C:\\AltSTU\\computer-vision\\" + fileName, "jpg");
}

void InterestingPointsSearcher::adaptiveNonMaximumSuppression(const int countPoints) {
    for (int r = 0; r < image.getWidth() + image.getHeight() && points.size() > countPoints; ++r) {
        for (int i = 0; i < points.size(); ++i) {
            for (int j = i+1; j < points.size(); ++j) {
                if (sqrt((points[i].x - points[j].x) * (points[i].x - points[j].x)
                         + (points[i].y - points[j].y) * (points[i].y - points[j].y)) <= r
                    && FILTER_FACTOR * points[i].weight > points[j].weight) {
                    points.erase(points.begin() + j);
                    j--;
                    break;
                }
            }
        }
    }
}

const vector<InterestingPoint> &InterestingPointsSearcher::getPoints() const {
    return points;
}

bool InterestingPointsSearcher::checkOnePoint(Pyramid &pyramid, BorderType borderType, int pointX, int pointY,
                                              int octaveIndex, int layerIndex, double centerValue) const {
    bool isMax = true, isMin = true;
    for (int dz = -1; dz < 1; ++dz) {
//        for (int dy = -1; dy < 1; ++dy) {
//            for (int dx = -1; dx < 1; ++dx) {
                if (dz == 0) continue;
//                if (dx == 0 && dy == 0 && dz == 0) continue;
                double curValue = pyramid.diffs[octaveIndex][layerIndex + dz].image.get(
                        pointX, pointY, octaveIndex, borderType);
                if (curValue > centerValue) isMax = false;
                if (curValue < centerValue) isMin = false;
                if (!isMax && !isMin) return false;
//            }
//        }
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
                                point.x, point.y, octaveIndex, borderType);
                        if (checkOnePoint(*pyramid, borderType, point.x, point.y, octaveIndex, layerIndex-2, centerValue)) {
                            point.x *= round(pow(2, octaveIndex));
                            point.y *= round(pow(2, octaveIndex));
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
                if (point.x == points[i].x+dx && point.y == points[i].y+dy)
                    return true;
            }
        }
    return false;
}
