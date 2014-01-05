#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QListWidget>
#include <QFileDialog>
#include <iostream>

#include "mainwindow.h"
#include "imagescollectionview.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createToolBar();
    listWidget = new QListWidget;
    listWidget->addItem(tr("Appearance"));
    listWidget->addItem(tr("Web Browser"));
    listWidget->addItem(tr("Mail & News"));
    listWidget->addItem(tr("Advanced"));

    imagesCollectionView = new ImagesCollectionView;
    setCentralWidget(imagesCollectionView);
    setMinimumSize(1105,837);
}

void MainWindow::createActions()
{
    importAction = new QAction(tr("&Import"), this);
    importAction->setIcon(QIcon(":/images/add.png"));
    importAction->setStatusTip("import images");
    connect(importAction,SIGNAL(triggered()),this, SLOT(import()));

    nextAction = new QAction(tr("&next"), this);
    nextAction->setIcon(QIcon(":/images/next.png"));
    connect(nextAction,SIGNAL(triggered()),this, SLOT(tonextpage()));

    previousAction = new QAction(tr("&previous"), this);
    previousAction->setIcon(QIcon(":/images/back.png"));
    connect(previousAction,SIGNAL(triggered()),this, SLOT(topreviouspage()));

    searchAction = new QAction(tr("&search"), this);
    searchAction->setIcon(QIcon(":/images/search.png"));
}

void MainWindow::createToolBar()
{
    actionToolBar = addToolBar(tr("&Action"));
    actionToolBar->addAction(importAction);
    actionToolBar->addSeparator();
    actionToolBar->addAction(previousAction);
    actionToolBar->addAction(nextAction);
    actionToolBar->addSeparator();
    actionToolBar->addAction(searchAction);
}

void MainWindow::import()
{
    QString directoryName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                              "/",
                                                              QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    if (!directoryName.isEmpty()) {
        imagesCollectionView->setBasePath(directoryName);
        printQStringToConsole(directoryName);
        imagesCollectionView->readDirectoryFiles();
    }
}

void MainWindow::printQStringToConsole(QString log)
{
    QByteArray out(log.toLatin1());
    cout << "CBIR LOG: " << out.data() << endl;
}

void MainWindow::topreviouspage()
{
    imagesCollectionView->topreviousPage();
}

void MainWindow::tonextpage()
{
    imagesCollectionView->tonextPage();
}

MainWindow::~MainWindow()
{

}
