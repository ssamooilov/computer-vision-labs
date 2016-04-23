//
// Created by Samoilov Sergei on 23.04.2016.
//

#include "InterestingPoints.h"


InterestingPointsSearcher::InterestingPointsSearcher(const Image &image, InterestingPointsMethod method, BorderType borderType)
        : image(image) {
    switch (method) {
        case InterestingPointsMethod::Moravek:
            moravek(borderType);
            break;
        case InterestingPointsMethod::Harris:
            harris();
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
    for (int y = 0; y < contrast.getHeight(); ++y) {
        for (int x = 0; x < contrast.getWidth(); ++x) {
            if (contrast.get(x, y) < THRESHOLD) continue;
            for (int dy = -LOCAL_SHIFT; dy < LOCAL_SHIFT; ++dy) {
                for (int dx = -LOCAL_SHIFT; dx < LOCAL_SHIFT; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    if (contrast.get(x+dx, y+dy, borderType) >= contrast.get(x, y)) goto end;
                }
            }
            points.emplace_back(InterestingPoint(x, y, contrast.get(x, y)));
            end:;
        }
    }
    qDebug() << "count: " << points.size();
}

void InterestingPointsSearcher::harris() {
    auto sobelX = image.convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = image.convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);

}

void InterestingPointsSearcher::output(QString fileName) const {
    QImage qImage = QImage(image.getWidth(), image.getHeight(), QImage::Format_RGB32);
    for (int i = 0; i < image.getHeight(); ++i) {
        for (int j = 0; j < image.getWidth(); ++j) {
            int color = (int) (image.get(j, i) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    for (auto &point : points) {
        for (int dy = -POINT_SHIFT; dy < POINT_SHIFT; ++dy) {
            for (int dx = -POINT_SHIFT; dx < POINT_SHIFT; ++dx) {
                qImage.setPixel(point.x + dx, point.y + dy, qRgb(255, 0, 0));
            }
        }
    }
    qImage.save("C:\\AltSTU\\computer-vision\\" + fileName, "png");
}
