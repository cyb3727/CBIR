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
signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    void printQStringToConsole(QString log);
    void createImagePage();
    int count;
    int selectedIndexofImage;
    QWidget *imagePageWidget;
    QString basePath;
    QStringList imagesCollection;
    QGridLayout* pageGrid;
    ImageLabel images[16];
};

#endif // IMAGESCOLLECTIONVIEW_H
