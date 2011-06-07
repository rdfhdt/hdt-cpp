/*
 * Camera.h
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <QtOpenGL>

#include <QObject>

class Camera : public QObject {
    Q_OBJECT

private:
    double zoom;
    double rotx, roty;
    double offx, offy;
    int widgetWidth, widgetHeight;

public:
    explicit Camera();

public:
    void setZoom(double newz);
    void increaseZoom(int dif);
    void decreaseZoom(int dif);

    void setOffset(double x,double y);
    void moveOffset(double x,double y);

    void setRotation(double x,double y);
    void rotateCamera(double x,double y);

    void toDefaultValues();

    void setScreenSize(int width, int height);
    void applyTransform();

public slots:
    void setFrontView();
    void setLeftView();
    void setRightView();
    void setTopView();
    void set3DView();

signals:
    void cameraChanged();
};

#endif /* CAMERA_H_ */
