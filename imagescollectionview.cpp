#include <QGridLayout>
#include <QDir>
#include <QMouseEvent>
#include <QImage>
#include <iostream>

using namespace std;

#include "imagescollectionview.h"

ImagesCollectionView::ImagesCollectionView(QWidget *parent) :
    QWidget(parent)
{
    count = 0;
    selectedIndexofImage = -1;
    pageGrid = new QGridLayout;
    setLayout(pageGrid);
}

void ImagesCollectionView::readDirectoryFiles()
{
    QDir dir(basePath);
    if (!dir.exists())
        return;

    imagesCollection = dir.entryList().filter("jpg");

    cout << "CBIR LOG ImagesCollectionView: files count " << imagesCollection.size() << endl;
    count = 0;
    createImagePage();
}

void ImagesCollectionView::createImagePage()
{
    int size = imagesCollection.size();

    for (int i = 0; i < 3 && count < size; i++) {
        for (int j = 0; j < 3 && count < size; j++) {
            images[i + j * 3].setPixmap(basePath + "/" + imagesCollection[count]);
            count ++;
            pageGrid->addWidget(&images[i + j * 3], i, j, 1, 1);
        }
    }
    pageGrid->setSpacing(20);
}

void ImagesCollectionView::setBasePath(QString path)
{
    basePath = path;
}

void ImagesCollectionView::printQStringToConsole(QString log)
{
    QByteArray out(log.toLatin1());
    cout << "CBIR LOG: " << out.data() << endl;
}

void ImagesCollectionView::tonextPage()
{
    int size = imagesCollection.size();

    if (count < size) {
        createImagePage();
    }
}

void ImagesCollectionView::topreviousPage()
{
    if ((count - 18) >= 0 ) {
        count -= 18;
        createImagePage();
    }
}

void ImagesCollectionView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        for (int i = 0; i < 9; i++) {
            QRect rect = images[i].frameGeometry();
            QRect p = QRect(event->pos().x(),event->pos().y(),1,1);

            if (rect.intersects(p)) {
                images[i].setBeingSelected(true);
                selectedIndexofImage = count - 9 + i;
                cout << "press the left mouse button: select the button, index:"
                        << selectedIndexofImage << endl;
            }
            else
                images[i].setBeingSelected(false);
        }
    } else if (event->button() == Qt::RightButton) {
        for (int i = 0; i < 9; i++) {
            QRect rect = images[i].frameGeometry();
            QRect p = QRect(event->pos().x(),event->pos().y(),1,1);

            if (rect.intersects(p) && images[i].getBeingSelected()) {
                images[i].setBeingSelected(false);
                selectedIndexofImage = -1;
                cout << "press the right mouse button: cancel the selected, index:"
                     << selectedIndexofImage << endl;
            }
        }
    }
}

QStringList ImagesCollectionView::findSimilarities(QString fileName)
{
    QImage *selectedImage = new QImage;
    selectedImage->load(basePath + "/" + fileName);

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
        {255, 255, 255}}; // color pattern to be mapped
    int colorTest[96];
    memset(colorTest, 0, sizeof(int)* 96);

    uchar* imagePixels = selectedImage->bits();

    delete selectedImage;
}
