#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class ImagesCollectionView;
class ImageProccesser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void import();
    void topreviouspage();
    void tonextpage();
private:
    void createActions();
    void createToolBar();
    void printQStringToConsole(QString log);
    QToolBar *actionToolBar;
    QAction *importAction;
    QAction *previousAction;
    QAction *nextAction;
    QAction *searchAction;
    QListWidget *listWidget;
    ImagesCollectionView *imagesCollectionView;
    ImageProccesser *imageProccessor;
    char **pathArray;
};

#endif // MAINWINDOW_H
