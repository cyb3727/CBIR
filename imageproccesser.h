#ifndef IMAGEPROCCESSER_H
#define IMAGEPROCCESSER_H

class QWidget;

class ImageProccesser
{
public:
    ImageProccesser();
    void setDirectory(QString path);
    bool proccessCreateTxtInfoForImages(QWidget* parent);
private:
    void readDirectoryFiles();
    QString basePath;
    QStringList imagesCollection;
};

#endif // IMAGEPROCCESSER_H
