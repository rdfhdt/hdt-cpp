#ifndef HDTIT_HPP
#define HDTIT_HPP

#include <QMainWindow>
#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>

namespace Ui {
    class HDTit;
}

class HDTit : public QMainWindow
{
    Q_OBJECT

private:
    Ui::HDTit *ui;
    void openHDTFile(QString &file);
    void importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec);

public:
    explicit HDTit(QWidget *parent = 0);
    ~HDTit();

private slots:
    void set3Dview();
    void on_searchPatternButton_clicked();

    void on_actionOpenHDT_triggered();
    void on_actionImportRDF_triggered();
    void on_actionSaveHDT_triggered();
    void on_actionExportRDF_triggered();

    void on_actionFrontView_toggled(bool arg1);
    void on_actionLeftView_toggled(bool arg1);
    void on_actionTopView_toggled(bool arg1);
    void on_action3Dview_toggled(bool arg1);

    void on_actionTest_triggered();


};

#endif // HDTIT_HPP
