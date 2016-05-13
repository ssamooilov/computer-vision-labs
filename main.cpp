#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"
#include "InterestingPoints.h"
#include "DescriptorsKit.h"

using namespace std;

unique_ptr<Image> input(QString filename);
void lab1();
void lab2();
void lab3();
void lab4();

int main()
{
//    lab1();
//    lab2();
//    lab3();
    lab4();
    return 0;
}

unique_ptr<Image> input(QString filename) {
    QImage qImage = QImage();
    qImage.load("C:\\AltSTU\\computer-vision\\" + filename);

    auto image = make_unique<Image>(qImage.width(), qImage.height());
    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            image->set(j, i, qRed(qImage.pixel(j, i)), qGreen(qImage.pixel(j, i)), qBlue(qImage.pixel(j, i)));
        }
    }
    return move(image);
}

void lab1() {
    auto image = input("avatar.jpg");
    auto sobelX = image->convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = image->convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    auto result = sobelX->calculateHypotenuse(*sobelY);
    result = result->normalize();
    result->output("lab1.png");
}

void lab2() {
    auto image = input("avatar.jpg");
    auto pyramid = Pyramid(*image, 5);
    for (int i = 0; i < pyramid.octaves.size(); ++i) {
        for (int j = 0; j < pyramid.octaves[i].size(); ++j) {
            qDebug() << "octave: " << i << "layer: " << j << " global_sigma: "
                     << pyramid.octaves[i][j].global_sigma << " local_sigma: " << pyramid.octaves[i][j].local_sigma;
            pyramid.octaves[i][j].image.output(QString::number(i) + "_" + QString::number(j) + QString(".png"));
        }
    }
}

void lab3() {
    auto image = input("circles.jpg");
    auto searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Moravek, BorderType::Mirror);
    searcher->output("moravek.png");
    searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Harris, BorderType::Border);
    auto pyramid = Pyramid(*image, 5);
    pyramid.calculateDiffs();
    searcher->extractBlobs(pyramid, BorderType::Border);
    searcher->adaptiveNonMaximumSuppression(500);
    searcher->output("harris.png");
}

void lab4() {
    auto first = input("second.jpg"), second = input("second2x.jpg");
    QImage qImage = QImage(first->getWidth() + second->getWidth(),
                           max(first->getHeight(), second->getHeight()),
                           QImage::Format_RGB32);
    for (int y = 0; y < first->getHeight(); ++y) {
        for (int x = 0; x < first->getWidth(); ++x) {
            int color = (int) (first->get(x, y) * 255.);
            qImage.setPixel(x, y, qRgb(color, color, color));
        }
    }
    for (int y = 0; y < second->getHeight(); ++y) {
        for (int x = 0; x < second->getWidth(); ++x) {
            int color = (int) (second->get(x, y) * 255.);
            qImage.setPixel(x + first->getWidth(), y, qRgb(color, color, color));
        }
    }

    DescriptorsKit firstKit(*first, BorderType::Border), secondKit(*second, BorderType::Border);
    QPainter painter(&qImage);
    for (auto match : firstKit.findMatches(secondKit)) {
        painter.setPen(QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256));
        painter.drawLine(match[0], match[1], match[2] + first->getWidth(), match[3]);
    }

    qImage.save("C:\\AltSTU\\computer-vision\\lab4.png", "png");
}
