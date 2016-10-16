#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"
#include "InterestingPoints.h"
#include "DescriptorsKit.h"
#include "Transform.h"

using namespace std;

unique_ptr<Image> input(QString filename);
void lab1();
void lab2();
void lab3();
void lab4();
void lab8();

int main()
{
//    lab1();
//    lab2();
//    lab3();
//    lab4();
    lab8();
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
    result->output("lab1.jpg");
}

void lab2() {
    auto image = input("avatar.jpg");
    auto pyramid = Pyramid(*image, 5);
    pyramid.calculateDiffs();
    for (int i = 0; i < pyramid.diffs.size(); ++i) {
        for (int j = 0; j < pyramid.diffs[i].size(); ++j) {
            qDebug() << "octave: " << i << "layer: " << j << " global_sigma: "
                     << pyramid.diffs[i][j].global_sigma << " local_sigma: " << pyramid.diffs[i][j].local_sigma;
            pyramid.diffs[i][j].image.normalize()->output(QString::number(i) + "_" + QString::number(j) + QString(".jpg"));
        }
    }
}

void lab3() {
    auto image = input("second2x.jpg");
//    auto searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Moravek, BorderType::Mirror);
//    searcher->output("moravek.jpg");
//    searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Harris, BorderType::Border);
//    searcher->adaptiveNonMaximumSuppression(500);
//    searcher->output("harris.jpg");
//    auto searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Blob, BorderType::Border);
//    searcher->adaptiveNonMaximumSuppression(500);
    auto searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Blob2, BorderType::Border);
    searcher->output("blob.jpg", true);
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

    qImage.save("C:\\AltSTU\\computer-vision\\lab4.jpg", "jpg");
}

void lab8() {
    auto first = input("fourth_right.jpg"), second = input("fourth_lef135.jpg");
    int width = max(first->getWidth(), second->getWidth());
    int height = max(first->getHeight(), second->getHeight());

    DescriptorsKit firstKit(*first, BorderType::Border), secondKit(*second, BorderType::Border);
    auto trans = Transform().ransac(firstKit.findMatches(secondKit));
    QImage qImage = QImage(width * 3, height * 3, QImage::Format_RGB32);

    for (int y = 0; y < second->getHeight(); ++y) {
        for (int x = 0; x < second->getWidth(); ++x) {
            int color = (int) (second->get(x, y) * 255.);
            qImage.setPixel(x + width, y + height, qRgb(color, color, color));
        }
    }

    for (int y = 0; y < first->getHeight(); ++y) {
        for (int x = 0; x < first->getWidth(); ++x) {
            int color = (int) (first->get(x, y) * 255.);
            double k = x * trans[6] + y * trans[7] + trans[8];
            int transX = int(round((x * trans[0] + y * trans[1] + trans[2]) / k));
            int transY = int(round((x * trans[3] + y * trans[4] + trans[5]) / k));
            qImage.setPixel(transX + width, transY + height, qRgb(color, color, color));
        }
    }
    qImage.save("C:\\AltSTU\\computer-vision\\lab8.jpg", "jpg");
}
