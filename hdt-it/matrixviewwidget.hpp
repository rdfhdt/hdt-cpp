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
#include "StopWatch.hpp"

#include "hdtmanager.hpp"
#include "searchresultsmodel.hpp"
#include "triplecomponentmodel.hpp"

class SearchResultsModel;
class TripleComponentModel;
class HDTManager;

class MatrixViewWidget : public QGLWidget
{
    Q_OBJECT

private:
    QTimer timer;
    Camera camera;
    int lastX, lastY, lastClickX, lastClickY, buttonClick;
    HDTManager *hdtmanager;

    vector<hdt::TripleID> triples;

    void unProject(int x, int y, double *outx, double *outy, double *outz);
    void selectTriple(int s, int p, int o);

    void paintShared();
    void paintScales();
    void paintPoints();
    void paintSelected();

public:
    explicit MatrixViewWidget(QWidget *parent = 0);
    ~MatrixViewWidget();

    void setManager(HDTManager *hdtManager);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    Camera &getCamera();
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent( QWheelEvent* e );

public slots:
    void reloadHDTInfo();

signals:
    void cameraChanged();
};


#endif // MATRIXVIEWWIDGET_HPP
