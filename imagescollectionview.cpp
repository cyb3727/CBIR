#include <QGridLayout>
#include <QDir>
#include <QMouseEvent>
#include <QImage>
#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include <cmath>
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <stdio.h>
using namespace std;

#include "imagescollectionview.h"

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

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
            images[i*3 + j].setPixmap(basePath + "/" + imagesCollection[count]);
            count ++;
            pageGrid->addWidget(&images[i*3 + j], i, j, 1, 1);
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

void ImagesCollectionView::clearPage()
{
    count = 0;
    imagesCollection = QStringList();
    delete pageGrid;
    pageGrid = new QGridLayout;
    setLayout(pageGrid);
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

void ImagesCollectionView::searchSimilaritiesWithFileName(QString filename)
{
    if (imagesCollection.contains(filename)) {
        imagesCollection = findSimilarities(filename);
        count = 0;
        createImagePage();
    } else {
        cout << "CBIR LOG ImagesCollectionView: no this file -- " << qPrintable(filename) << endl;
    }
}

bool ImagesCollectionView::searchSimilaritiesWithSift(QWidget* parent)
{
    int siftCount = 30;
    int imagesCount = imagesCollection.size();
    double* matches = new double[siftCount];
    int* index = new int[siftCount];

    for (int i = 0; i < siftCount; i++)
        index[i] = i;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QProgressDialog progressDialog(parent);
    progressDialog.setLabelText(QString("Compare images"));
    progressDialog.setRange(0, siftCount);
    progressDialog.setModal(true);

    for (int i = 0; i < siftCount; i++) {
        progressDialog.setValue(i);

        if (progressDialog.wasCanceled()) {
            QApplication::restoreOverrideCursor();
            return false;
        }
        matches[i] = (double)sift((char*)qPrintable(basePath + "/" + imagesCollection[0]),
               (char*)qPrintable(basePath + "/" + imagesCollection[i]));
        cout << matches[i] << endl;
    }

    quick_sort(matches, index, siftCount);

    cout << "here_finish" << endl;
    QStringList afterSift;

    for (int i = siftCount - 1; i >= 0; i--)
        afterSift << imagesCollection[index[i]];

    for (int i = siftCount; i < imagesCount; i++)
        afterSift << imagesCollection[i];

    imagesCollection = afterSift;
    count = 0;
    createImagePage();

    QApplication::restoreOverrideCursor();

    return true;
}

void ImagesCollectionView::searchSimilarities()
{
    if (selectedIndexofImage < 0)
        return;

    QString selectedFileName = imagesCollection[selectedIndexofImage];

    imagesCollection = findSimilarities(selectedFileName);

    cout << "CBIR LOG ImagesCollectionView: selected file name "
            << qPrintable(selectedFileName) << endl;
    count = 0;
    createImagePage();

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
    int height = selectedImage->height();
    int width = selectedImage->width();
    int R,G,B,min,index;
    int regionSize = height * width / 4;

    memset(colorTest, 0, sizeof(int)* 96);


    for (int y = 0; y < height/2; y++) {
        for (int x = 0; x < width/2; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index]++;
        }
    }
    for (int y = height/2 + 1; y < height; y++) {
        for (int x = 0; x < width/2; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 24]++;
        }
    }
    for (int y = 0; y < height/2; y++) {
        for (int x = width/2+1; x < width; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 48]++;
        }
    }

    for (int y = height/2 + 1; y < height; y++) {
        for (int x = width/2 + 1; x < width; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 72]++;
        }
    }
    delete selectedImage;

    FILE *file = fopen("/Users/Johnson/Desktop/Histogram.txt","r");

    cout << "CBIR LOG ImagesCollectionView: find, open histogram.txt successfully" << endl;

    int imagesCount = imagesCollection.size();

    double mixedData[1005];
    int imageIndex[1005];

    char** titleCollection = new char*[imagesCount];
    for (int i = 0; i < imagesCount; i++)
        titleCollection[i] = new char[50];

    for (int i = 0; i < imagesCount; i++) imageIndex[i] = i;

    memset(mixedData, 0, sizeof(int) * imagesCount);

    for (int i = 0; i < imagesCount; i++) {
        double similarity = 0;
        double color;

        fscanf(file, "%s", titleCollection[i]);

        for (int j = 0; j < 96; j++) {
            fscanf(file, "%lf", &color);
            similarity += sqrt((double)colorTest[j] / regionSize * color);
        }
        mixedData[i] = similarity / 4;
    }
    fclose(file);

    quick_sort(mixedData, imageIndex, imagesCount);

    QStringList findResult;

    for (int i = imagesCount-1; i >= 0; i--) {
        findResult << titleCollection[imageIndex[i]];
    }

    FILE* output = fopen("/Users/Johnson/Desktop/Result.txt", "wr+");

    cout << "CBIR LOG ImagesCollectionView: find, open Result.txt successfully" << endl;

    for (int i = imagesCount - 1; i >= imagesCount; i--) {
        char* pic = new char[1000];
        sprintf(pic, "%s:%lf\n", titleCollection[imageIndex[i]], mixedData[i]);
        fwrite(pic, strlen(pic), 1, output);
        delete pic;
    }
    fclose(output);

    delete [] titleCollection;

    return findResult;
}

void ImagesCollectionView::calculateSearchAccuracy(QWidget* parent)
{

    QMessageBox::warning(this, tr("AP"),tr("%1").arg(calculateApValue()),
                                              QMessageBox::Yes | QMessageBox::Default | QMessageBox::Escape);
}

double ImagesCollectionView::calculateApValue()
{
    int size = imagesCollection.size();
    int* result = new int[size];

    for (int i = 0; i < size; i++) {
        QStringList s = imagesCollection[i].split(".");
        result[i] = s[0].toInt();
    }

    for (int i = 0; i < size; i++) result[i] = result[i] / 100;

    int count = 0;
    double sum = 0;
    int crieria = result[0];

    for (int i = 0; i < size; i++) {
        int r = 0;
        if (result[i] == crieria) {
            count++;
            r = 1;
        }
        sum += r * (double)count / (i+1);
    }

    double ap = sum / count;
    cout << "CBIR LOG AP:" << ap << endl;

    return ap;
}

void ImagesCollectionView::calculateTheGroupApValue() {
    QStringList group1;
    for (int i = 0; i < 100; i++) {
        group1 << QString("%1.jpg").arg(i);
    }

    double apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group1[i]);
        apsum += calculateApValue();
    }
    cout << "group1:" << apsum/100 << endl;

    QStringList group2;
    for (int i = 100; i < 200; i++) {
        group2 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group2[i]);
        apsum += calculateApValue();
    }
    cout << "group2:" << apsum/100 << endl;

    QStringList group3;
    for (int i = 200; i < 300; i++) {
        group3 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group3[i]);
        apsum += calculateApValue();
    }
    cout << "group3:" << apsum/100 << endl;

    QStringList group4;
    for (int i = 300; i < 400; i++) {
        group4 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group4[i]);
        apsum += calculateApValue();
    }
    cout << "group4:" << apsum/100 << endl;

    QStringList group5;
    for (int i = 400; i < 500; i++) {
        group5 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group5[i]);
        apsum += calculateApValue();
    }
    cout << "group5:" << apsum/100 << endl;

    QStringList group6;
    for (int i = 500; i < 600; i++) {
        group6 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group6[i]);
        apsum += calculateApValue();
    }
    cout << "group6:" << apsum/100 << endl;

    QStringList group7;
    for (int i = 600; i < 700; i++) {
        group7 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group7[i]);
        apsum += calculateApValue();
    }
    cout << "group7:" << apsum/100 << endl;

    QStringList group8;
    for (int i = 700; i < 800; i++) {
        group8 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group8[i]);
        apsum += calculateApValue();
    }
    cout << "group8:" << apsum/100 << endl;

    QStringList group9;
    for (int i = 800; i < 900; i++) {
        group9 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group9[i]);
        apsum += calculateApValue();
    }
    cout << "group9:" << apsum/100 << endl;

    QStringList group10;
    for (int i = 900; i < 1000; i++) {
        group10 << QString("%1.jpg").arg(i);
    }

    apsum = 0;
    for (int i = 0; i < 100; i++) {
        searchSimilaritiesWithFileName(group10[i]);
        apsum += calculateApValue();
    }
    cout << "group10:" << apsum/100 << endl;
}

int ImagesCollectionView::partition(double *data, int *index, int low,int high)
{
    double t = 0;
    int indext = 0;
    t = data[low];
    indext = index[low];
    while(low < high){
        while(low < high && data[high] >= t)
            high--;
        data[low] = data[high];
        index[low] = index[high];
        while(low < high && data[low] <= t)
            low++;
        data[high] = data[low];
        index[high] = index[low];
    }
    data[low] = t;
    index[low] = indext;
    return low;
}

void ImagesCollectionView::sort(double *data, int *index, int low, int high)
{
    if (low >= high)
        return;
    int pivotloc = 0;
    pivotloc = partition(data, index, low, high);
    sort(data, index, low, pivotloc - 1);
    sort(data, index, pivotloc + 1, high);
}

void ImagesCollectionView::quick_sort(double *data, int *index, int n)
{
    sort(data, index, 0, n - 1);
}

int ImagesCollectionView::sift(char *img1_file, char *img2_file)
{
    IplImage* img1, * img2, * stacked;
    struct feature* feat1, * feat2, * feat;
    struct feature** nbrs;
    struct kd_node* kd_root;
    CvPoint pt1, pt2;
    double d0, d1;
    int n1, n2, k, i, m = 0;

    img1 = cvLoadImage( img1_file, 1);
    if( ! img1 )
        fatal_error( "unable to load image from %s", img1_file );
    img2 = cvLoadImage( img2_file, 1);
    if( ! img2 )
        fatal_error( "unable to load image from %s", img2_file );

    cout << img1_file << endl;
    cout << img2_file << endl;
    stacked = stack_imgs( img1, img2 );


    fprintf( stderr, "Finding features in %s...\n", img1_file );
    n1 = sift_features( img1, &feat1 );
    fprintf( stderr, "Finding features in %s...\n", img2_file );
    n2 = sift_features( img2, &feat2 );


    kd_root = kdtree_build( feat2, n2 );
    for( i = 0; i < n1; i++ )
    {
        feat = feat1 + i;
        k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
        if( k == 2 )
        {
            d0 = descr_dist_sq( feat, nbrs[0] );
            d1 = descr_dist_sq( feat, nbrs[1] );
            if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
            {
                pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
                pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
                pt2.y += img1->height;
                cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
                m++;
                feat1[i].fwd_match = nbrs[0];
            }
        }
        free( nbrs );
    }

    cvReleaseImage( &stacked );
    cvReleaseImage( &img1 );
    cvReleaseImage( &img2 );
    kdtree_release( kd_root );
    free( feat1 );
    free( feat2 );
    return m;
}

void ImagesCollectionView::allResult()
{
    FILE* output = fopen("/Users/Johnson/Desktop/allResult.txt", "wr+");

    for (int i = 0; i < 1000; i++) {
        QString selected = QString("%1.jpg").arg(i);
        QStringList result = find(selected);
        int count = result.size();
        for (int j = 0; j < count; j++) {
            const char* toprint = qPrintable(result[j].split(".")[0].append(" "));
            fwrite(toprint, strlen(toprint), 1, output);
        }
        fwrite("\n", strlen("\n"), 1, output);
    }
    fclose(output);
}

QStringList ImagesCollectionView::find(QString fileName)
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
    int height = selectedImage->height();
    int width = selectedImage->width();
    int R,G,B,min,index;
    int regionSize = height * width / 4;

    memset(colorTest, 0, sizeof(int)* 96);


    for (int y = 0; y < height/2; y++) {
        for (int x = 0; x < width/2; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index]++;
        }
    }
    for (int y = height/2 + 1; y < height; y++) {
        for (int x = 0; x < width/2; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 24]++;
        }
    }
    for (int y = 0; y < height/2; y++) {
        for (int x = width/2+1; x < width; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 48]++;
        }
    }

    for (int y = height/2 + 1; y < height; y++) {
        for (int x = width/2 + 1; x < width; x++) {
            QRgb pixels = selectedImage->pixel(x, y);
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
            colorTest[index + 72]++;
        }
    }
    delete selectedImage;

    FILE *file = fopen("/Users/Johnson/Desktop/Histogram.txt","r");

    cout << "CBIR LOG ImagesCollectionView: find, open histogram.txt successfully" << endl;

    int imagesCount = imagesCollection.size();

    double mixedData[1005];
    int imageIndex[1005];

    char** titleCollection = new char*[imagesCount];
    for (int i = 0; i < imagesCount; i++)
        titleCollection[i] = new char[50];

    for (int i = 0; i < imagesCount; i++) imageIndex[i] = i;

    memset(mixedData, 0, sizeof(int) * imagesCount);

    for (int i = 0; i < imagesCount; i++) {
        double similarity = 0;
        double color;

        fscanf(file, "%s", titleCollection[i]);

        for (int j = 0; j < 96; j++) {
            fscanf(file, "%lf", &color);
            similarity += sqrt((double)colorTest[j] / regionSize * color);
        }
        mixedData[i] = similarity / 4;
    }
    fclose(file);

    quick_sort(mixedData, imageIndex, imagesCount);

    QStringList findResult;

    for (int i = imagesCount-1; i >= 0; i--) {
        findResult << titleCollection[imageIndex[i]];
    }

    delete [] titleCollection;

    return findResult;
}
