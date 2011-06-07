/*
 * Camera.cpp
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#include <iostream>

#include "Camera.h"

Camera::Camera() {
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
    offx += (float) x / zoom;
    offy -= (float) y / zoom;
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
    zoom = 0.8;
    widgetWidth = 800;
    widgetHeight = 600;
    offx = -0.5 * widgetWidth;
    offy = -0.5 * widgetHeight;
    emit cameraChanged();
}

void Camera::setScreenSize(int w, int h){
    widgetWidth = w;
    widgetHeight = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    /*GLfloat fieldOfView = 90.0f;

     glMatrixMode (GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(fieldOfView, (GLfloat) width/(GLfloat) height, 0.1, 500.0);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();*/

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-widgetWidth / 2, +widgetWidth / 2, -widgetHeight / 2,
                    +widgetHeight / 2, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Camera::applyTransform(){
    glScaled(zoom*0.8, zoom*0.8, 1.0);

    glTranslatef(offx, offy, 1);

    glScalef(widgetWidth, widgetHeight, 1.0);

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

void Camera::setRightView()
{
    setRotation(-90,0);
}

void Camera::setTopView()
{
    setRotation(0,90);
}

void Camera::set3DView()
{
    setRotation(-45,45);
}

