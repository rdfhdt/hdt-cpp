#include "matrixviewwidget.hpp"

#include <Color.h>
#include <fstream>

#define BUF_SIZE 1024

MatrixViewWidget::MatrixViewWidget(QWidget *parent) :
    QGLWidget(parent),
    hdt(NULL),
    searchPattern(0,0,0)
{
    predicateModel = new PredicateModel(this, this, hdt::PREDICATE);
    setMouseTracking(true);
    connect(&camera, SIGNAL(cameraChanged()), (QObject *)this, SLOT(updateGL()));
}

MatrixViewWidget::~MatrixViewWidget() {
    closeHDT();
    delete predicateModel;
}

void MatrixViewWidget::updateOnHDTChanged()
{
    activePredicate.resize(hdt->getDictionary().getNpredicates(), true);

    triples.clear();

    predicateCount.clear();
    predicateCount.resize(hdt->getDictionary().getNpredicates(), 0);
    maxPredicateCount = 0;

    hdt::Triples &t = hdt->getTriples();
    hdt::TripleID pattern(0,0,0);

    hdt::IteratorTripleID *it = t.search(pattern);
    while(it->hasNext()) {
        hdt::TripleID tid = it->next();

        triples.push_back(tid);
        predicateCount[tid.getPredicate()-1]++;

        if(maxPredicateCount<predicateCount[tid.getPredicate()-1]) {
            maxPredicateCount = predicateCount[tid.getPredicate()-1];
        }
    }
    delete it;

    predicateModel->notifyLayoutChanged();
    this->updateGL();

    emit maxPredicateCountChanged(maxPredicateCount);
    emit datasetChanged();
}

void MatrixViewWidget::openHDTFile(QString file)
{
    std::ifstream in(file.toAscii());

    if(in.good()) {
        closeHDT();

        hdt = hdt::HDTFactory::createDefaultHDT();
        hdt->loadFromHDT(in);
        in.close();

        updateOnHDTChanged();
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void MatrixViewWidget::saveHDTFile(QString file)
{
    if(hdt!=NULL) {
        ofstream out;

        // Save HDT
        out.open(file.toAscii());
        if(out.good()){
            try {
                hdt->saveToHDT(out);
            } catch (char *e) {
                QMessageBox::critical(0, "HDT Save Error", e);
            }
            out.close();
        } else {
            QMessageBox::critical(0, "File Error", "Could not open file: "+file);
        }

    }
}

void MatrixViewWidget::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    std::ifstream in(file.toAscii());

    if(in.good()) {
        closeHDT();

        hdt = hdt::HDTFactory::createHDT(spec);
        hdt->loadFromRDF(in, notation);
        in.close();

        updateOnHDTChanged();
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void MatrixViewWidget::exportRDFFile(QString file, hdt::RDFNotation notation)
{
    if(hdt!=NULL) {
        ofstream out;

        // Save HDT
        out.open(file.toAscii());
        if(out.good()){
            try {
                hdt->saveToRDF(out, notation);
            } catch (char *e) {
                QMessageBox::critical(0, "RDF Export Error", e);
            }
            out.close();
        } else {
            QMessageBox::critical(0, "File Error", "Could not open file: "+file);
        }
    }
}


void MatrixViewWidget::closeHDT()
{
    if(hdt!=NULL) {
        activePredicate.clear();
        delete hdt;
        predicateModel->notifyLayoutChanged();
        hdt = NULL;
        emit datasetChanged();
    }
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

void MatrixViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(hdt==NULL) {
        return;
    }

    glLoadIdentity();

    camera.applyTransform();

    unsigned int nsubjects = hdt->getDictionary().getMaxSubjectID();
    unsigned int nobjects = hdt->getDictionary().getMaxObjectID(); //dictionary->getNobjects();
    unsigned int npredicates = hdt->getDictionary().getMaxPredicateID();
    unsigned int nshared = hdt->getDictionary().getSsubobj();

    glScalef(1.0f / (float) nobjects, 1.0f / (float) nsubjects, 1.0f / (float) npredicates);

    // Draw shared area
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

    // Draw subject scale
    for (unsigned int i = 0; i <= nsubjects; i += nsubjects / 15) {
        char str[BUF_SIZE];
        if (nsubjects > 20000) {
            snprintf(str, BUF_SIZE, " %uK", i / 1000);
        } else {
            snprintf(str, BUF_SIZE, " %u", i);
        }
        //texto(str, 0, i + nsubjects * 0.01, 0);
        glColor4f(TEXT_COLOR);
        this->renderText(0, i+nsubjects*0.01, 0, str);

        glColor4f(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(0, i, 0);
        glVertex3f(nobjects, i, 0);
        glVertex3f(0, i, 0);
        glVertex3f(0, i, npredicates);
        glEnd();
    }

    // Draw object scale
    for (unsigned int i = 0; i <= nobjects; i += nobjects / 15) {
        char str[BUF_SIZE];
        if (nobjects > 20000) {
            snprintf(str, BUF_SIZE, " %uK", i / 1000);
        } else {
            snprintf(str, BUF_SIZE, " %u", i);
        }

        //texto(str, i, nsubjects * 0.01, 0);
        glColor4f(TEXT_COLOR);
        this->renderText(i, 0, 0, str);

        glColor4f(GRID_COLOR);
        glBegin(GL_LINES);
        glVertex3f(i, 0, 0);
        glVertex3f(i, nsubjects, 0);
        glVertex3f(i, 0, 0);
        glVertex3f(i, 0, npredicates);
        glEnd();
    }

    // Draw predicate scale
    for (unsigned int i = 0; i <= npredicates; i += npredicates / 10) {
        char str[BUF_SIZE];
        snprintf(str, BUF_SIZE, " %u", i);
        //texto(str, 0, 0, i);
        glColor4f(TEXT_COLOR);
        this->renderText(0, 0, i, tr(str), QFont(), 2000);

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

    // RENDER POINTS

    glPointSize(RDF_POINT_SIZE);

    glBegin(GL_POINTS);

#if 1
    hdt::TripleID all(0,0,0);
    hdt::IteratorTripleID *it = hdt->getTriples().search(searchPattern);

    while(it->hasNext()) {
        hdt::TripleID tid = it->next();
        Color c;
        ColorRamp2 cr;
        cr.apply(&c, tid.getPredicate(), 1, npredicates);

        if(activePredicate[tid.getPredicate()-1]) {

        } else {
            c.r = c.r/4;
            c.g = c.g/4;
            c.b = c.b/4;
        }
        glColor4f(c.r, c.g, c.b, c.a);
        glVertex3f((float)tid.getObject(), (float)tid.getSubject(), (float)tid.getPredicate());

    }
    delete it;
#else

    for(unsigned int i=0;i<triples.size();i++) {
        hdt::TripleID *tid = &triples[i];
        Color c;
        ColorRamp2 cr;
        cr.apply(&c, tid->getPredicate(), 1, npredicates);

        if(activePredicate[tid->getPredicate()-1]) {

        } else {
            c.r = c.r/4;
            c.g = c.g/4;
            c.b = c.b/4;
        }
        glColor4f(c.r, c.g, c.b, c.a);
        glVertex3f((float)tid->getObject(), (float)tid->getSubject(), (float)tid->getPredicate());

    }

#endif

    glEnd();
}

void MatrixViewWidget::resizeGL(int w, int h)
{
    std::cout << "Widget resize: " << w << ", " << h << std::endl;
    camera.setScreenSize(w,h);
    this->updateGL();
}

void MatrixViewWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout << event->type() << "DOW Btn: "<< event->buttons() << " Mod: " << event->modifiers() << "   " << event->x() << ", " << event->y() << std::endl;

    lastClickX = lastX = event->x();
    lastClickY = lastY = event->y();
    buttonClick = event->buttons();
}

void MatrixViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    std::cout << event->type() << "REL Btn: "<< event->buttons() << " Mod: " << event->modifiers() << "   " << event->x() << ", " << event->y() << std::endl;

    if(event->x()==lastClickX && event->y()==lastClickY) {
        std::cout << "Mouse CLICK" << std::endl;
        if(buttonClick & Qt::LeftButton) {
            std::cout << "Left Mouse CLICK" << std::endl;
        } else if (buttonClick & Qt::RightButton) {
            std::cout << "Right Mouse CLICK" << std::endl;
        }
    }
}

void MatrixViewWidget::unProject(int x, int y, double *outx, double *outy, double *outz)
{
    if(hdt==NULL)
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyTransform();

    glScalef(1.0f / (float) hdt->getDictionary().getNobjects(),
             1.0f / (float) hdt->getDictionary().getNsubjects(),
             1.0f / (float) hdt->getDictionary().getNpredicates());

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
    printf("Orig: %f %f %f\n", wx, wy, wz);
    gluUnProject(wx, wy, 0, modelview, projection, viewport, outx, outy, outz);
    printf("Dest: %f %f %f\n", *outx, *outy, *outz);
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

    GLdouble x, y, z;
    this->unProject(event->x(), event->y(), &x, &y, &z);

    QString coordinate = QString("S: %1\nP: %2\nO: %3").arg((int)x).arg((int)z).arg((int)y);

    QToolTip::hideText();
    QToolTip::showText(this->mapToGlobal(event->pos()), coordinate);
}


void MatrixViewWidget::wheelEvent( QWheelEvent* e )
{
  int delta = e->delta();
  if (e->orientation() == Qt::Horizontal) {
      std::cout << "DeltaX: " << delta << std::endl;
  } else {
      camera.increaseZoom(delta);
      emit cameraChanged();
      std::cout << "DeltaY: " << delta << std::endl;
  }

  e->accept();
}

QSize MatrixViewWidget::minimumSizeHint() const
{
    return QSize(200,200);
}

QSize MatrixViewWidget::sizeHint() const
{
    return QSize(800,600);
}

PredicateModel * MatrixViewWidget::getPredicateModel()
{
    return predicateModel;
}

void MatrixViewWidget::selectAllPredicates()
{
    cout << "selectAllPredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = true;
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    updateGL();
}

void MatrixViewWidget::selectNonePredicates()
{
    cout << "selectNonePredicates" << endl;
    cout << "selectAllPredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = false;
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    updateGL();
}

void MatrixViewWidget::setSearchPattern(hdt::TripleString pattern)
{
    try {
        searchPattern = hdt->getDictionary().tripleStringtoTripleID(pattern);
        cout << "Search Pattern ID: " << searchPattern << endl;
    } catch (char *exception){
        cout << "Exception" << endl;
    }
}

void MatrixViewWidget::setMinimumPredicateCount(int count)
{
    //count = count * maxPredicateCount / 100;

    for(unsigned int i=0;i<activePredicate.size();i++) {
        //cout << "PredicateCount: " << i << " => " << predicateCount[i] << endl;
        activePredicate[i] = predicateCount[i]>=count;
    }

    this->updateGL();
    predicateModel->itemsChanged(0, activePredicate.size());
}


int MatrixViewWidget::getMaxPredicateCount()
{
    return maxPredicateCount;
}


PredicateModel::PredicateModel(QObject *parent, MatrixViewWidget *view, hdt::TripleComponentRole compRole) : matrixView(view), tripleComponentRole(compRole)
{

}

int PredicateModel::rowCount(const QModelIndex &parent) const
{
    //cout << "Row Count" << endl;

    if(matrixView->hdt != NULL) {
        hdt::Dictionary &dict = matrixView->hdt->getDictionary();
        switch(tripleComponentRole) {
        case hdt::SUBJECT:
            return dict.getNsubjects();
        case hdt::PREDICATE:
            return dict.getNpredicates();
        case hdt::OBJECT:
            return dict.getNobjects();
        }
    }
    return 0;
}

int PredicateModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant PredicateModel::data(const QModelIndex &index, int role) const
{
    if(matrixView->hdt == NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole:
    {
        hdt::Dictionary &d = matrixView->hdt->getDictionary();
        //cout << "Data: " << index.row()+1 << endl;
        return d.idToString(index.row()+1, tripleComponentRole).c_str();
    }
    case Qt::FontRole:
    {
        QFont font;
        font.setPointSize(10);
        return font;
    }
    case Qt::CheckStateRole:
        if(tripleComponentRole==hdt::PREDICATE) {
            return matrixView->activePredicate[index.row()] ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

bool PredicateModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(matrixView->hdt == NULL) {
        return false;
    }

    cout << "SetData" << endl;
    switch(role) {
        case Qt::CheckStateRole:
        if(tripleComponentRole==hdt::PREDICATE) {
            matrixView->activePredicate[index.row()] = value.toBool();
            matrixView->updateGL();
        }
    }
    return true;
}

Qt::ItemFlags PredicateModel::flags(const QModelIndex &index) const
{
    //cout << "Flags" << endl;
    if(tripleComponentRole == hdt::PREDICATE) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

void PredicateModel::itemsChanged(unsigned int ini, unsigned fin)
{
    QModelIndex first = createIndex(ini, 1);
    QModelIndex last = createIndex(fin, 1);
    emit dataChanged(first, last);
}

void PredicateModel::notifyLayoutChanged()
{
    emit layoutChanged();
}





