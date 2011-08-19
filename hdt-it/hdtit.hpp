#ifndef HDTIT_HPP
#define HDTIT_HPP

#include <QtGui>
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
    QModelIndex lastContextMenuCell;
    QTableView *lastContextMenuTable;

    void hdtChanged(QString &file);

public:
    explicit HDTit(QWidget *parent = 0);
    ~HDTit();

    void openHDTFile(QString &file);
    void importRDFFile(QString &file, string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec);

private slots:
    void on_actionOpenHDT_triggered();
    void on_actionImportRDF_triggered();
    void on_actionSaveHDT_triggered();
    void on_actionExportRDF_triggered();
    void on_actionClose_triggered();

    void on_actionFrontView_toggled(bool arg1);
    void on_actionLeftView_toggled(bool arg1);
    void on_actionTopView_toggled(bool arg1);
    void on_action3Dview_toggled(bool arg1);
    void on_actionReset_triggered();

    void searchPatternEdited();
    void refreshSearchPattern();

    void setPatternSubject(QModelIndex index);
    void setPatternPredicate(QModelIndex index);
    void setPatternObject(QModelIndex index);

    void showContextMenu(QPoint pos);
    void setPatternContextSubject();
    void setPatternContextPredicate();
    void setPatternContextObject();
    void copyResultTableSelection();

    void setPatternGlobal(QModelIndex index);
    void on_actionAbout_triggered();

    void updateViewButtons();
    void updateNumResults();
    void on_actionFull_Screen_triggered();
};

#endif // HDTIT_HPP
