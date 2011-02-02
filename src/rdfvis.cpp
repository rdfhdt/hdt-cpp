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
#include <stdint.h>
#include <iostream>
#include <limits>

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


#define RDFVIS_FONT GLUT_BITMAP_HELVETICA_12
#define TIMER_DELAY 50
#define WIDTH 1024 
#define HEIGHT 768
#define POSX 9
#define POSY 30
#define RENDER_NUM_POINTS 100000
#define BUF_SIZE 250

#define SCREEN

#ifndef SCREEN
#define TEXT_COLOR 0.0, 0.0, 0.0, 1.0
#define TEXT_BACKGROUND_COLOR 1.0, 1.0, 1.0, 0.9
#define BACKGROUND_COLOR 1, 1, 1, 1
#define SHARED_AREA_COLOR 0.9, 0.9, 0.9, 1.0
#define SHARED_AREA_BORDER_COLOR 0.1, 0.1, 0.1, 1.0
#define AXIS_COLOR 0.1, 0.1, 0.1, 1.0
#define GRID_COLOR 0.3, 0.3, 0.3, 1.0
#else
#define TEXT_COLOR 1.0, 1.0, 1.0, 1.0
#define TEXT_BACKGROUND_COLOR 0.0, 0.0, 0.0, 0.7
#define BACKGROUND_COLOR 0, 0, 0, 1
#define SHARED_AREA_COLOR 0.1, 0.1, 0.1, 1.0
#define SHARED_AREA_BORDER_COLOR 0.2, 0.2, 0.2, 1.0
#define AXIS_COLOR 0.8, 0.8, 0.8, 1.0
#define GRID_COLOR 0.2, 0.2, 0.2, 1.0
#endif

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
int lastclickx = 0;
int lastclicky = 0;
int button = 0;
int fps = 0;
int fullscreen = 0;
char fps_str[10]="";

char mouse_str_suj[BUF_SIZE]="";
char mouse_str_pred[BUF_SIZE]="";
char mouse_str_obj[BUF_SIZE]="";
char dataset_str[BUF_SIZE]="";
char currPredicateStr[BUF_SIZE]="";

bool clustering = false;

// HDT
Header *header;
Dictionary *dictionary;
Triples *triples;

vector<char *> datasets;
int currentFile=0;

int currpredicate=0;
unsigned int currFrame=-1;//std::numeric_limits<unsigned int>::max()-1;
int increment=1;

TripleID *foundTriple=NULL;

void setPredicate() {
	int count=0;
	vector<TripleID> graph = triples->getGraph();
	for(unsigned int i=0;i<graph.size();i++) {
		if(graph[i].x==currpredicate) {
			count++;
		}
	}
	
	printf("Currpredicate: %d\n", currpredicate);
	if(currpredicate>0) {
		string pred = dictionary->retrieveString(currpredicate, VOC_PREDICATE);
		snprintf(currPredicateStr, BUF_SIZE, "Predicate (%d/%d): %s (%d Triples)", currpredicate, dictionary->getNpredicates(), pred.c_str(), count);
	} else {
		snprintf(currPredicateStr, BUF_SIZE, "Predicate (ALL/%d)", dictionary->getNpredicates());
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
	
	increment = triples->size()/RENDER_NUM_POINTS;
	increment = increment<1 ? 1 : increment;
	
	snprintf(dataset_str, BUF_SIZE, "%s (%u Triples)", datasets[currentFile], triples->size());
	
	clustering = 0;

	
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
	printf("KeyDown: %d\n", key);
	mouse_str_suj[0]='\0';
	mouse_str_pred[0]='\0';
	mouse_str_obj[0]='\0';
	
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
			currpredicate = 0;
			
			currentFile++;
			if(currentFile>=datasets.size()) {
				currentFile = 0;
			}
			
			loadHDT(datasets[currentFile]);
		
			break;
		case 'b':
			unloadHDT();
			currpredicate = 0;
			
			currentFile--;
			if(currentFile<0) {
				currentFile = datasets.size()-1;
			}
			
			loadHDT(datasets[currentFile]);
			
			break;
		case 'm':
			unloadHDT();
			currpredicate = 0;
			
			currentFile=random() % datasets.size();
			
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
		case 'f':
			if(!fullscreen) {
				glutFullScreen();
				fullscreen=1;
			} else {
				glutReshapeWindow(WIDTH,HEIGHT);
				fullscreen=0;
			}
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
	mouse_str_suj[0]='\0';
	mouse_str_pred[0]='\0';
	mouse_str_obj[0]='\0';
	
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
		lastclickx=lastx=x;
		lastclicky=lasty=y;
	}
	button = btn;
	
	if(state==GLUT_UP && (lastclickx==x) && (lastclicky==y)) {
		printf("CLICK\n");
		if( button==GLUT_LEFT_BUTTON && foundTriple!=NULL) {
			currpredicate = foundTriple->x;
		} else if( button==GLUT_RIGHT_BUTTON ) {
			currpredicate = 0;
		}
		setPredicate();
	}
	
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
	
	mouse_str_suj[0]='\0';
	mouse_str_pred[0]='\0';
	mouse_str_obj[0]='\0';
	
	glutPostRedisplay();
}


unsigned long long inline DIST(unsigned long long x1, unsigned long long x2, unsigned long long y1, unsigned long long y2) {
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

TripleID *findTripleBinSearch(vector<TripleID> &graph, unsigned int x, unsigned int y, unsigned int z) {
	long long min=0;
	long long max = graph.size()-1;
	long long mid;
	unsigned int jumps = 0;

	//printf("\nSearch: %u, %u, %u\n", x, y, z);
	
	// BinarySearch the triple in the graph.	
	do {
//		printf("%u / %u\n", min, max); fflush(stdout);
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
	} while( (min<max) && ((graph[mid].x!=x) || (graph[mid].y!=y) || (graph[mid].z!=z)));
			
	//printf("Found: %u, %u, %u in %u jumps\n", graph[mid].x, graph[mid].y, graph[mid].z, jumps);
	return &graph[mid];
}

TripleID *findTripleSeq(vector<TripleID> &graph, unsigned int x, unsigned int y, unsigned int z) {
	TripleID *best= &graph[0];
	unsigned int bestpos=0;
	unsigned long long bestdist = DIST(graph[0].y, y, graph[0].z, z);
	
	for(unsigned int i=0; i<graph.size(); i+=increment) {
		if((x==0) || (x==graph[i].x)) {
			unsigned long long dist = DIST(graph[i].y, y, graph[i].z, z);
			
			if(dist<bestdist) {
				best = &graph[i];
				bestdist = dist;
				bestpos = i;
				//printf("1New %u, %u, %u, Dist: %u Pos: %u\n", best->x, best->y, best->z, bestdist, bestpos);
			}
		}
	}
	
	//printf("Found: %u, %u, %u, Dist: %llu\n", best->x, best->y, best->z, bestdist);	
	return best;
}

void mousePassiveMotion(int x, int y) {
	//printf("%d %d\n", x, y);
	mousex = x;
	mousey = y;
	
	// HDT
	unsigned int nsubjects = dictionary->getNsubjects();
	unsigned int nobjects = dictionary->getMaxID();
	unsigned int npredicates = dictionary->getNpredicates();
	unsigned int nshared = dictionary->getSsubobj();
	
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
	//printf("Orig: %f %f %f\n", ox, oy, oz);
	
	// Fetch Dictionary
	unsigned int subject = (unsigned int) oy;
	unsigned int object = (unsigned int) ox;
	unsigned int predicate = currpredicate;
	
	foundTriple = findTripleSeq(triples->getGraph(), currpredicate, subject, object);
	predicate = foundTriple->x;
	subject = foundTriple->y;
	object = foundTriple->z;
	
	if( (subject>0 && subject<nsubjects) && (object>0 && object<=nobjects)) {
		string subjStr = dictionary->retrieveString(subject, VOC_SUBJECT);
		string predStr = dictionary->retrieveString(predicate, VOC_PREDICATE);
		//object = object>nshared ? object-(subject-nshared) : object;
		string objStr = dictionary->retrieveString(object, VOC_OBJECT);

		snprintf(mouse_str_suj,  BUF_SIZE, "S: %6u / %s", subject, subjStr.c_str());
		snprintf(mouse_str_pred, BUF_SIZE, "P: %6u / %s", predicate, predStr.c_str());
		snprintf(mouse_str_obj,  BUF_SIZE, "O: %6u / %s", object, objStr.c_str());

	} else {
		mouse_str_suj[0]='\0';
		mouse_str_pred[0]='\0';
		mouse_str_obj[0]='\0';
	}

	glutPostRedisplay();
}

/* reshaped window */
void reshape(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	
	printf("Reshape: %d/%d\n",w,h);

	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	
	/*	GLfloat fieldOfView = 90.0f;

	 
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
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(TEXT_COLOR);
	glRasterPos3f(x, y, z);
	for (char *c = cadena; *c; c++)
	{
		glutBitmapCharacter(RDFVIS_FONT, *c);
	}
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

int textSize(char *cadena) {
	int sz = 0;
	
	for(char *c = cadena; *c; c++) {
		sz += glutBitmapWidth(RDFVIS_FONT, *c);
		//sz += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
	}
	
	return sz;
}

void textoBox( char *cadena, float x, float y, float z)
{
	if(!*cadena) return;
	
	int siz = textSize(cadena);
	
	//printf("printBox (%d%) %s\n", siz, cadena);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(TEXT_BACKGROUND_COLOR);
	glBegin(GL_QUAD_STRIP);
	glVertex3f(x-5, y-6, 0);
	glVertex3f(x-5, y+14, 0);
	glVertex3f(x+siz+5, y-6, 0);
	glVertex3f(x+siz+5, y+14, 0);
	glEnd();
	glDisable(GL_BLEND);
	
	texto(cadena,x,y,z);
}

void invertColor(COLOR *c) {
	c->r = 1-c->r;
	c->g = 1-c->g;
	c->b = 1-c->b;
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

#define RETURN_RGB(red, green, blue) {c->r = red; c->g = green; c->b = blue;}

void HSV_to_RGB( double h, double s, double v, COLOR *c ) {
	// H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
	// RGB are each returned on [0, 1].
	float m, n, f;
	int i;
	
	if (h == -1) {
		RETURN_RGB(v, v, v);
		return;
	}
	
	i = floor(h);
	f = h - i;
	if ( !(i&1) ) f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: RETURN_RGB(v, n, m); break;
		case 1: RETURN_RGB(n, v, m); break;
		case 2: RETURN_RGB(m, v, n); break;
		case 3: RETURN_RGB(m, n, v); break;
		case 4: RETURN_RGB(n, m, v); break;
		case 5: RETURN_RGB(v, m, n); break;
	}
} 

void getColor2(double val,double vmin,double vmax, COLOR *c)
{
	double h,s,v;
	double dv = vmax-vmin;
	double prop = (vmin+val/dv); 
#ifdef SCREEN
	HSV_to_RGB( fmod(prop*100,6), 1.0, 0.5+0.5*prop, c);
	invertColor(c);
#else
	HSV_to_RGB( fmod(prop*100,6), 1.0, 0.3+0.7*prop, c);
//	invertColor(c);
#endif
//	printf("(%.1f, %.1f, %.1f) %.1f\n", val, vmin, vmax, prop);
	
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
	unsigned int nsubjects = dictionary->getNsubjects();
	unsigned int nobjects = dictionary->getMaxID(); //dictionary->getNobjects();
	unsigned int npredicates = dictionary->getNpredicates();
	unsigned int nshared = dictionary->getSsubobj();
	unsigned int maxVal = dictionary->getMaxID();
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
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for(unsigned int i=0;i<=nsubjects;i+=nsubjects/20) {
		char str[BUF_SIZE];
		if(nsubjects>20000){
			snprintf(str,BUF_SIZE, " %uK", i/1000);
		} else {
			snprintf(str,BUF_SIZE, " %u", i);
		}
		texto(str, 0, i+nsubjects*0.01, 0);
		glColor4f(GRID_COLOR);
		glBegin(GL_LINES);
		glVertex3f(0, i, 0);
		glVertex3f(nobjects, i, 0);
		glVertex3f(0, i, 0);
		glVertex3f(0, i, npredicates);
		glEnd();
	}
	
	// Draw object scale
	for(unsigned int i=0;i<=nobjects;i+=nobjects/20) {
		char str[BUF_SIZE];
		if(nobjects>20000) {
			snprintf(str, BUF_SIZE," %uK", i/1000);
		} else {
			snprintf(str, BUF_SIZE," %u", i);
		}
		texto(str, i, nsubjects*0.01, 0);
		
		glColor4f(GRID_COLOR);
		glBegin(GL_LINES);
		glVertex3f(i, 0, 0);
		glVertex3f(i, nsubjects, 0);
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, npredicates);
		glEnd();
		
	}
	
	// Draw predicate scale
	for(unsigned int i=0;i<=npredicates;i+=npredicates/10) {
		char str[BUF_SIZE];
		snprintf(str, BUF_SIZE," %u", i);
		texto(str, 0, 0, i);
		
		glColor4f(GRID_COLOR);		
		glBegin(GL_LINES);
		glVertex3f(0, 0, i);
		glVertex3f(nobjects,0,  i);
		glVertex3f(0, 0, i);
		glVertex3f(0, nsubjects,  i);
		glEnd();
	}
	
	// Draw outter axis
	glColor4f(AXIS_COLOR);
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
	texto((char *)"Subjects", 0, nsubjects*1.04, 0);
	texto((char *)"Objects", nobjects*1.05, 0, 0);
	//texto("Predicates", -nsubjects*1.01, 0, npredicates*1.05);
	
	// Draw vertices
#ifdef SCREEN
	glPointSize(1.5);
#else 
	glPointSize(3);
#endif
	glBegin(GL_POINTS);
	COLOR c;

	for(int i=0; (i<graph.size()) && (i<currFrame);i+=increment) {
		//printf("Triple: %u, %u, %u\n", graph[i].x, graph[i].y, graph[i].z );
		
		if( currpredicate==0 || currpredicate==graph[i].x) {
			getColor2(graph[i].x, 0, dictionary->getNpredicates(), &c);
			glColor4f(c.r, c.g, c.b, 0.8);
			glVertex3f( ((float)graph[i].z), ((float)graph[i].y), ((float)graph[i].x));
		}
	}
	glEnd();
		
	// Draw found triple
	if(foundTriple!=NULL) {
		glPointSize(5);
		glBegin(GL_POINTS);
		getColor(foundTriple->x, 0, dictionary->getNpredicates(), &c);
		glColor4f(c.r, c.g, c.b, 0.8);
		glVertex3f( foundTriple->z, foundTriple->y, foundTriple->x);
		glEnd();
		
		glColor4f(TEXT_COLOR);
		glBegin(GL_LINES);
		GLfloat sizex = nobjects/100;
		GLfloat sizey = nsubjects/100;
		glVertex3f(foundTriple->z-sizex, foundTriple->y-sizey, foundTriple->x);
		glVertex3f(foundTriple->z+sizex, foundTriple->y+sizey, foundTriple->x);
		glVertex3f(foundTriple->z-sizex, foundTriple->y+sizey, foundTriple->x);
		glVertex3f(foundTriple->z+sizex, foundTriple->y-sizey, foundTriple->x);
		glEnd();
	}

	glPopMatrix();
	
	// Draw onscreen labels
	//int xpos = -screenWidth*0.5+mousex - 0.5*textSize(mouse_str_obj);
	int xpos = -screenWidth*0.5+mousex -100; // - 0.5*textSize(mouse_str_obj);
	if(xpos< -screenWidth*0.5+10) {
		xpos = -screenWidth*0.5+10;
	}
	textoBox(mouse_str_suj, xpos, 0.5*screenHeight-mousey+40, 0);
	textoBox(mouse_str_pred, xpos, 0.5*screenHeight-mousey+25, 0);	
	textoBox(mouse_str_obj, xpos, 0.5*screenHeight-mousey+10, 0);
	
	texto(fps_str, -screenWidth*0.5, -screenHeight*0.5, 0);
	texto(dataset_str, -screenWidth*0.5+5, +screenHeight*0.5-14, 0);
	texto(currPredicateStr, -screenWidth*0.5+5, +screenHeight*0.5-30, 0);
	
	// Animate
	if(currFrame+graph.size()/500<std::numeric_limits<unsigned int>::max()) {
		currFrame+=graph.size()/500;
		glutPostRedisplay();
	}
	
	//	glFlush();
	glutSwapBuffers();
}

void idle() { 
	glutPostRedisplay();
}

void timer(int valor) {
	glutPostRedisplay();
	glutTimerFunc( TIMER_DELAY, timer, 1);
}

void initGL(int width, int height) {
	
	reshape(width, height);
	
	glClearColor(BACKGROUND_COLOR);
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
	
	//glutTimerFunc( TIMER_DELAY, timer, 1);
	
	initGL(WIDTH, HEIGHT);
	
	glutMainLoop();
	return 0;
}
