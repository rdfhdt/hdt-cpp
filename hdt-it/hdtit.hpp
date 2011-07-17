#ifndef HDTIT_HPP
#define HDTIT_HPP

#include <QMainWindow>
#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <QAbstractItemModel>
#include <QDir>

#include "hdtmanager.hpp"

namespace Ui {
    class HDTit;
}

class HDTit : public QMainWindow
{
    Q_OBJECT

private:
    Ui::HDTit *ui;
    HDTManager *hdtManager;
    QString lastDir;
    void openHDTFile(QString &file);
    void importRDFFile(QString &file, hdt::RDFNotation notation, hdt::HDTSpecification &spec);

    void hdtChanged(QString &file);
    void updateNumResults();

public:
    explicit HDTit(QWidget *parent = 0);
    ~HDTit();

private slots:
    void on_actionOpenHDT_triggered();
    void on_actionImportRDF_triggered();
    void on_actionSaveHDT_triggered();
    void on_actionExportRDF_triggered();

    void on_actionFrontView_toggled(bool arg1);
    void on_actionLeftView_toggled(bool arg1);
    void on_actionTopView_toggled(bool arg1);
    void on_action3Dview_toggled(bool arg1);

    void searchPatternEdited();
    void refreshSearchPattern();

    void setPatternSubject(QModelIndex index);
    void setPatternPredicate(QModelIndex index);
    void setPatternObject(QModelIndex index);

    void setPatternGlobal(QModelIndex index);
    void on_actionAbout_triggered();

    void updateViewButtons();
};

#endif // HDTIT_HPP
