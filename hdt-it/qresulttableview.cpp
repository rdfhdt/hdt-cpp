#include "qresulttableview.hpp"

#include <iostream>

QResultTableView::QResultTableView(QWidget *parent) :
    QTableView(parent)
{
    int width = this->geometry().width();

    setColumnWidth(0, width/3);
    setColumnWidth(1, width/3);
    setColumnWidth(2, width/3);
}

void QResultTableView::resizeEvent(QResizeEvent *event)
{
    //std::cout << "Resize" << std::endl;
    //int width = event->size().width();

    int width = this->geometry().width();

    setColumnWidth(0, width/3);
    setColumnWidth(1, width/3);
    setColumnWidth(2, width/3);
}
