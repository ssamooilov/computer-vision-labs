#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"
#include "InterestingPoints.h"

using namespace std;

void lab1(const Image &image);
void lab2(const Image &image);
void lab3(const Image &image);

int main()
{
    QImage qImage = QImage();
    qImage.load("C:\\AltSTU\\computer-vision\\avatar.jpg");

    auto image = make_unique<Image>(qImage.width(), qImage.height());
    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            image->set(j, i, qRed(qImage.pixel(j, i)), qGreen(qImage.pixel(j, i)), qBlue(qImage.pixel(j, i)));
        }
    }

//    lab1(*image);
//    lab2(*image);
    lab3(*image);
    return 0;
}

void lab1(const Image &image) {
    auto sobelX = image.convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = image.convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    auto result = sobelX->calculateHypotenuse(*sobelY);
    result = result->normalize();
    result->output("lab1.png");
}

void lab2(const Image &image) {
    auto pyramid = Pyramid(image, 5);
    for (int i = 0; i < pyramid.octaves.size(); ++i) {
        for (int j = 0; j < pyramid.octaves[i].size(); ++j) {
            qDebug() << "octave: " << i << "layer: " << j << " global_sigma: "
                     << pyramid.octaves[i][j].global_sigma << " local_sigma: " << pyramid.octaves[i][j].local_sigma;
            pyramid.octaves[i][j].image.output(QString::number(i) + "_" + QString::number(j) + QString(".png"));
        }
    }
}

void lab3(const Image &image) {
    auto searcher = make_unique<InterestingPointsSearcher>(image, InterestingPointsMethod::Moravek, BorderType::Mirror);
    searcher->output("moravek.png");
    searcher = make_unique<InterestingPointsSearcher>(image, InterestingPointsMethod::Harris, BorderType::Mirror);
    searcher->adaptiveNonMaximumSuppression(100);
    searcher->output("harris.png");
}
