#ifndef HDTSPECFORM_HPP
#define HDTSPECFORM_HPP

#include <QDialog>

#include <HDTSpecification.hpp>
#include <HDTVocabulary.hpp>
#include <HDTEnums.hpp>

namespace Ui {
    class HDTSpecForm;
}

class HDTSpecForm : public QDialog
{
    Q_OBJECT

public:
    explicit HDTSpecForm(QWidget *parent = 0);
    ~HDTSpecForm();

public:
    QString getFileName();
    QString getBaseUri();
    void fillHDTSpecification(hdt::HDTSpecification &spec);
    hdt::RDFNotation getNotation();

private:
    Ui::HDTSpecForm *ui;
    std::string getStreamType(int index);

private slots:
    void on_triplesTypeCombo_currentIndexChanged(int index);
    void on_inputFileButton_clicked();
    void accept();
};

#endif // HDTSPECFORM_HPP
