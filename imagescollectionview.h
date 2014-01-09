#ifndef IMAGESCOLLECTIONVIEW_H
#define IMAGESCOLLECTIONVIEW_H

#include <QWidget>
#include <QLabel>

#include "imagelabel.h"

class QGridLayout;
class QMouseEvent;

class ImagesCollectionView : public QWidget
{
    Q_OBJECT
public:
    explicit ImagesCollectionView(QWidget *parent = 0);
    void setBasePath(QString path);
    void readDirectoryFiles();
    void topreviousPage();
    void tonextPage();
    void searchSimilarities();
    void searchSimilaritiesWithFileName(QString filename);
    bool searchSimilaritiesWithSift(QWidget *parent);
    void clearPage();
    QStringList findSimilarities(QString fileName);
    void allResult();
    void calculateSearchAccuracy(QWidget *parent);
    void calculateTheGroupApValue();
signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void printQStringToConsole(QString log);
    void createImagePage();
    int sift(char *img1_file, char *img2_file);
    double calculateApValue();

    int count;
    int selectedIndexofImage;
    QWidget *imagePageWidget;
    QString basePath;
    QStringList imagesCollection;
    QGridLayout* pageGrid;
    ImageLabel images[16];

    int partition(double *data, int *index, int low,int high);
    void sort(double *data, int *index, int low, int high);
    void quick_sort(double *data, int *index, int n);

    QStringList find(QString fileName);
};

#endif // IMAGESCOLLECTIONVIEW_H
