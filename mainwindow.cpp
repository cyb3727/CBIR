#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressDialog>
#include <QMessageBox>

#include <iostream>

#include "mainwindow.h"
#include "imagescollectionview.h"
#include "imageproccesser.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createToolBar();

    imagesCollectionView = new ImagesCollectionView;
    imageProccessor = new ImageProccesser;

    setCentralWidget(imagesCollectionView);
    setMinimumSize(1105,837);
}

void MainWindow::createActions()
{
    importAction = new QAction(tr("&Import"), this);
    importAction->setIcon(QIcon(":/images/plus.png"));
    importAction->setStatusTip("import images");
    connect(importAction,SIGNAL(triggered()),this, SLOT(import()));

    nextAction = new QAction(tr("&next"), this);
    nextAction->setIcon(QIcon(":/images/skip_forward.png"));
    connect(nextAction,SIGNAL(triggered()),this, SLOT(tonextpage()));

    previousAction = new QAction(tr("&previous"), this);
    previousAction->setIcon(QIcon(":/images/skip_backward.png"));
    connect(previousAction,SIGNAL(triggered()),this, SLOT(topreviouspage()));

    searchAction = new QAction(tr("&search"), this);
    searchAction->setIcon(QIcon(":/images/search.png"));
    connect(searchAction,SIGNAL(triggered()),this, SLOT(search()));

    searchWithSiftAction = new QAction(tr("&searchSift"), this);
    searchWithSiftAction->setIcon(QIcon(":/images/like.png"));
    connect(searchWithSiftAction,SIGNAL(triggered()),this, SLOT(searchWithSift()));

    clearAction = new QAction(tr("&clear"), this);
    clearAction->setIcon(QIcon(":/images/delete.png"));
    connect(clearAction,SIGNAL(triggered()),this, SLOT(clearPage()));
}

void MainWindow::createToolBar()
{
    actionToolBar = addToolBar(tr("&Action"));
    actionToolBar->addAction(importAction);
    actionToolBar->addSeparator();
    actionToolBar->addAction(previousAction);
    actionToolBar->addAction(nextAction);
    actionToolBar->addAction(clearAction);

    inputToolBar = addToolBar(tr("&input"));
    inputLabel = new QLabel(tr("& file name:"), this);
    inputFileName = new QLineEdit;
    inputFileName->setFixedWidth(200);
    inputLabel->setBuddy(inputFileName);
    inputToolBar->addWidget(inputLabel);
    inputToolBar->addWidget(inputFileName);

    confirmToSearch = new QPushButton(tr("&Search"), this);
    inputToolBar->addWidget(confirmToSearch);
    connect(confirmToSearch, SIGNAL(clicked()),this, SLOT(searchClicked()));

    searchToolBar = addToolBar(tr("&search"));
    searchToolBar->addAction(searchAction);
    searchToolBar->addAction(searchWithSiftAction);
}

void MainWindow::import()
{
    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                              "/",
                                                              QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    if (!directoryName.isEmpty()) {
        imagesCollectionView->setBasePath(directoryName);

        cout << "CBIR LOG Mainwindow: select directory name " << qPrintable(directoryName) << endl;

        imagesCollectionView->readDirectoryFiles();

        imageProccessor->setDirectory(directoryName);
        imageProccessor->proccessCreateTxtInfoForImages(this);
    }
}

void MainWindow::topreviouspage()
{
    cout << "CBIR LOG Mainwindow: to previous page " << endl;
    imagesCollectionView->topreviousPage();
}

void MainWindow::tonextpage()
{
    cout << "CBIR LOG Mainwindow: to next page " << endl;
    imagesCollectionView->tonextPage();
}

void MainWindow::search()
{
    cout << "CBIR LOG Search: search the similar pictures" << endl;
    imagesCollectionView->searchSimilarities(this);
}

void MainWindow::searchClicked()
{
    cout << "CBIR LOG MainWindow: searchButton clicked" << endl;
    imagesCollectionView->searchSimilaritiesWithFileName(inputFileName->text());
}

void MainWindow::searchWithSift()
{
    cout << "CBIR LOG MainWindow: search with sift" << endl;
    imagesCollectionView->searchSimilaritiesWithSift(this);
}

void MainWindow::clearPage()
{
    cout << "CBIR LOG MainWindow: clearButton clicked" << endl;
    int r = QMessageBox::warning(this, tr("Clear"),tr("Really clear?"),
                                          QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape);
    if (r == QMessageBox::Yes) {
        imagesCollectionView->clearPage();
    }
}

MainWindow::~MainWindow()
{

}
