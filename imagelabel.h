#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class QMouseEvent;

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QLabel *parent = 0);
    void setBeingSelected(bool s);
    bool getBeingSelected();
signals:

public slots:

private:
    bool beingSelected;
};

#endif // IMAGELABEL_H
