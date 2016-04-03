#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"

using namespace std;

QString pathPrefix = "C:\\AltSTU\\computer-vision\\";

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

//    auto sobelX = image->convolution(*KernelFactory::buildSobelX(), NormingType::Mirror);
//    auto sobelY = image->convolution(*KernelFactory::buildSobelY(), NormingType::Mirror);
//    image = sobelX->calculateHypotenuse(*sobelY);
//    image = image->normalize();

    Pyramid pyramid = Pyramid(*image);
    for (int i = 0; i < pyramid.layers.size(); ++i) {
        qDebug() << "layer: " << i << " sigma: " << pyramid.sigmas[i];
        output(pyramid.layers[i], QString::number(i) + QString("_layer.png"));
    }
    return 0;
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
