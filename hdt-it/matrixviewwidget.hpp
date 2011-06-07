#ifndef MATRIXVIEWWIDGET_HPP
#define MATRIXVIEWWIDGET_HPP

#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>

#include <HDT.hpp>
#include <HDTFactory.hpp>
#include <HDTSpecification.hpp>

#include "colors.hpp"
#include "Camera.h"

class MatrixViewWidget;

class PredicateModel : public QAbstractTableModel {
    Q_OBJECT
private:
    MatrixViewWidget *matrixView;
    hdt::TripleComponentRole tripleComponentRole;
public:
    PredicateModel(QObject *parent, MatrixViewWidget *view, hdt::TripleComponentRole compRole);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void itemsChanged(unsigned int ini, unsigned fin);
    void notifyLayoutChanged();
signals:
};


class MatrixViewWidget : public QGLWidget
{
    Q_OBJECT

private:
    Camera camera;
    int lastX, lastY, lastClickX, lastClickY, buttonClick;
    hdt::HDT *hdt;
    PredicateModel *predicateModel;
    vector<bool> activePredicate;
    vector<unsigned int> predicateCount;
    unsigned int maxPredicateCount;
    hdt::TripleID searchPattern;
    vector<hdt::TripleID> triples;

    void updateOnHDTChanged();
    void unProject(int x, int y, double *outx, double *outy, double *outz);

public:
    explicit MatrixViewWidget(QWidget *parent = 0);
    ~MatrixViewWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void openHDTFile(QString file);
    void saveHDTFile(QString file);

    void importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec);
    void exportRDFFile(QString file, hdt::RDFNotation notation);

    void closeHDT();
    PredicateModel *getPredicateModel();

    int getMaxPredicateCount();

    void setSearchPattern(hdt::TripleString pattern);

    Camera &getCamera();



protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent( QWheelEvent* e );

signals:
    void datasetChanged();
    void cameraChanged();
    void maxPredicateCountChanged(int maxpredicate);

public slots:
    void selectAllPredicates();
    void selectNonePredicates();
    void setMinimumPredicateCount(int count);

    friend class PredicateModel;
};



#endif // MATRIXVIEWWIDGET_HPP
