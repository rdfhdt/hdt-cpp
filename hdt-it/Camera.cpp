/*
 * Camera.cpp
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#include <iostream>

#include "Camera.h"

Camera::Camera() :
    rotationAnimation(this, "camRotation")
{
    widgetWidth = 800;
    widgetHeight = 600;
    toDefaultValues();

    connect(this, SIGNAL(rotationChanged()), this, SIGNAL(cameraChanged()));
    connect(this, SIGNAL(zoomChanged()), this, SIGNAL(cameraChanged()));
    connect(this, SIGNAL(offsetChanged()), this, SIGNAL(cameraChanged()));
}

Camera::~Camera()
{
}

qreal Camera::getZoom()
{
    return zoom;
}

void Camera::setZoom(double newz)
{
    zoom = newz;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit zoomChanged();
}

void Camera::increaseZoom(int dif)
{
    zoom += 0.001*dif*zoom;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit zoomChanged();
}

void Camera::decreaseZoom(int dif){
    zoom -= 0.001*dif*zoom;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit zoomChanged();
}

void Camera::multiplyZoom(int mult)
{
    zoom *= mult;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit zoomChanged();
}


void Camera::setOffset(double x,double y){
    offx = x;
    offy = y;
    if(offx<-1.0) offx=-1.0;
    if(offx>0.0) offx=0.0;
    if(offy<-1.0) offy=-1.0;
    if(offy>0.0) offy=0.0;
    emit offsetChanged();
}

void Camera::setOffset(QPointF &rect){
    this->setOffset(rect.x(), rect.y());
}

QPointF Camera::getOffset() {
    return QPointF(offx, offy);
}

void Camera::moveOffset(double x,double y){
    setOffset(offx + x / (zoom*widgetWidth), offy - y / (zoom*widgetHeight));
}

void Camera::setRotation(double x,double y){
    rotx = x;
    roty = y;
    emit rotationChanged();
}

void Camera::setRotation(QPointF &rect){
    this->setRotation(rect.x(), rect.y());
}

void Camera::animateRotation(double x, double y) {
    rotationAnimation.stop();
    rotationAnimation.setDuration(500);
    rotationAnimation.setEasingCurve(QEasingCurve::InOutQuad);
    rotationAnimation.setStartValue(QPointF(rotx, roty));
    rotationAnimation.setEndValue(QPointF(x,y));
    rotationAnimation.start();
}

QPointF Camera::getRotation() {
    return QPointF(rotx, roty);
}

void Camera::rotateCamera(double x,double y){
    rotationAnimation.stop();
    rotx += 0.5 * x;
    roty += 0.5 * y;
    emit rotationChanged();
}

void Camera::toDefaultValues(){
    rotx = roty = 0;
    zoom = 1;
    offx = -0.5;
    offy = -0.5;
    emit rotationChanged();
    emit zoomChanged();
    emit offsetChanged();
}

void Camera::setScreenSize(int w, int h){
    widgetWidth = w;
    widgetHeight = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    GLdouble aspect, left, right, bottom, top;

    aspect = (GLdouble) widgetWidth / (GLdouble) widgetHeight;

    if ( aspect < 1.0 ) {
         left = -0.75;
         right = 0.75;
         bottom = -1.0 * ( 1.0 / aspect );
         top = 1.0 * ( 1.0 / aspect );
    } else {
         left = -0.75 * aspect;
         right = 0.75 * aspect;
         bottom = -1.0;
         top = 1.0;
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( left*widgetWidth, right*widgetWidth, bottom*widgetHeight, top*widgetHeight, -10, 10 );
    glMatrixMode( GL_MODELVIEW );
}

void Camera::applyTransform(){
    glScaled(1.1*zoom, 1.1*zoom, 1.0);

    glScalef(widgetWidth, widgetHeight, 1.0);

    glTranslatef(offx, offy, 1);

    glTranslatef(0.5, 0.5, 0.5);
    glRotatef(roty, 1, 0, 0);
    glRotatef(rotx, 0, 1, 0);
    glTranslatef(-0.5, -0.5, -0.5);
}

void Camera::setFrontView()
{
    animateRotation(0, 0);
}

void Camera::setLeftView()
{
    animateRotation(90,0);
}

void Camera::setTopView()
{
    animateRotation(0,90);
}

void Camera::set3DView()
{
    animateRotation(-45,45);
}

bool Camera::isFrontView()
{
    return rotx==0 && roty==0;
}

bool Camera::isLeftView()
{
    return rotx==90 && roty==0;
}

bool Camera::isTopView()
{
    return rotx==0 && roty==90;
}

bool Camera::is3DView()
{
    return rotx==-45 && roty==45;
}

void Camera::toDefaultValuesAnimated()
{
    QPropertyAnimation *rotAnim = new QPropertyAnimation(this, "camRotation");
    rotAnim->setEasingCurve(QEasingCurve::InOutQuad);
    rotAnim->setDuration(400);
    rotAnim->setStartValue(QPointF(rotx, roty));
    rotAnim->setEndValue(QPointF(0,0));

    QPropertyAnimation *zoomAnim = new QPropertyAnimation(this, "camZoom");
    zoomAnim->setEasingCurve(QEasingCurve::InOutQuad);
    zoomAnim->setDuration(400);
    zoomAnim->setStartValue(zoom);
    zoomAnim->setEndValue(1);

    QPropertyAnimation *offAnim = new QPropertyAnimation(this, "camOffset");
    offAnim->setEasingCurve(QEasingCurve::InOutQuad);
    offAnim->setDuration(400);
    offAnim->setStartValue(QPointF(offx, offy));
    offAnim->setEndValue(QPointF(-0.5, -0.5));

    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup();

    if(offAnim->startValue()!=offAnim->endValue())
        animationGroup->addAnimation(offAnim);
    else
        delete offAnim;

    if(zoomAnim->startValue()!=zoomAnim->endValue())
        animationGroup->addAnimation(zoomAnim);
    else
        delete zoomAnim;

    if(rotAnim->startValue()!=rotAnim->endValue())
        animationGroup->addAnimation(rotAnim);
    else
        delete rotAnim;

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void Camera::stopAnimations()
{
    rotationAnimation.stop();
}


