#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class ImagesCollectionView;
class ImageProccesser;
class QLineEdit;
class QPushButton;
class QLabel;

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
    void search();
    void clearPage();
    void searchClicked();

private:
    void createActions();
    void createToolBar();
    QAction *importAction;
    QAction *previousAction;
    QAction *nextAction;
    QAction *searchAction;
    QAction *clearAction;

    ImagesCollectionView *imagesCollectionView;
    ImageProccesser *imageProccessor;
    char **pathArray;

    QToolBar *actionToolBar;
    QToolBar *inputToolBar;
    QLabel *inputLabel;
    QLineEdit *inputFileName;
    QPushButton *confirmToSearch;
};

#endif // MAINWINDOW_H
