#ifndef QCLEARLINEEDIT_HPP
#define QCLEARLINEEDIT_HPP

#include <QLineEdit>

class QToolButton;

class QClearLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    QClearLineEdit(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateCloseButton(const QString &text);

private:
    QToolButton *clearButton;

};

#endif // QCLEARLINEEDIT_HPP
