#include <iostream>
#include <qDebug>
#include <QtGui>
#include "Image.h"
#include "Pyramid.h"
#include "InterestingPoints.h"
#include "Descriptor.h"

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
    auto image = input("avatar.jpg");
    auto searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Moravek, BorderType::Mirror);
    searcher->output("moravek.png");
    searcher = make_unique<InterestingPointsSearcher>(*image, InterestingPointsMethod::Harris, BorderType::Mirror);
    searcher->adaptiveNonMaximumSuppression(50);
    searcher->output("harris.png");
}

void lab4() {
    vector<Descriptor> firstDescs, secondDescs;

    auto first = input("first.jpg");
    auto sobelX = first->convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    auto sobelY = first->convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    auto searcher = make_unique<InterestingPointsSearcher>(*first, InterestingPointsMethod::Harris, BorderType::Mirror);
    searcher->adaptiveNonMaximumSuppression(500);
    for (auto &point : searcher->getPoints())
        firstDescs.emplace_back(Descriptor(*sobelX, *sobelY, point, BorderType::Mirror));

    auto second = input("second.jpg");
    sobelX = second->convolution(*KernelFactory::buildSobelX(), BorderType::Mirror);
    sobelY = second->convolution(*KernelFactory::buildSobelY(), BorderType::Mirror);
    searcher = make_unique<InterestingPointsSearcher>(*second, InterestingPointsMethod::Harris, BorderType::Mirror);
    searcher->adaptiveNonMaximumSuppression(500);
    for (auto &point : searcher->getPoints())
        secondDescs.emplace_back(Descriptor(*sobelX, *sobelY, point, BorderType::Mirror));

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

    auto distances = make_unique<double []>(firstDescs.size() * secondDescs.size());
    for (int i = 0; i < firstDescs.size(); ++i) {
        for (int j = 0; j < secondDescs.size(); ++j) {
            distances[i * secondDescs.size() + j] = firstDescs[i].calculateDistance(secondDescs[j]);
        }
    }

    QPainter painter(&qImage);
    for (int i = 0; i < firstDescs.size(); ++i) {
        double firstBest, secondBest;
        if (distances[i * secondDescs.size()] < distances[i * secondDescs.size() + 1]) {
            firstBest = distances[i * secondDescs.size()];
            secondBest = distances[i * secondDescs.size() + 1];
        } else {
            firstBest = distances[i * secondDescs.size() + 1];
            secondBest = distances[i * secondDescs.size()];
        }
        for (int j = 2; j < secondDescs.size(); ++j) {
            if (distances[i * secondDescs.size() + secondBest] > distances[i * secondDescs.size() + j]) {
                if (distances[i * secondDescs.size() + firstBest] > distances[i * secondDescs.size() + j]) {
                    secondBest = firstBest;
                    firstBest = j;
                } else {
                    secondBest = j;
                }
            }
        }
        if (abs(distances[i * secondDescs.size() + firstBest] - distances[i * secondDescs.size() + secondBest]) > 0.2) {
            painter.setPen(QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256));
            painter.drawLine(firstDescs[i].getX(),
                             firstDescs[i].getY(),
                             secondDescs[firstBest].getX() + first->getWidth(),
                             secondDescs[firstBest].getY());
        }
    }

    qImage.save("C:\\AltSTU\\computer-vision\\lab4.png", "png");
}
