#ifndef QCLEARLINEEDIT_HPP
#define QCLEARLINEEDIT_HPP

#include <QLineEdit>

#include "dictionarysuggestions.hpp"

class QToolButton;

class QClearLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    QClearLineEdit(QWidget *parent = 0);
    ~QClearLineEdit();
    DictionarySuggestions *getSuggestions();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateCloseButton(const QString &text);

private:
    QToolButton *clearButton;
    DictionarySuggestions *suggestions;
};

#endif // QCLEARLINEEDIT_HPP
