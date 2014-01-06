#include <QDir>
#include <QImage>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <iostream>

#include "imageproccesser.h"

using namespace std;

ImageProccesser::ImageProccesser()
{
}

void ImageProccesser::setDirectory(QString path)
{
    basePath = path;
    readDirectoryFiles();
    cout << "CBIR LOG ImageProccessor: set the base directory" << endl;
}

void ImageProccesser::readDirectoryFiles()
{
    QDir dir(basePath);
    if (!dir.exists())
        return;
    imagesCollection = dir.entryList().filter("jpg");
    cout << "CBIR LOG ImageProcessor: read the file list of the directory" << endl;
}

void ImageProccesser::proccessCreateTxtInfoForImages()
{
    int Pattern[24][3] = {{0, 0, 0},
        {0, 182, 0},
        {0, 255, 170},
        {36, 73, 0},
        {36, 146, 170},
        {36, 255, 0},
        {73, 36, 170},
        {73, 146, 0},
        {73, 219, 170},
        {109, 36, 0},
        {109, 109, 170},
        {109, 219, 0},
        {146, 0, 170},
        {146, 182, 170},
        {182, 0, 0},
        {182, 73, 170},
        {182, 182, 0},
        {182, 255, 170},
        {219, 73, 0},
        {219, 146, 170},
        {219, 255, 0},
        {255, 36, 170},
        {255, 146, 0},
        {255, 255, 255}}; // color patter to be mapped

    int imagesCount = imagesCollection.size();

    int colorLookUp[96];
    memset(colorLookUp, 0, sizeof(int)* 96);

    FILE* file = fopen("/Users/Johnson/Desktop/Histogram.txt","wt+");

    cout << "CBIR LOG ImageProccessor: open histogram.txt successfully" << endl;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for (int i = 0; i < imagesCount; i++) {
        QImage *image = new QImage;
        image->load(basePath + "/" + imagesCollection[i]);

        int width = image->width();
        int height = image->height();
        int regionSize = width * height / 4;
        int R,G,B, min, index;

        for (int y = 0; y < height/2; y++) {
            for (int x = 0; x < width/2; x++) {
                QRgb pixels = image->pixel(x, y);
                QColor color = QColor(pixels);
                R = color.red(); G = color.green(); B = color.blue();

                min = abs(B - Pattern[0][2]) +
                        abs(G - Pattern[0][1]) +
                        abs(R - Pattern[0][0]);
                index = 0;
                for (int k = 1; k < 24; k++) {
                    int temp = abs(B - Pattern[k][2]) +
                            abs(G - Pattern[k][1]) +
                            abs(R - Pattern[k][0]);
                    if (temp < min) {
                        min = temp;
                        index = k;
                    }
                }
                colorLookUp[index]++;
            }
        }

        for (int y = height/2+1; y < height; y++) {
            for (int x = 0; x < width/2; x++) {
                QRgb pixels = image->pixel(x, y);
                QColor color = QColor(pixels);
                R = color.red(); G = color.green(); B = color.blue();
                min = abs(B - Pattern[0][2]) +
                        abs(G - Pattern[0][1]) +
                        abs(R - Pattern[0][0]);
                index = 0;
                for (int k = 1; k < 24; k++) {
                    int temp = abs(B - Pattern[k][2]) +
                            abs(G - Pattern[k][1]) +
                            abs(R - Pattern[k][0]);
                    if (temp < min) {
                        min = temp;
                        index = k;
                    }
                }
                colorLookUp[index + 24]++;
            }
        }

        for (int y = 0; y < height/2; y++) {
            for (int x = width/2+1; x < width; x++) {
                QRgb pixels = image->pixel(x, y);
                QColor color = QColor(pixels);
                R = color.red(); G = color.green(); B = color.blue();
                min = abs(B - Pattern[0][2]) +
                        abs(G - Pattern[0][1]) +
                        abs(R - Pattern[0][0]);
                index = 0;
                for (int k = 1; k < 24; k++) {
                    int temp = abs(B - Pattern[k][2]) +
                            abs(G - Pattern[k][1]) +
                            abs(R - Pattern[k][0]);
                    if (temp < min) {
                        min = temp;
                        index = k;
                    }
                }
                colorLookUp[index + 48]++;
            }
        }

        for (int y = height/2+1; y < height; y++) {
            for (int x = width/2+1; x < width; x++) {
                QRgb pixels = image->pixel(x, y);
                QColor color = QColor(pixels);
                R = color.red(); G = color.green(); B = color.blue();
                min = abs(B - Pattern[0][2]) +
                        abs(G - Pattern[0][1]) +
                        abs(R - Pattern[0][0]);
                index = 0;
                for (int k = 1; k < 24; k++) {
                    int temp = abs(B - Pattern[k][2]) +
                            abs(G - Pattern[k][1]) +
                            abs(R - Pattern[k][0]);
                    if (temp < min) {
                        min = temp;
                        index = k;
                    }
                }
                colorLookUp[index + 72]++;
            }
        }
        QString filenameline = QString("%1\n").arg(imagesCollection[i]);

        const char* pic = qPrintable(filenameline);
        fwrite(pic, strlen(pic), 1, file);

        for (int k = 0; k < 96; k++) {
            char* pic = new char[100];
            sprintf(pic, "%lf\n", (double)colorLookUp[k] / regionSize);
            fwrite(pic, strlen(pic), 1, file);
            delete pic;
        }
        memset(colorLookUp, 0, sizeof(int)*96);
        delete image;
    }
    fclose(file);
    cout << "CBIR LOG ImageProccessor: create the histograms successfully" << endl;
    QApplication::restoreOverrideCursor();
}
