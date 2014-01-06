#ifndef IMAGEPROCCESSER_H
#define IMAGEPROCCESSER_H

class ImageProccesser
{
public:
    ImageProccesser();
    void setDirectory(QString path);
    void proccessCreateTxtInfoForImages();
private:
    void readDirectoryFiles();
    QString basePath;
    QStringList imagesCollection;
};

#endif // IMAGEPROCCESSER_H
