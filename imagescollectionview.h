#ifndef IMAGESCOLLECTIONVIEW_H
#define IMAGESCOLLECTIONVIEW_H

#include <QWidget>
#include <QLabel>

class QGridLayout;

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

private:
    void printQStringToConsole(QString log);
    void createImagePage();
    int count;
    QWidget *imagePageWidget;
    QString basePath;
    QStringList imagesCollection;
    QGridLayout* pageGrid;
    QLabel images[16];
};

#endif // IMAGESCOLLECTIONVIEW_H
