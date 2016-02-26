#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Convolution.h"
#include "BlackWhiteConverter.h"

using namespace std;

int main()
{
    size_t width, height;
    QImage qImage = QImage();
    qImage.load("C:\\AltSTU\\computer-vision\\avatar.jpg");
    width = (size_t) qImage.width();
    height = (size_t) qImage.height();
    qDebug() << "image width: " << width << " image height: " << height;

    auto image = BlackWhiteConverter().convert(qImage, width, height);
    auto image1 = Convolution(ConvolutionType::Test).calculate(image, width, height);
    auto output = Convolution(ConvolutionType::SobelY).calculate(image1, width, height);

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int color = (int)(output[(j * width) + i] * 255.);
            qImage.setPixel(i, j, qRgb(color, color, color));
        }
    }
    qImage.save("C:\\AltSTU\\computer-vision\\output.png", "PNG");
    return 0;
}