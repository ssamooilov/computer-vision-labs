#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Convolution.h"
#include "Image.h"

using namespace std;

int main()
{
    QImage qImage = QImage();
    qImage.load("C:\\AltSTU\\computer-vision\\avatar.jpg");

    unique_ptr<Image> image = make_unique<Image>(qImage.width(), qImage.height());
    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            image->set(j, i, qRed(qImage.pixel(j, i)), qGreen(qImage.pixel(j, i)), qBlue(qImage.pixel(j, i)));
        }
    }

    image = Convolution(ConvolutionType::Test, NormingType::Mirror).calculate(image.get());
    image = Convolution(ConvolutionType::SobelX, NormingType::Mirror).calculate(image.get());

    for (int i = 0; i < image->getHeight(); ++i) {
        for (int j = 0; j < image->getWidth(); ++j) {
            int color = (int) (min(1., max(0., image->get(j, i))) * 255.);
            qImage.setPixel(j, i, qRgb(color, color, color));
        }
    }
    qImage.save("C:\\AltSTU\\computer-vision\\output.png", "PNG");
    return 0;
}