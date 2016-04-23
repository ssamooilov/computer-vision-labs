#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"

using namespace std;

QString pathPrefix = "C:\\AltSTU\\computer-vision\\";

void lab1(const Image &image);
void lab2(const Image &image);
void output(const Image &image, QString fileName);

int main()
{
    QImage qImage = QImage();
    qImage.load(pathPrefix + "avatar.jpg");

    auto image = make_unique<Image>(qImage.width(), qImage.height());
    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            image->set(j, i, qRed(qImage.pixel(j, i)), qGreen(qImage.pixel(j, i)), qBlue(qImage.pixel(j, i)));
        }
    }

    lab1(*image);
    lab2(*image);
    return 0;
}

void lab1(const Image &image) {
    auto sobelX = image.convolution(*KernelFactory::buildSobelX(), NormingType::Mirror);
    auto sobelY = image.convolution(*KernelFactory::buildSobelY(), NormingType::Mirror);
    auto result = sobelX->calculateHypotenuse(*sobelY);
    result = result->normalize();
    output(*result, "lab1.png");
}

void lab2(const Image &image) {
    Pyramid pyramid = Pyramid(image, 5);
    for (int i = 0; i < pyramid.octaves.size(); ++i) {
        for (int j = 0; j < pyramid.octaves[i].size(); ++j) {
            qDebug() << "octave: " << i << "layer: " << j << " global_sigma: "
                     << pyramid.octaves[i][j].global_sigma << " local_sigma: " << pyramid.octaves[i][j].local_sigma;
            output(pyramid.octaves[i][j].image, QString::number(i) + "_" + QString::number(j) + QString(".png"));
        }
    }
}

void output(const Image &image, QString fileName) {
    QImage qImage = QImage(image.getWidth(), image.getHeight(), QImage::Format_RGB32);
    for (int i = 0; i < image.getHeight(); ++i) {
        for (int j = 0; j < image.getWidth(); ++j) {
            int color = (int) (image.get(j, i) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    qImage.save(pathPrefix + fileName, "png");
}
