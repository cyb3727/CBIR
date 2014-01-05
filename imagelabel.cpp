#include <QMouseEvent>

#include "imagelabel.h"

ImageLabel::ImageLabel(QLabel *parent) :
    QLabel(parent)
{
    setMargin(2);
    beingSelected = false;
}

void ImageLabel::setBeingSelected(bool s)
{
    beingSelected = s;
    if (s) {
        setStyleSheet("QLabel { background-color: red}");
    } else {
        setStyleSheet("QLabel { background-color: transparent}");
    }
}

bool ImageLabel::getBeingSelected()
{
    return beingSelected;
}
