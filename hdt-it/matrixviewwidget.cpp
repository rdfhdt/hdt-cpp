#include "matrixviewwidget.hpp"

#include "Color.h"
#include <fstream>
#include "stringutils.hpp"


MatrixViewWidget::MatrixViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setMouseTracking(true);
    connect(&camera, SIGNAL(cameraChanged()), (QObject *)this, SLOT(updateGL()));
    connect(&camera, SIGNAL(rotationChanged()), (QObject *)this, SIGNAL(rotationChanged()));

    timer.setInterval(20);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    //timer.start();

#ifdef GESTURES
    //grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    //grabGesture(Qt::SwipeGesture);
#endif
}

MatrixViewWidget::~MatrixViewWidget() {

}

void MatrixViewWidget::setManager(HDTController *hdtManager)
{
    this->hdtController = hdtManager;
}

Camera & MatrixViewWidget::getCamera()
{
    return camera;
}

void MatrixViewWidget::initializeGL()
{
    glClearColor(BACKGROUND_COLOR);
    //glClearDepth(1.0f);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);

    // FIXME: Check support of alpha blending
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MatrixViewWidget::paintShared()
{
    size_t nshared = hdtController->getHDT()->getDictionary()->getNshared();

    glColor4d(SHARED_AREA_COLOR);
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(0, nshared, 0);
    glVertex3f(nshared, nshared, 0);
    glVertex3f(nshared, 0, 0);
    glEnd();

    glColor4d(SHARED_AREA_BORDER_COLOR);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, nshared, 0);
    glVertex3f(nshared, nshared, 0);
    glVertex3f(nshared, 0, 0);
    glEnd();
}

void MatrixViewWidget::paintScales()
{
    hdt::Dictionary *dict = hdtController->getHDT()->getDictionary();
    size_t nsubjects = dict->getMaxSubjectID();
    size_t nobjects = dict->getMaxObjectID();
    size_t npredicates = dict->getMaxPredicateID();

    // Draw subject scale
    for (size_t i = 0; i <= nsubjects; i += 1+nsubjects / 15) {
        QString str;
        if (nsubjects > 20000) {
            str.append(QString("%1K").arg(i/1000));
        } else {
            str.append(QString("%1").arg(i));
        }
        glColor4d(TEXT_COLOR);
        this->renderText(0.0, i+nsubjects*0.01, 0, str);

        glColor4d(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(0, i, 0);
        glVertex3f(nobjects, i, 0);
        glVertex3f(0, i, 0);
        glVertex3f(0, i, npredicates);
        glEnd();
    }

    // Draw object scale
    for (size_t i = 0; i <= nobjects; i += 1+ nobjects / 15) {
        QString str;
        if (nobjects > 20000) {
            str.append(QString("%1K").arg(i/1000));
        } else {
            str.append(QString("%1").arg(i));
        }
        glColor4d(TEXT_COLOR);
        this->renderText(i, 0.0, 0, str);

        glColor4d(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(i, 0, 0);
        glVertex3f(i, nsubjects, 0);
        glVertex3f(i, 0, 0);
        glVertex3f(i, 0, npredicates);
        glEnd();
    }

    // Draw predicate scale
    for (size_t i = 0; i <= npredicates; i += 1+npredicates / 10) {
        QString str = QString::number(i);
        //texto(str, 0, 0, i);
        glColor4d(TEXT_COLOR);
        this->renderText(0, 0, i, str, QFont());

        glColor4d(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(0, 0, i);
        glVertex3f(nobjects, 0, i);
        glVertex3f(0, 0, i);
        glVertex3f(0, nsubjects, i);
        glEnd();
    }

    // Draw outter axis
    glColor4d(AXIS_COLOR);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, nsubjects, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(nobjects, 0, 0);

    glVertex3f(0, 0, npredicates);
    glVertex3f(0, nsubjects, npredicates);
    glVertex3f(0, 0, npredicates);
    glVertex3f(nobjects, 0, npredicates);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, npredicates);

    glVertex3f(0, nsubjects, 0);
    glVertex3f(0, nsubjects, npredicates);

    glVertex3f(nobjects, 0, 0);
    glVertex3f(nobjects, 0, npredicates);

    glVertex3f(nobjects, 0, 0);
    glVertex3f(nobjects, nsubjects, 0);

    glVertex3f(nobjects, nsubjects, 0);
    glVertex3f(0, nsubjects, 0);
    glEnd();

    // Draw labels
    glColor4d(TEXT_COLOR);
    renderText(0, nsubjects * 1.04, 0, "Subjects");
    renderText(nobjects * 1.05, 0, 0, "Objects");
    renderText(0, 0, npredicates*1.05, "Predicates");
}

void MatrixViewWidget::paintPoints()
{
    if(hdtController->getNumResults()==0) {
        // Do not render anything
    } else if(hdtController->getNumResults()<5000) {
	// Render directly from iterator.
        hdt::IteratorTripleID *it = hdtController->getHDT()->getTriples()->search(hdtController->getSearchPatternID());

        glPointSize(3);
        glBegin(GL_POINTS);
        while(it->hasNext()) {
            hdt::TripleID *tid = it->next();

            Color *c = hdtController->getHDTCachedInfo()->getPredicateColor(tid->getPredicate()-1);

            if(hdtController->getPredicateStatus()->isPredicateActive(tid->getPredicate()-1)) {
                glColor4d(c->r, c->g, c->b, 1.0);
            } else {
                glColor4d(c->r/4, c->g/4, c->b/4, 0.3);
            }

            glVertex3f((float)tid->getObject(), (float)tid->getSubject(), (float)tid->getPredicate());
        }
        glEnd();
        delete it;

    } else {
	// Render from cached points.
        glPointSize(RDF_POINT_SIZE);
        glBegin(GL_POINTS);
        vector<hdt::TripleID> triples = hdtController->getTriples();
        for(size_t i=0;i<triples.size();i++) {
            hdt::TripleID *tid = &triples[i];

            if(tid->match(hdtController->getSearchPatternID())) {
                Color *c = hdtController->getHDTCachedInfo()->getPredicateColor(tid->getPredicate()-1);

                if(hdtController->getPredicateStatus()->isPredicateActive(tid->getPredicate()-1)) {
                    glColor4d(c->r, c->g, c->b, 1.0);
                } else {
                    glColor4d(c->r/4, c->g/4, c->b/4, 0.3);
                }

                glVertex3f((float)tid->getObject(), (float)tid->getSubject(), (float)tid->getPredicate());
            }
        }
        glEnd();
    }

}



void MatrixViewWidget::paintSelected()
{
    hdt::TripleID selectedTriple = hdtController->getSelectedTriple();

    // Draw selected triple
    if (selectedTriple.isValid()) {
        size_t nsubjects = hdtController->getHDT()->getDictionary()->getMaxSubjectID();
        size_t npredicates = hdtController->getHDT()->getDictionary()->getMaxPredicateID();
        size_t nobjects = hdtController->getHDT()->getDictionary()->getMaxObjectID();

        float x = selectedTriple.getObject();
        float y = selectedTriple.getSubject();
        float z = selectedTriple.getPredicate();

        glColor4f(CROSS_COLOR);
        glBegin(GL_LINES);

        // Draw +
        glVertex3f(0, y, z);
        glVertex3f(nobjects, y, z);
        glVertex3f(x, 0, z);
        glVertex3f(x, nsubjects, z);

        glEnd();

        // Draw point
        glPointSize(5);
        glBegin(GL_POINTS);
        Color c;
        ColorRamp2 cr;
        cr.apply(&c, z, 0, npredicates);
        glColor4f(c.r, c.g, c.b, c.a);
        glVertex3f(x, y, z);
        glEnd();
    }
}

void MatrixViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(!hdtController->hasHDT()) {
        return;
    }

    glLoadIdentity();

    camera.applyTransform();

    size_t nsubjects = hdtController->getHDT()->getDictionary()->getMaxSubjectID();
    size_t nobjects = hdtController->getHDT()->getDictionary()->getMaxObjectID();
    size_t npredicates = hdtController->getHDT()->getDictionary()->getMaxPredicateID();

    glScalef(1.0f / (float) nobjects, 1.0f / (float) nsubjects, 1.0f / (float) npredicates);

    // PAINT SHARED AREA
    paintShared();

    // RENDER SCALES
    paintScales();

    // RENDER POINTS
    paintPoints();

    // RENDER SELECTED
    paintSelected();
}

void MatrixViewWidget::resizeGL(int w, int h)
{
    //std::cout << "Widget resize: " << w << ", " << h << std::endl;
    camera.setScreenSize(w,h);
    this->updateGL();
}

void MatrixViewWidget::mousePressEvent(QMouseEvent *event)
{
    //std::cout << event->type() << "DOW Btn: "<< event->buttons() << " Mod: " << event->modifiers() << "   " << event->x() << ", " << event->y() << std::endl;

    lastClickX = lastX = event->x();
    lastClickY = lastY = event->y();
    buttonClick = event->buttons();
}

void MatrixViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //std::cout << event->type() << "REL Btn: "<< event->buttons() << " Mod: " << event->modifiers() << "   " << event->x() << ", " << event->y() << std::endl;

    if(event->x()==lastClickX && event->y()==lastClickY) {
        //std::cout << "Mouse CLICK" << std::endl;
        if(buttonClick & Qt::LeftButton) {
            //std::cout << "Left Mouse CLICK" << std::endl;
            if(hdtController->getSelectedTriple().isValid()) {
                hdtController->getPredicateStatus()->selectPredicate(hdtController->getSelectedTriple().getPredicate());
            }
        } else if (buttonClick & Qt::RightButton) {
            //std::cout << "Right Mouse CLICK" << std::endl;
            hdtController->getPredicateStatus()->selectAllPredicates();
        }
    }
}

void MatrixViewWidget::unProject(int x, int y, double *outx, double *outy, double *outz)
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyTransform();

    hdt::Dictionary *dict = hdtController->getHDT()->getDictionary();

    glScalef(1.0f / (float) dict->getMaxObjectID(),
             1.0f / (float) dict->getMaxSubjectID(),
             1.0f / (float) dict->getMaxPredicateID());

    // UnProject
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLdouble wx = x, wy, wz;

    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3] - y;
    wy = y;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    //printf("Orig: %f %f %f\n", wx, wy, wz);
    gluUnProject(wx, wy, 0, modelview, projection, viewport, outx, outy, outz);
    //printf("Dest: %f %f %f\n", *outx, *outy, *outz);
}

void MatrixViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    //std::cout << event->type() << "MOV Btn: "<< event->buttons() << " Mod: " << event->modifiers() << "   " << event->x() << ", " << event->y() << std::endl;

    int diffx = event->x() - lastX;
    int diffy = event->y() - lastY;

    lastX = event->x();
    lastY = event->y();

    bool left = event->buttons() & Qt::LeftButton;
    bool right = event->buttons() & Qt::RightButton;
    bool shift = event->modifiers() & Qt::ShiftModifier;

    if(right || (left && shift)) {
        camera.moveOffset(1.8*diffx, 1.8*diffy);
        emit cameraChanged();
    } else if(left) {
        camera.rotateCamera(diffx, diffy);
        emit cameraChanged();
    }

    if(hdtController->getHDT()==NULL)
        return;

    if(!camera.isFrontView()) {
        hdtController->clearSelectedTriple();
        return;
    }

    GLdouble subject, predicate, object;
    this->unProject(event->x(), event->y(), &object, &subject, &predicate);

    hdt::Dictionary *dictionary = hdtController->getHDT()->getDictionary();
    if ( (subject > 0 && subject < dictionary->getMaxSubjectID()) &&
         (object > 0 && object <= dictionary->getMaxObjectID())
       ) {
        hdtController->selectNearestTriple(subject,predicate, object);

    QString subjStr = stringutils::escapeHTML(stringutils::toQString(dictionary->idToString(hdtController->getSelectedTriple().getSubject(), hdt::SUBJECT).c_str()));
    QString predStr = stringutils::escapeHTML(stringutils::toQString(dictionary->idToString(hdtController->getSelectedTriple().getPredicate(), hdt::PREDICATE).c_str()));
    QString objStr = stringutils::escapeHTML(stringutils::toQString(dictionary->idToString(hdtController->getSelectedTriple().getObject(), hdt::OBJECT).c_str()));
        stringutils::cut(objStr, 1000);
        QString tooltip = QString("<p style='white-space:pre'><b>S</b>: %1</p><p style='white-space:pre'><b>P</b>: %2</p><p><b>O</b>:&nbsp;%3</p>").arg(subjStr).arg(predStr).arg(objStr);
        QPoint point = this->mapToGlobal(event->pos());
        QRect rect = QRect(point.x()-10, point.y()+10, 20, 20);
        QToolTip::showText(point, tooltip, this, rect);
    } else {
        hdtController->clearSelectedTriple();
        QToolTip::hideText();
    }

    updateGL();
}


void MatrixViewWidget::wheelEvent( QWheelEvent* e )
{
  int delta = e->delta();
  if (e->orientation() == Qt::Horizontal) {
#ifdef GESTURES
      camera.moveOffset(delta/2, 0);
#endif
  } else {
#ifdef GESTURES
     camera.moveOffset(0, delta/2);
#else
      camera.increaseZoom(delta);
#endif
  }
  hdtController->clearSelectedTriple();
  e->accept();
}

#ifdef GESTURES
bool MatrixViewWidget::event(QEvent *event)
{
    static qreal previousZoom;
    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
        if (QGesture *swipe = gestureEvent->gesture(Qt::SwipeGesture)) {
            QSwipeGesture *swipeGesture = static_cast<QSwipeGesture *>(swipe);
        } else if (QGesture *pan = gestureEvent->gesture(Qt::PanGesture)) {
            QPanGesture *panGesture = static_cast<QPanGesture *>(pan);
            if(panGesture->state() == Qt::GestureStarted) {
                //cout << "Panning start" << endl;
            }
            if(panGesture->state() == Qt::GestureFinished) {
                //cout << "Panning end" << endl;
            }
            return true;
        } else if (QGesture *pinch = gestureEvent->gesture(Qt::PinchGesture)) {
            QPinchGesture *pinchGesture = static_cast<QPinchGesture *>(pinch);

            QPinchGesture::ChangeFlags changeFlags = pinchGesture->changeFlags();
            if (changeFlags & QPinchGesture::RotationAngleChanged) {
                qreal value = pinchGesture->property("rotationAngle").toReal();
                qreal lastValue = pinchGesture->property("lastRotationAngle").toReal();
                camera.rotateCamera(2.0*(lastValue-value), 0);
                return true;
            }

            if (changeFlags & QPinchGesture::ScaleFactorChanged) {
                if(pinchGesture->state() == Qt::GestureStarted) {
                    previousZoom = camera.getZoom();
                }
                qreal value = pinchGesture->property("scaleFactor").toReal();
                camera.setZoom(previousZoom*value);
                return true;
            }
        }
    }
    return QGLWidget::event(event);
}
#endif

QSize MatrixViewWidget::minimumSizeHint() const
{
    return QSize(300,150);
}

QSize MatrixViewWidget::sizeHint() const
{
    return QSize(800,600);
}

void MatrixViewWidget::reloadHDTInfo()
{
    if(hdtController->getHDT()==NULL) {
        return;
    }

    updateGL();
}









