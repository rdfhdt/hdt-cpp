#include "matrixviewwidget.hpp"

#include "Color.h"
#include <fstream>

MatrixViewWidget::MatrixViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setMouseTracking(true);
    connect(&camera, SIGNAL(cameraChanged()), (QObject *)this, SLOT(updateGL()));

    timer.setInterval(20);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));

    //timer.start();
}

MatrixViewWidget::~MatrixViewWidget() {

}

void MatrixViewWidget::setManager(HDTManager *hdtManager)
{
    this->hdtmanager = hdtManager;
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

}

void MatrixViewWidget::paintShared()
{
    unsigned int nshared = hdtmanager->getHDT()->getDictionary().getSsubobj();

    glColor4f(SHARED_AREA_COLOR);
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(0, nshared, 0);
    glVertex3f(nshared, nshared, 0);
    glVertex3f(nshared, 0, 0);
    glEnd();

    glColor4f(SHARED_AREA_BORDER_COLOR);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, nshared, 0);
    glVertex3f(nshared, nshared, 0);
    glVertex3f(nshared, 0, 0);
    glEnd();
}

void MatrixViewWidget::paintScales()
{
    hdt::Dictionary &dict = hdtmanager->getHDT()->getDictionary();
    unsigned int nsubjects = dict.getMaxSubjectID();
    unsigned int nobjects = dict.getMaxObjectID();
    unsigned int npredicates = dict.getMaxPredicateID();

    // Draw subject scale
    for (unsigned int i = 0; i <= nsubjects; i += 1+nsubjects / 15) {
        QString str;
        if (nsubjects > 20000) {
            str.append(QString("%1K").arg(i/1000));
        } else {
            str.append(QString("%1").arg(i));
        }
        glColor4f(TEXT_COLOR);
        this->renderText(0.0, i+nsubjects*0.01, 0, str);

        glColor4f(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(0, i, 0);
        glVertex3f(nobjects, i, 0);
        glVertex3f(0, i, 0);
        glVertex3f(0, i, npredicates);
        glEnd();
    }

    // Draw object scale
    for (unsigned int i = 0; i <= nobjects; i += 1+ nobjects / 15) {
        QString str;
        if (nobjects > 20000) {
            str.append(QString("%1K").arg(i/1000));
        } else {
            str.append(QString("%1").arg(i));
        }
        glColor4f(TEXT_COLOR);
        this->renderText(i, 0.0, 0, str);

        glColor4f(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(i, 0, 0);
        glVertex3f(i, nsubjects, 0);
        glVertex3f(i, 0, 0);
        glVertex3f(i, 0, npredicates);
        glEnd();
    }

    // Draw predicate scale
    for (unsigned int i = 0; i <= npredicates; i += 1+npredicates / 10) {
        QString str = QString::number(i);
        //texto(str, 0, 0, i);
        glColor4f(TEXT_COLOR);
        this->renderText(0, 0, i, str, QFont(), 2000);

        glColor4f(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(0, 0, i);
        glVertex3f(nobjects, 0, i);
        glVertex3f(0, 0, i);
        glVertex3f(0, nsubjects, i);
        glEnd();
    }

    // Draw outter axis
    glColor4f(AXIS_COLOR);
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
    glColor4f(TEXT_COLOR);
    renderText(0, nsubjects * 1.04, 0, "Subjects");
    renderText(nobjects * 1.05, 0, 0, "Objects");
    renderText(-nsubjects*1.01, 0, npredicates*1.05, "Predicates");
}

void MatrixViewWidget::paintPoints()
{

    Color c;
    ColorRamp2 cr;

    if(hdtmanager->getNumResults()==0) {
        // Do not render anything
    } else if(hdtmanager->getNumResults()<10000) {

        hdt::IteratorTripleID *it = hdtmanager->getHDT()->getTriples().search(hdtmanager->getSearchPatternID());

        glPointSize(3);
        glBegin(GL_POINTS);
        while(it->hasNext()) {
            hdt::TripleID *tid = it->next();

            cr.apply(&c, tid->getPredicate(), 0, hdtmanager->getHDT()->getDictionary().getMaxPredicateID());

            if(hdtmanager->isPredicateActive(tid->getPredicate()-1)) {

            } else {
                c.r = c.r/4;
                c.g = c.g/4;
                c.b = c.b/4;
            }
            glColor4f(c.r, c.g, c.b, c.a);
            glVertex3f((float)tid->getObject(), (float)tid->getSubject(), (float)tid->getPredicate());

        }
        glEnd();
        delete it;

    } else {
        glPointSize(RDF_POINT_SIZE);
        glBegin(GL_POINTS);
        vector<hdt::TripleID> triples = hdtmanager->getTriples();
        for(unsigned int i=0;i<triples.size();i++) {
            hdt::TripleID *tid = &triples[i];

            if(tid->match(hdtmanager->getSearchPatternID())) {

                cr.apply(&c, tid->getPredicate(), 0, hdtmanager->getHDT()->getDictionary().getMaxPredicateID());

                if(hdtmanager->isPredicateActive(tid->getPredicate()-1)) {

                } else {
                    c.r = c.r/4;
                    c.g = c.g/4;
                    c.b = c.b/4;
                }
                glColor4f(c.r, c.g, c.b, c.a);
                glVertex3f((float)tid->getObject(), (float)tid->getSubject(), (float)tid->getPredicate());
            }
        }
        glEnd();
    }

}



void MatrixViewWidget::paintSelected()
{
    hdt::TripleID selectedTriple = hdtmanager->getSelectedTriple();

    // Draw selected triple
    if (selectedTriple.isValid()) {
        unsigned int nsubjects = hdtmanager->getHDT()->getDictionary().getMaxSubjectID();
        unsigned int npredicates = hdtmanager->getHDT()->getDictionary().getMaxPredicateID();
        unsigned int nobjects = hdtmanager->getHDT()->getDictionary().getMaxObjectID();

        float x = selectedTriple.getObject();
        float y = selectedTriple.getSubject();
        float z = selectedTriple.getPredicate();

        glColor4f(CROSS_COLOR);
        glBegin(GL_LINES);
#if 0
        // Draw X
        GLfloat sizex = nobjects / 100;
        GLfloat sizey = nsubjects / 100;
        glVertex3f(x - sizex, y - sizey, z);
        glVertex3f(x + sizex, y + sizey, z);
        glVertex3f(x - sizex, y + sizey, z);
        glVertex3f(x + sizex, y - sizey, z);
#else
        // Draw +
        glVertex3f(0, y, z);
        glVertex3f(nobjects, y, z);
        glVertex3f(x, 0, z);
        glVertex3f(x, nsubjects, z);
#endif
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

    if(hdtmanager->getHDT()==NULL) {
        return;
    }

    glLoadIdentity();

    camera.applyTransform();

    unsigned int nsubjects = hdtmanager->getHDT()->getDictionary().getMaxSubjectID();
    unsigned int nobjects = hdtmanager->getHDT()->getDictionary().getMaxObjectID();
    unsigned int npredicates = hdtmanager->getHDT()->getDictionary().getMaxPredicateID();

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
            if(hdtmanager->getSelectedTriple().isValid()) {
                hdtmanager->selectPredicate(hdtmanager->getSelectedTriple().getPredicate());
            }
        } else if (buttonClick & Qt::RightButton) {
            //std::cout << "Right Mouse CLICK" << std::endl;
            hdtmanager->selectAllPredicates();
        }
    }
}

void MatrixViewWidget::unProject(int x, int y, double *outx, double *outy, double *outz)
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyTransform();

    hdt::Dictionary &dict = hdtmanager->getHDT()->getDictionary();

    glScalef(1.0f / (float) dict.getMaxObjectID(),
             1.0f / (float) dict.getMaxSubjectID(),
             1.0f / (float) dict.getMaxPredicateID());

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

QString cleanString(string in) {
    if(in.size()>150) {
        QString str = in.substr(0, 147).c_str();
        str.append("...");
        return str;
    }
    return in.c_str();
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

    if(right || left && shift) {
        camera.moveOffset(diffx, diffy);
        emit cameraChanged();
    } else if(left) {
        camera.rotateCamera(diffx, diffy);
        emit cameraChanged();
    }

    if(hdtmanager->getHDT()==NULL)
        return;

    if(!camera.isFrontView()) {
        hdtmanager->clearSelectedTriple();
        return;
    }

    GLdouble subject, predicate, object;
    this->unProject(event->x(), event->y(), &object, &subject, &predicate);

    QToolTip::hideText();

    hdt::Dictionary &dictionary = hdtmanager->getHDT()->getDictionary();
    if ( (subject > 0 && subject < dictionary.getMaxSubjectID()) &&
         (object > 0 && object <= dictionary.getMaxObjectID())
       ) {
        hdtmanager->selectTriple(subject,predicate, object);

        QString subjStr = cleanString(dictionary.idToString(hdtmanager->getSelectedTriple().getSubject(), hdt::SUBJECT));
        QString predStr = cleanString(dictionary.idToString(hdtmanager->getSelectedTriple().getPredicate(), hdt::PREDICATE));
        QString objStr = cleanString(dictionary.idToString(hdtmanager->getSelectedTriple().getObject(), hdt::OBJECT));
        QString coordinate = QString("S: %1\nP: %2\nO: %3").arg(subjStr).arg(predStr).arg(objStr);
        QToolTip::showText(this->mapToGlobal(event->pos()), coordinate);
    } else {
        hdtmanager->clearSelectedTriple();
    }

    updateGL();
}


void MatrixViewWidget::wheelEvent( QWheelEvent* e )
{
  int delta = e->delta();
  if (e->orientation() == Qt::Horizontal) {
      //std::cout << "DeltaX: " << delta << std::endl;
  } else {
      //std::cout << "DeltaY: " << delta << std::endl;
      camera.increaseZoom(delta);
      emit cameraChanged();
  }

  e->accept();
}

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
    cout << "Reloading HDT Info: " << hdtmanager->getHDT() << endl;

    if(hdtmanager->getHDT()==NULL) {
        return;
    }

    updateGL();
}








