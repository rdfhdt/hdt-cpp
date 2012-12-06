#ifndef MATRIXVIEWWIDGET_HPP
#define MATRIXVIEWWIDGET_HPP

#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>

#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

#include <HDT.hpp>
#include <HDTSpecification.hpp>

#include "colors.hpp"
#include "Camera.h"
#include "StopWatch.hpp"

#include "hdtcontroller.hpp"
#include "searchresultsmodel.hpp"
#include "triplecomponentmodel.hpp"

class SearchResultsModel;
class TripleComponentModel;
class HDTController;

#ifdef __APPLE__
//#define GESTURES
#endif

class MatrixViewWidget : public QGLWidget
{
    Q_OBJECT

private:
    QTimer timer;
    Camera camera;
    int lastX, lastY, lastClickX, lastClickY, buttonClick;
    HDTController *hdtController;

    void unProject(int x, int y, double *outx, double *outy, double *outz);

    void paintShared();
    void paintScales();
    void paintPoints();
    void paintSelected();

public:
    explicit MatrixViewWidget(QWidget *parent = 0);
    ~MatrixViewWidget();

    void setManager(HDTController *hdtManager);

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
#ifdef GESTURES
    bool event(QEvent *);
#endif

public slots:
    void reloadHDTInfo();

signals:
    void rotationChanged();
    void cameraChanged();
};


#endif // MATRIXVIEWWIDGET_HPP
