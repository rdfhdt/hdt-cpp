#ifndef HDTIT_HPP
#define HDTIT_HPP

#include <QtGui>
#include <HDTSpecification.hpp>
#include <HDTEnums.hpp>
#include <HDTVocabulary.hpp>
#include <QAbstractItemModel>
#include <QDir>
#include <QMainWindow>
#include <QTableView>

#include "hdtcontroller.hpp"

namespace Ui {
    class HDTit;
}

class HDTit : public QMainWindow
{
    Q_OBJECT

private:
    Ui::HDTit *ui;
    HDTController *hdtController;
    QString lastDir;
    QModelIndex lastContextMenuCell;
    QTableView *lastContextMenuTable;

    void hdtChanged(QString &file);

public:
    explicit HDTit(QWidget *parent = 0);
    ~HDTit();

    void openHDTFile(QString &file);
    void importRDFFile(QString &file, const string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec);

    HDTController *getManager();
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
    void on_actionSparql_triggered();
    void on_regexSearchButton_clicked();
    void on_regexEdit_editingFinished();
    void on_regexResultsView_doubleClicked(const QModelIndex &index);
};

#endif // HDTIT_HPP
