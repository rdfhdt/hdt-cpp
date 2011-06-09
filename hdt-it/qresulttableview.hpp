#ifndef QRESULTTABLEVIEW_HPP
#define QRESULTTABLEVIEW_HPP

#include <QTableView>

class QResultTableView : public QTableView
{
    Q_OBJECT
public:
    explicit QResultTableView(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:

};

#endif // QRESULTTABLEVIEW_HPP
