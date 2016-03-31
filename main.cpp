#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"

using namespace std;

QString pathPrefix = "C:\\AltSTU\\computer-vision\\";

void output(const Image &image, QString fileName);
void build_pyramids(const Image &input);

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
//    output(*image, QString("output.png"));

    build_pyramids(*image);
    return 0;
}

void build_pyramids(const Image &input) {
    const int min_size = 32;
    const double begin_sigma = 0.5, zero_sigma = 1.6;

    auto image = input.convolution(*KernelFactory::buildGaussX(zero_sigma), NormingType::Mirror);
    image = image->convolution(*KernelFactory::buildGaussY(zero_sigma), NormingType::Mirror);

    int count_layers = 0, size = min(input.getWidth(), input.getHeight());
    for (; size > min_size; count_layers++)
        size /= 2;

    double sigma = begin_sigma;
    for (int i = 0; i < count_layers; ++i) {
        image = image->convolution(*KernelFactory::buildGaussX(sigma / 2), NormingType::Mirror);
        image = image->convolution(*KernelFactory::buildGaussY(sigma / 2), NormingType::Mirror);
        image = image->normalize();
        image = image->scale();
        output(*image, QString::number(i) + QString("_layer1.png"));

        auto out = image->convolution(*KernelFactory::buildGaussX(sigma), NormingType::Mirror);
        out = out->convolution(*KernelFactory::buildGaussY(sigma), NormingType::Mirror);
        out = out->normalize();
        out = image->scale();
        for (int j = 0; j < i; ++j)
            out = out->scale();
        output(*out, QString::number(i) + QString("_layer2.png"));

        qDebug() << "layer: " << i << " sigma: " << sigma;
        sigma *= 2;
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