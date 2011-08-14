/*
 * Camera.h
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <QPointF>
#include <QPropertyAnimation>

#include <QtOpenGL>

class Camera : public QObject {
    Q_OBJECT
    Q_PROPERTY(QPointF camRotation READ getRotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QPointF camOffset READ getOffset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal camZoom READ getZoom WRITE setZoom NOTIFY zoomChanged)

private:
    double zoom;
    double rotx, roty;
    double offx, offy;
    int widgetWidth, widgetHeight;
    QPropertyAnimation rotationAnimation;

    void stopAnimations();
public:
    explicit Camera();
    ~Camera();

public:
    qreal getZoom();
    void setZoom(double newz);
    void increaseZoom(int dif);
    void decreaseZoom(int dif);
    void multiplyZoom(int mult);

    void setOffset(double x,double y);
    void setOffset(QPointF &rect);
    QPointF getOffset();

    void moveOffset(double x,double y);

    QPointF getRotation();
    void setRotation(QPointF &rect);
    void setRotation(double x,double y);
    void animateRotation(double x,double y);
    void rotateCamera(double x,double y);

    void toDefaultValues();
    void toDefaultValuesAnimated();

    void setScreenSize(int width, int height);
    void applyTransform();

    bool isFrontView();
    bool isLeftView();
    bool isTopView();
    bool is3DView();

public slots:
    void setFrontView();
    void setLeftView();
    void setTopView();
    void set3DView();

signals:
    void rotationChanged();
    void zoomChanged();
    void offsetChanged();
    void cameraChanged();
};

#endif /* CAMERA_H_ */
