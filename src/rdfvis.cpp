#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string>

#ifdef __APPLE__
 #include <OpenGL/OpenGL.h>
 #include <OpenGL/gl.h>
 #include <OpenGL/CGLDevice.h>
 #include <GLUT/glut.h>
#else 
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glut.h>
#endif

#include "Header.h"
#include "Dictionary.h"
#include "PlainDictionary.h"
#include "Triples.h"
#include "PlainTriples.h"
#include "CompactTriples.h"
#include "BitmapTriples.h"

#define TIMER_DELAY 50
#define WIDTH 1800
#define HEIGHT 1100
#define POSX 9
#define POSY 30

// Color
typedef struct {
	double r,g,b;
} COLOR;

// Variables
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
double offx=-0.5*WIDTH;
double offy=-0.5*HEIGHT;
double zoom = 0.75;
double rotx = 0, roty=0;
int mousex=0,mousey=0;
int lastx = 0;
int lasty = 0;
int button = 0;
int fps = 0;
char fps_str[10]="";
char mouse_str[2048]="";
char dataset_str[2048]="";
char currPredicateStr[2048]="";

bool clustering = false;

// HDT
Header *header;
Dictionary *dictionary;
Triples *triples;

vector<char *> datasets;
int currentFile=0;

int currpredicate=0;
unsigned int currFrame=0;
int increment=1;

TripleID *foundTriple=NULL;

void setPredicate() {
	printf("Currpredicate: %d\n", currpredicate);
	if(currpredicate>0) {
		string pred = dictionary->retrieveString(currpredicate, VOC_PREDICATE);
		sprintf(currPredicateStr, "Predicate (%d/%d): %s", currpredicate, dictionary->getNpredicates(), pred.c_str());
	} else {
		sprintf(currPredicateStr, "Predicate (ALL/%d)", dictionary->getNpredicates());
	}
}


bool loadHDT(char *hdt) {
	DataTime t1, t2;
	getTime(&t1);
	cout.precision(4);
	string fpath(hdt);
	fpath.append("/hdt");
	
	cout << "[" << showpoint << t1.user - t1.user << "]  Loading HDT" << endl;
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Reading Header from: " << fpath << endl;
	header = new Header();
	header->read(fpath);
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Dictionary from: " << fpath << endl;
	
	int mapping = header->getMapping();
	
	dictionary = new PlainDictionary(fpath, mapping, header->getD_separator(), header->getNsubjects(), header->getNpredicates(), header->getNobjects(), header->getSsubobj());
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "]  Loading Triples from: " << fpath << endl;
	switch(header->getT_encoding())
	{
		case PLAIN:
			triples = new PlainTriples(dictionary, header->getNtriples(), header->getParsing(), fpath);
			break;
		case COMPACT:
			triples = new CompactTriples(dictionary, header->getNtriples(), header->getParsing(), fpath);
			break;
		case BITMAP:
			triples = new BitmapTriples(dictionary, header->getNtriples(), header->getParsing(), fpath);
			break;
		case K2TREE:
			cout << "   <WARNING> K2-tree Triples encoding is not currently supported" << endl;
			return false;
			break;
		default:
			cout << "   <ERROR> Current triples encoding  is not supported" << endl;
			return false;
	}
	triples->loadGraphMemory();
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "] Convert to PSO" << endl;
	triples->convertParsing(PSO);
	
	getTime(&t2);
	cout << "[" << showpoint << t2.user - t1.user << "] HDT Succesfully loaded" << endl;
	
	increment = triples->size()/400000;
	increment = increment<1 ? 1 : increment;
	
	sprintf(dataset_str, "%s (%u Triples)", datasets[currentFile], triples->size());
	
	clustering = 0;
	
	currFrame= 0;
	
	setPredicate();
	
	return true;
}

void unloadHDT() {
	if(dictionary!=NULL) {
		delete dictionary;
		dictionary=NULL;
	}
	if(triples!=NULL) {
		delete triples;
		triples=NULL;
	}
	if(header!=NULL) {
		delete header;
		header=NULL;
	}
}

/* executed when a regular key is pressed */
void keyboardDown(unsigned char key, int x, int y) {
	printf("Key: %d\n", key);
	mouse_str[0]='\0';
	
	switch(key) {
		case 'r':
			currFrame=0;
			break;
		case 'c':
			if(!clustering) {
				triples->clustering();
				clustering=1;
			} else {
				unloadHDT();
				loadHDT(datasets[currentFile]);
			}
			break;
		case '+':
			zoom = zoom * 1.2;
			printf("Zoom: %.3f\n", zoom);
			break;
		case '-':
			zoom = zoom / 1.2;
			printf("Zoom: %.3f\n", zoom);
			break;
		case 'n':
			unloadHDT();
			
			currentFile++;
			if(currentFile>=datasets.size()) {
				currentFile = 0;
			}
			
			loadHDT(datasets[currentFile]);

			break;
		case '0':
			zoom =0.7;
			offx = -0.5*screenWidth;
			offy = -0.5*screenHeight;
			break;
		case '4':
			rotx = 0;
			roty = 0;
			break;
		case '5':
			rotx = 45;
			roty = -45;
			break;
		case '6':
			rotx = 0;
			roty = -90;
			break;
		case '8':
			rotx = 90;
			roty = 0;
			break;
		case 'a':
			increment++;
			printf("Increment: %d\n",increment);
			break;
		case 's':
			increment--;
			if(increment<1) increment = 1;
			printf("Increment: %d\n",increment);
			break;
		case  27:   // ESC
			unloadHDT();
			exit(0);
	}
	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) {
	
}

void keyboardSpecialUp(int k, int x, int y) {
	printf("SpecialUp %d, %d, %d\n", k, x, y);
}

void keyboardSpecialDown(int k, int x, int y) {
	printf("SpecialDown %d, %d, %d\n", k, x, y);
	mouse_str[0]='\0';
	switch (k) {
		case GLUT_KEY_RIGHT:
			offx -=20/zoom;
			printf("Offset: %.3f,%.3f\n", offx, offy);
			break;
		case GLUT_KEY_LEFT:
			offx +=20/zoom;
			printf("Offset: %.3f,%.3f\n", offx, offy);
			break;
		case GLUT_KEY_DOWN:
			offy +=20/zoom;
			printf("Offset: %.3f,%.3f\n", offx, offy);
			break;
		case GLUT_KEY_UP:
			offy -=20/zoom;
			printf("Offset: %.3f,%.3f\n", offx, offy);
			break;
		case GLUT_KEY_PAGE_UP:
			currpredicate = currpredicate==0 ? dictionary->getNpredicates()-1 : currpredicate-1;
			setPredicate();
			break;
		case GLUT_KEY_PAGE_DOWN:
			currpredicate = currpredicate>=dictionary->getNpredicates() ? 0 : currpredicate+1;
			setPredicate();
			break;
		case GLUT_KEY_HOME:
			currpredicate = 0;
			setPredicate();
			break;
	}
	glutPostRedisplay();
}

void mouseClick(int btn, int state, int x, int y) {
	//printf("Click %d, %d, %d, %d\n", button, state, x, y);
	
	if(state==GLUT_DOWN) {
		lastx=x;
		lasty=y;
	}
	button = btn;
	glutPostRedisplay();
}

void mouseMotion(int x, int y) {
	int diffx=0, diffy=0;
	
	diffx=x-lastx;
	diffy=y-lasty;
		
	//printf("Diff: %d,%d\n", diffx, diffy);
		
	lastx = x;
	lasty = y;
	
	if(button==GLUT_LEFT_BUTTON) {
		rotx += (float) 0.5f * diffy;
		roty += (float) 0.5f * diffx;
	} else if(button==GLUT_RIGHT_BUTTON) {
		offx += (float) diffx/zoom;
		offy -= (float) diffy/zoom;
	} else if(button==GLUT_MIDDLE_BUTTON) {
		zoom += (float)	0.01*diffy;
		zoom = zoom<0 ? 0.01 : zoom;
	}
	
	mouse_str[0]='\0';
	
	glutPostRedisplay();
}

TripleID *findTriple(vector<TripleID> &graph, unsigned int x, unsigned int y, unsigned int z) {
	unsigned int min=0;
	unsigned int max = graph.size()-1;
	unsigned int mid;
	unsigned int jumps = 0;

	printf("\nSearch: %u, %u, %u\n", x, y, z);
	
	// BinarySearch the triple in the graph.
	
	do {
		mid = min + (max-min)/2;
		if( x > graph[mid].x) {
			min = mid+1;
		} else if( x < graph[mid].x){
			max = mid-1;
		} else {
			if( y>graph[mid].y ) {
				min = mid+1;
			} else if( y<graph[mid].y ) {
				max = mid-1;
			} else {
				if( z<graph[mid].z ) {
					min = mid+1;
				} else if(z>graph[mid].z) {
					max = mid-1;
				}
			}
		}
		jumps++;
	} while((min<max) && ((graph[mid].x!=x) || (graph[mid].y!=y) || (graph[mid].z!=z)));
			
	printf("Found: %u, %u, %u in %u jumps\n", graph[mid].x, graph[mid].y, graph[mid].z, jumps);
	
	return &graph[mid];
}

void mousePassiveMotion(int x, int y) {
	//printf("%d %d\n", x, y);
	mousex = x;
	mousey = y;
	
	// HDT
	unsigned int nsubjects = dictionary->getNsubjects();
	unsigned int nobjects = dictionary->getNobjects();
	unsigned int npredicates = dictionary->getNpredicates();
	cout << "NSubjects: "<< nsubjects<<endl;
	cout << "Nobjects:" << nobjects<<endl;
	cout << "NPredicates: "<< npredicates<<endl;
	cout << "NShared: "<< dictionary->getSsubobj()<<endl;
	
	// ModelView
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glScaled(zoom, zoom, 1.0);
	
	glTranslatef(offx, offy, 0);	
	
	glScalef(screenWidth, screenHeight, 1.0);
	
	glTranslatef(0.5, 0.5, 0.5);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	glTranslatef(-0.5, -0.5, -0.5);
	
	//glScalef(1.0f/(float)maxVal,1.0f/(float)maxVal, 1.0f/(float)npredicates);
	glScalef(1.0f/(float)nobjects,1.0f/(float)nsubjects, 1.0f/(float)npredicates);
	
	// UnProject
	GLint viewport[4];
	GLdouble modelview[16],projection[16];
	GLdouble wx=x,wy,wz;
	
	GLdouble ox, oy, oz;
	
	glGetIntegerv(GL_VIEWPORT,viewport);
	y=viewport[3]-y;
	wy=y;

	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projection);
	glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
	//printf("Read z: %f\n", wz);
	gluUnProject(wx,wy,currpredicate,modelview,projection,viewport,&ox,&oy,&oz);
	printf("Orig: %f %f %f\n", ox, oy, oz);
	
	// Fetch Dictionary
	unsigned int subject = (unsigned int) oy;
	unsigned int object = (unsigned int) ox;
	unsigned int predicate = currpredicate;
	
	if(currpredicate!=0) {
		foundTriple = findTriple(triples->getGraph(), currpredicate, subject, object);
		predicate = foundTriple->x;
		subject = foundTriple->y;
		object = foundTriple->z;
	} else {
		foundTriple =NULL;
	}
	
	if( (subject>0 && subject<nsubjects) && (object>0 && object<nobjects)) {
		string subjStr = dictionary->retrieveString(subject, VOC_SUBJECT);
		string objStr = dictionary->retrieveString(object, VOC_OBJECT);

		sprintf(mouse_str, "%u / %u (%s, %s)", subject, object, subjStr.c_str(), objStr.c_str());

	} else {
		mouse_str[0]='\0';
	}

	glutPostRedisplay();
}

/* reshaped window */
void reshape(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	
	printf("Reshape: %d/%d\n",w,h);
	
	/*	GLfloat fieldOfView = 90.0f;
	 glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	 
	 glMatrixMode (GL_PROJECTION);
	 glLoadIdentity();
	 gluPerspective(fieldOfView, (GLfloat) width/(GLfloat) height, 0.1, 500.0);
	 
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();*/
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-screenWidth/2, +screenWidth/2, -screenHeight/2, +screenHeight/2, -10,10);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void texto( char *cadena, float x, float y, float z)
{
	char *c;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);
	glRasterPos3f(x, y, z);
	for (c = cadena; *c; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}


void timer(int valor) {
	glutPostRedisplay();
	glutTimerFunc( TIMER_DELAY, timer, 1);
}

void getColor(double v,double vmin,double vmax, COLOR *c)
{
	c->r = 1.0;
	c->g = 1.0;
	c->b = 1.0;
	
	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	
	double dv = vmax - vmin;
	
	if (v < (vmin + 0.25 * dv)) {
		c->r = 0;
		c->g = 4 * (v - vmin) / dv;
	} else if (v < (vmin + 0.5 * dv)) {
		c->r = 0;
		c->b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
	} else if (v < (vmin + 0.75 * dv)) {
		c->r = 4 * (v - vmin - 0.5 * dv) / dv;
		c->b = 0;
	} else {
		c->g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
		c->b = 0;
	}
}

/* render the scene */
void draw() {
	static int lastime=0;
	fps++;
	
	if(time(NULL)-lastime>2) {
		sprintf(fps_str,"FPS: %d",fps/2);
		lastime = time(NULL);
		fps=0;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();
	
	// Get vars
	vector<TripleID> &graph = triples->getGraph();
	unsigned int maxVal = dictionary->getMaxID();
	unsigned int nsubjects = dictionary->getNsubjects();
	unsigned int nobjects = dictionary->getNobjects();
	unsigned int npredicates = dictionary->getNpredicates();
	unsigned int maxAxis = nsubjects;
	
	glScaled(zoom, zoom, 1.0);
	
	glTranslatef(offx, offy, 1);	
	
	glScalef(screenWidth, screenHeight, 1.0);

	glTranslatef(0.5,0.5,0.5);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	glTranslatef(-0.5,-0.5,-0.5);
	
//	glScalef(1.0f/(float)maxVal,1.0f/(float)maxVal, -1.0f/(float)npredicates);
	glScalef(1.0f/(float)nobjects,1.0f/(float)nsubjects, 1.0f/(float)npredicates);	
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	// Draw subject scale
	for(unsigned int i=0;i<=nsubjects;i+=nsubjects/20) {
		char str[1024];
		sprintf(str," %uK", i/1000);
		texto(str, 0, i+nsubjects*0.01, 0);
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_LINES);
		glVertex3f(0, i, 0);
		glVertex3f(maxVal, i, 0);
		glVertex3f(0, i, 0);
		glVertex3f(0, i, npredicates);
		glEnd();
	}
	
	// Draw object scale
	for(unsigned int i=0;i<=nobjects;i+=nobjects/20) {
		char str[1024];
		sprintf(str," %uK", i/1000);
		texto(str, i, nsubjects*0.01, 0);
		
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_LINES);
		glVertex3f(i, 0, 0);
		glVertex3f(i, maxAxis, 0);
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, npredicates);
		glEnd();
		
	}
	
	// Draw predicate scale
	for(unsigned int i=0;i<=npredicates;i+=npredicates/10) {
		char str[1024];
		sprintf(str," %u", i);
		texto(str, 0, 0, i);
		
		glColor3f(0.2, 0.2, 0.2);		
		glBegin(GL_LINES);
		glVertex3f(0, 0, i);
		glVertex3f(maxVal,0,  i);
		glVertex3f(0, 0, i);
		glVertex3f(0, maxAxis,  i);
		glEnd();
	}
	
	// Draw outter axis
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, maxAxis, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(maxVal, 0, 0);
	
	glVertex3f(0, 0, npredicates);
	glVertex3f(0, maxAxis, npredicates);
	glVertex3f(0, 0, npredicates);
	glVertex3f(maxVal, 0, npredicates);
	
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, npredicates);
	
	glVertex3f(0, maxAxis, 0);
	glVertex3f(0, maxAxis, npredicates);
	
	glVertex3f(maxVal, 0, 0);
	glVertex3f(maxVal, 0, npredicates);
	
	glVertex3f(maxVal, 0, 0);
	glVertex3f(maxVal, maxAxis, 0);
	
	glVertex3f(maxVal, maxAxis, 0);
	glVertex3f(0, maxAxis, 0);
	glEnd();
	
	// Draw labels
	texto((char *)"Subjects", 0, nsubjects*1.02, 0);
	texto((char *)"Objects", nobjects*1.07, 0, 0);
	//texto("Predicates", -nsubjects*1.01, 0, npredicates*1.05);
	
	// Draw points
	glPointSize(1.5);
	glBegin(GL_POINTS);
	COLOR c;

	unsigned int maxy = 0;
	for(int i=0; (i<graph.size()) && (i<currFrame);i+=increment) {
		//printf("Triple: %u, %u, %u\n", graph[i].x, graph[i].y, graph[i].z );
		
		if( currpredicate==0 || currpredicate==graph[i].x) {
			getColor(graph[i].x, 0, dictionary->getNpredicates(), &c);
			glColor4f(c.r, c.g, c.b, 0.8);
			glVertex3f( ((float)graph[i].z), ((float)graph[i].y), ((float)graph[i].x));
			maxy = maxy>graph[i].z ? maxy : graph[i].z;
		}
	}
	glEnd();
	
	//cout << "Maxy: " << maxy << endl;
	
	if(foundTriple!=NULL) {
		glPointSize(5);
		glBegin(GL_POINTS);
		getColor(foundTriple->x, 0, dictionary->getNpredicates(), &c);
		glColor4f(c.r, c.g, c.b, 0.8);
		glVertex3f( foundTriple->z, foundTriple->y, foundTriple->x);
		glEnd();
		
		glColor4f(1.0,1.0,1.0,1.0);
		glBegin(GL_LINES);
		GLfloat sizex = nobjects/100;
		GLfloat sizey = nsubjects/100;
		glVertex3f(foundTriple->z-sizex, foundTriple->y-sizey, foundTriple->x);
		glVertex3f(foundTriple->z+sizex, foundTriple->y+sizey, foundTriple->x);
		glVertex3f(foundTriple->z-sizex, foundTriple->y+sizey, foundTriple->x);
		glVertex3f(foundTriple->z+sizex, foundTriple->y-sizey, foundTriple->x);
		glEnd();
	}

	currFrame+=graph.size()/100;

	glPopMatrix();
	
	// Draw onscreen labels
	texto(mouse_str, -screenWidth*0.5+mousex, 0.5*screenHeight-mousey+10, 0);
	texto(fps_str, -screenWidth*0.5, -screenHeight*0.5, 0);
	texto(dataset_str, -screenWidth*0.5+5, +screenHeight*0.5-14, 0);
	texto(currPredicateStr, -screenWidth*0.5+5, +screenHeight*0.5-30, 0);
	
	//	glFlush();
	glutSwapBuffers();
}

void idle() { 
	glutPostRedisplay();
}

void initGL(int width, int height) {
	
	reshape(width, height);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

int main(int argc, char** argv) {
	
	if(argc<2) {
		printf("$ ./rdfvis <hdt1> <hdt2> ...\n");
		return 0;
	}

	for(int i=1;i<argc;i++) {
		datasets.push_back(argv[i]);
	}

	loadHDT(datasets[0]);
	
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB /*| GLUT_DEPTH*/);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(POSX, POSY);
	glutCreateWindow("HDT RDF Viewer");
	
	// register glut call backs
	glutKeyboardFunc(keyboardDown);
	//glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecialDown);
	//glutSpecialUpFunc(keyboardSpecialUp);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mousePassiveMotion);
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);  
	//glutIdleFunc(idle);
	
	glutTimerFunc( TIMER_DELAY, timer, 1);
	
	initGL(WIDTH, HEIGHT);
	
	glutMainLoop();
	return 0;
}
