/*
 * Camera.cpp
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#include <iostream>

#include "Camera.h"

Camera::Camera() {
    widgetWidth = 800;
    widgetHeight = 600;
    toDefaultValues();
}

void Camera::setZoom(double newz)
{
    zoom = newz;
    emit cameraChanged();
}

void Camera::increaseZoom(int dif)
{
    zoom += 0.001*dif*zoom;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit cameraChanged();
}

void Camera::decreaseZoom(int dif){
    zoom -= 0.001*dif*zoom;
    zoom = zoom < 0.5 ? 0.5 : zoom;
    emit cameraChanged();
}

void Camera::setOffset(double x,double y){
    offx = x;
    offy = y;
    emit cameraChanged();
}

void Camera::moveOffset(double x,double y){
    offx += (float) x / (widgetWidth*zoom);
    offy -= (float) y / (widgetHeight*zoom);
    emit cameraChanged();
}

void Camera::setRotation(double x,double y){
    rotx = x;
    roty = y;
    emit cameraChanged();
}

void Camera::rotateCamera(double x,double y){
    rotx += (float) 0.5f * x;
    roty += (float) 0.5f * y;
    emit cameraChanged();
}

void Camera::toDefaultValues(){
    rotx = roty = 0;
    zoom = 1;
    offx = -0.5;
    offy = -0.5;
    emit cameraChanged();
}

void Camera::setScreenSize(int w, int h){
    widgetWidth = w;
    widgetHeight = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

#if 0
#if 0
    GLfloat fieldOfView = 90.0f;

     glMatrixMode (GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(fieldOfView, (GLfloat) widgetWidth/(GLfloat) widgetHeight, 0.1, 500.0);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-widgetWidth / 2, +widgetWidth / 2, -widgetHeight / 2,
                    +widgetHeight / 2, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
#else
    GLdouble    aspect, left, right, bottom, top;

    /* compute aspect ratio */
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
#endif
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
    setRotation(0, 0);
}

void Camera::setLeftView()
{
    setRotation(90,0);
}

void Camera::setTopView()
{
    setRotation(0,90);
}

void Camera::set3DView()
{
    setRotation(-45,45);
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

