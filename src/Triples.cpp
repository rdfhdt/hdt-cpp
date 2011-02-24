/* Triples.cpp
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class to implement different forms of the Triples component
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#include "Triples.h"
#include "Histogram.h"

/** Insert
 * @param subject Description of the param.
 * @param predicate Description of the param.
 * @param object Description of the param.
 * @return void
 */
void Triples::insert(unsigned int subject, unsigned int predicate,
		unsigned int object) {
	if ((subject == -1) || (predicate == -1) || (object == -1)) {
		return;
	}

	if (parsing <= SOP) {
		graph[ntriples].x = subject;
	} else if (parsing <= POS) {
		graph[ntriples].x = predicate;
	} else {
		graph[ntriples].x = object;
	}

	if ((parsing == PSO) || (parsing == OSP)) {
		graph[ntriples].y = subject;
	} else if ((parsing == SPO) || (parsing == OPS)) {
		graph[ntriples].y = predicate;
	} else {
		graph[ntriples].y = object;
	}

	if ((parsing == POS) || (parsing == OPS)) {
		graph[ntriples].z = subject;
	} else if ((parsing == SOP) || (parsing == OSP)) {
		graph[ntriples].z = predicate;
	} else {
		graph[ntriples].z = object;
	}

	ntriples++;
}

/** Retrieve Triple
 * @param x Description of the param.
 * @param y Description of the param.
 * @param z Description of the param.
 * @return The expected result
 */
TripleID Triples::retrieveTriple(unsigned int x, unsigned int y, unsigned int z) {
	TripleID triple;

	if (parsing <= SOP) {
		triple.x = x;
	} else if (parsing <= POS) {
		triple.y = x;
	} else {
		triple.z = x;
	}

	if ((parsing == PSO) || (parsing == OSP)) {
		triple.x = y;
	} else if ((parsing == SPO) || (parsing == OPS)) {
		triple.y = y;
	} else {
		triple.z = y;
	}

	if ((parsing == POS) || (parsing == OPS)) {
		triple.x = z;
	} else if ((parsing == SOP) || (parsing == OSP)) {
		triple.y = z;
	} else {
		triple.z = z;
	}

	return triple;
}

/// TODO: Bad debug here
#if 0
#define findDest(data, max1, max2) ( ((((double)data)*max1)/max2) )
//#define findDest(data, max1, max2) (data)

#define setMax(x,y) ( x = (x>y) ? (x) : (y) )
#define setMin(x,y) ( x = (x<y) ? (x) : (y) )

#define MAX_ITEMS 1000

void
Triples::gnuplot(unsigned int npredicates, vector <string> predicates, unsigned int max, string path)
{
	char *xstring = new char[16];

	FILE *data = NULL;

	// All Predicates
	string filename(path);
	filename.append("all.gnu");

	gnuplotHeader(1, npredicates, predicates[0], 1, max, 1, max, filename);

	unsigned int ymin = max, ymax = 0, zmin = max, zmax = 0;
	unsigned int x = 0, y = 0, z = 0;
	unsigned int added=0,discarded=0;

	for (unsigned int i=0; i<ntriples; i++)
	{
		if (x != graph[i].x)
		{
			if (x != 0)
			{
				// Predicate finished
				fclose(data);

				filename = path;
				sprintf(xstring,"%d", x);
				filename.append(xstring);
				filename.append(".gnu");

				// Print Gnuplot Header for the global-squared matrix
				gnuplotHeader(x, 1, predicates[x], 1, max, 1, max, (char*)filename.c_str());

				// Print Gnuplot Header for the local-adjusted matrix
				filename.replace(path.length(), filename.length(), xstring);
				filename.append("b.gnu");
				gnuplotHeader(x, 1, predicates[x], ymin-2, ymax+2, zmin-2, zmax+2, (char*)filename.c_str());
			}

			// Processing a new predicate
			ymin = max, ymax = 0, zmin = max, zmax = 0;

			x = graph[i].x;

			// Opening data file for this predicate
			filename = path;
			sprintf(xstring,"%d",x);
			filename.append(xstring);
			filename.append(".dat");
			data = fopen(filename.c_str(), "w");
		}

		unsigned int newy = findDest(graph[i].y, MAX_ITEMS, max);
		unsigned int newz = findDest(graph[i].z, MAX_ITEMS, max);

		if( (newy!=y) || (newz!=z) ) {
			added++;
			//cout << "Subject: " << graph[i].y << " => " << newy << endl;
			//cout << "Object: " << graph[i].z << " => " << newz << endl;

			y = newy;
			z = newz;

			if (parsing == PSO)
			fprintf(data, "%d %d\n", graph[i].z, graph[i].y);
			else
			fprintf(data, "%d %d\n", graph[i].y, graph[i].z);

			setMin(ymin, graph[i].y);
			setMax(ymax, graph[i].y);
			setMin(zmin, graph[i].z);
			setMax(zmax, graph[i].z);
		} else
		{
			discarded++;
			//cout << "\tSubject: " << graph[i].y << " => " << newy << endl;
			//cout << "\tObject: " << graph[i].z << " => " << newz << endl;
		}
	}

	cout << "Added: "<< added << endl;
	cout << "Discarded: "<< discarded << endl;

	// Last predicate
	fclose(data);

	filename = path;
	sprintf(xstring,"%d", x);
	filename.append(xstring);
	filename.append(".gnu");
	gnuplotHeader(x, 1, predicates[x], 1, max, 1, max, (char*)filename.c_str());

	filename.replace(path.length(), filename.length(), xstring);
	filename.append("b.gnu");
	gnuplotHeader(x, 1, predicates[x], ymin, ymax, zmin, zmax, (char*)filename.c_str());

	delete[] xstring;
}

#else

#define findDest(data, max1, max2) ( ((((double)data)*max1)/max2) )
//#define findDest(data, max1, max2) (data)

#define findSrc(data,max1,max2) (unsigned int)((((double)data)*max2)/max1)

#define setMax(x,y) ( x = (x>y) ? (x) : (y) )
#define setMin(x,y) ( x = (x<y) ? (x) : (y) )

#define MAX_ITEM 1000

/** Gnuplot
 * @param npredicates Description of the param.
 * @param predicates Description of the param.
 * @param max Description of the param.
 * @param path Description of the param.
 * @return void
 */
void Triples::gnuplot(unsigned int npredicates, vector<string> predicates,
		unsigned int max, string path) {
	//std::vector<std::vector<std::vector<unsigned int> > > data;
	cout << "max: " << max << endl;

	char *xstring = new char[16];

	FILE *data = NULL;

	// All Predicates
	string filename(path);
	filename.append("all.gnu");

	gnuplotHeader(1, npredicates, predicates[0], 1, max, 1, max, filename);

	unsigned int size = MAX_ITEM * MAX_ITEM;
	unsigned char *dataMatrix = new unsigned char[size];

	unsigned int x = 0;
	unsigned int ymin = max, ymax = 0, zmin = max, zmax = 0;

	for (int i = 0; i < size; i++) {
		dataMatrix[i] = 0;
	}

	for (unsigned int i = 0; i < ntriples; i++) {
		if (x != graph[i].x) {
			if (x != 0) {
				//cout << "End predicate" << endl;

				// Opening data file for this predicate
				filename = path;
				sprintf(xstring, "%d", x);
				filename.append(xstring);
				filename.append(".dat");
				data = fopen(filename.c_str(), "w");

				for (int i = 0; i < MAX_ITEM; i++) {
					for (int j = 0; j < MAX_ITEM; j++) {
						unsigned char item = dataMatrix[(i * MAX_ITEM) + j];
						//						cout << item << " ";

						if (item > 0) {
							int y = findSrc(i, MAX_ITEM, max);
							int z = findSrc(j, MAX_ITEM, max);

							if (parsing == PSO)
								fprintf(data, "%d %d\n", z, y);
							else
								fprintf(data, "%d %d\n", y, z);
						}

						dataMatrix[(i * MAX_ITEM) + j] = 0;
					}
					//					cout << endl;
				}

				// Predicate finished
				fclose(data);

				// Creating GnuPlot
				filename = path;
				sprintf(xstring, "%d", x);
				filename.append(xstring);
				filename.append(".gnu");

				// Print Gnuplot Header for the global-squared matrix
				gnuplotHeader(x, 1, predicates[x], 1, max, 1, max,
						(char*) filename.c_str());

				// Print Gnuplot Header for the local-adjusted matrix
				filename.replace(path.length(), filename.length(), xstring);
				filename.append("b.gnu");
				gnuplotHeader(x, 1, predicates[x], ymin - 2, ymax + 2,
						zmin - 2, zmax + 2, (char*) filename.c_str());
			}

			x = graph[i].x;

			//cout << "New predicate: " << x << endl;

			// Processing a new predicate
			ymin = max, ymax = 0, zmin = max, zmax = 0;
		}

		unsigned int newy = findDest(graph[i].y, MAX_ITEM, max);
		unsigned int newz = findDest(graph[i].z, MAX_ITEM, max);

		//cout << "Subject: " << graph[i].y << " => " << newy << " => " << findSrc(newy, MAX_ITEM, max)<< endl;
		//cout << "Object: " << graph[i].z << " => " << newz << " => " << findSrc(newz, MAX_ITEM, max)<< endl;

		dataMatrix[(newy * MAX_ITEM) + newz] = 1;

		setMin(ymin, graph[i].y);
		setMax(ymax, graph[i].y);
		setMin(zmin, graph[i].z);
		setMax(zmax, graph[i].z);
	}

	cout << "End predicate " << endl;
}
#endif

/** Graph Sort
 * @return void
 */
void Triples::graphSort() {
	graph.resize(ntriples);
	sort(graph.begin(), graph.end(), Triples::sortXYZ);
}

/** Size
 * @return The expected result
 */
unsigned int Triples::size() {
	return ntriples;
}

/** Gnuplot Header
 * @param firstP Description of the param.
 * @param sizeP Description of the param.
 * @param predicate Description of the param.
 * @param xmin Description of the param.
 * @param xmax Description of the param.
 * @param ymin Description of the param.
 * @param ymax Description of the param.
 * @param filename Description of the param.
 * @return void
 */
void Triples::gnuplotHeader(unsigned int firstP, unsigned int sizeP,
		string predicate, unsigned int xmin, unsigned int xmax,
		unsigned int ymin, unsigned int ymax, string filename) {
	FILE *header = fopen(filename.c_str(), "w");

	fprintf(header, "set terminal png size 1024,768\n");
	fprintf(header, "set pointsize 0.35\n");
	fprintf(header, "#set logscale\n");
	fprintf(header, "set output \"%s.png\"\n", filename.c_str());
	fprintf(header, "set xlabel \"Objects\"\n");
	fprintf(header, "set ylabel \"Subjects\"\n");

	if (parsing == PSO) {
		fprintf(header, "set xrange [%d:%d]\n", ymin, ymax);
		fprintf(header, "set yrange [%d:%d]\n", xmin, xmax);
	} else {
		fprintf(header, "set xrange [%d:%d]\n", xmin, xmax);
		fprintf(header, "set yrange [%d:%d]\n", ymin, ymax);
	}

	if (sizeP > 1) {
		fprintf(header, "plot \"1.dat\" notitle pt 7, \\\n");

		for (unsigned int i = 2; i <= sizeP - 1; i++)
			fprintf(header, "\"%d.dat\" notitle pt 7, \\\n", i);

		fprintf(header, "\"%d.dat\" notitle pt 7\n", sizeP);
	} else {
		fprintf(header, "plot \"%d.dat\" title \"%s\"\n", firstP,
				predicate.c_str());
	}

	fclose(header);
}

/** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
inline void swap(char &a, char &b) {
	unsigned int tmp;

	tmp = a;
	a = b;
	b = tmp;
}

/** Swap
 * @param a Description of the param.
 * @param b Description of the param.
 * @return void
 */
inline void swap(unsigned int &a, unsigned int &b) {
	unsigned int tmp;

	tmp = a;
	a = b;
	b = tmp;
}

/*
 SRC
 v
 SPO  SOP  PSO  POS  OSP  OPS   <DEST
 SPO       yz   xy   xyz2 xyz1 xz
 SOP  yz        xyz1 xz   xy   xyz2
 PSO  xy   xyz2      yz   xz   xyz1
 POS  xyz1 xz   yz        xyz2
 OSP  xyz2 xy   xz   xyz1      yz	
 OPS  xz   xyz1 xyz2 xy   yz
 
 SWAP:
 xy   12
 xz   13
 yz   23
 xyz1 12 13
 xyz2 12 23
 */

// swap 1-2 
//xy xyz1 xyz2
bool swap1tab[6][6] = { { 0, 0, 1, 1, 1, 0 }, { 0, 0, 1, 0, 1, 1 }, { 1, 1, 0,
		0, 0, 1 }, { 1, 0, 0, 0, 1, 0 }, { 1, 1, 0, 1, 0, 0 }, { 0, 1, 1, 1, 0,
		0 } };

// swap 1-3
// xz xyz1
bool swap2tab[6][6] = { { 0, 0, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 0 }, { 0, 0, 0,
		0, 1, 1 }, { 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0 }, { 1, 1, 0, 0, 0,
		0 } };

// swap 2-3
// yz xyz2
bool swap3tab[6][6] = { { 0, 1, 0, 1, 0, 0 }, { 1, 0, 0, 0, 0, 1 }, { 0, 1, 0,
		1, 0, 0 }, { 0, 0, 1, 0, 1, 0 }, { 1, 0, 0, 0, 0, 1 }, { 0, 0, 1, 0, 1,
		0 } };

/** Convert Parsing
 * @param to Description of the param.
 * @return void
 */
void Triples::convertParsing(unsigned int to) {

	if (parsing == to)
		return;

	bool swap1 = swap1tab[parsing - 1][to - 1];
	bool swap2 = swap2tab[parsing - 1][to - 1];
	bool swap3 = swap3tab[parsing - 1][to - 1];

	//TODO: Bad debug here
#if 0
	char str[4];
	strcpy(str, parsingTypeStr[parsing]);

	cout << "Convert from " << parsingTypeStr[parsing] << " to " << parsingTypeStr[to] << endl;

	if(swap1) {
		swap(str[0], str[1]);
		cout << "\tSwap x y" << endl;
	}

	if(swap2) {
		swap(str[0], str[2]);
		cout << "\tSwap x z" << endl;
	}

	if(swap3) {
		swap(str[1], str[2]);
		cout << "\tSwap y z" << endl;
	}
	cout << "Result: "<< str << endl;
#endif	

	for (int i = 0; i < ntriples; i++) {
		if (swap1) {
			swap(graph[i].x, graph[i].y);
		}

		if (swap2) {
			swap(graph[i].x, graph[i].z);
		}

		if (swap3) {
			swap(graph[i].y, graph[i].z);
		}
	}
	parsing = to;
	graphSort();
}

/** Calculate Degree
 * @param path Description of the param.
 * @return void
 */
void Triples::calculateDegree(string path) {
	const int maxval = 50000;
	const int nbins = 50000;

	Histogram hDegree(0, maxval, nbins);
	Histogram hDegreePartial(0, maxval, nbins);
	Histogram hDegreeLabeled(0, maxval, nbins);

	int xcount = 1, ycount = 1, ychanged = 1;

	int x = graph[0].x;
	int y = graph[0].y;
	int z = graph[0].z;

	//cout << graph[0].x << " " << graph[0].y << " " << graph[0].z << endl;	
	for (int i = 1; i < ntriples; i++) {
		//cout<<graph[i].x<< " " << graph[i].y << " " << graph[i].z<<"\n";
		// Ignore duplicate triples
		if ((x == graph[i].x) && (y == graph[i].y) && (z == graph[i].z)) {
			continue;
		}

		if (x != graph[i].x) {
			//cout << "\tdegree: " << xcount <<endl;
			hDegree.Add(xcount);

			//cout << "\tpartial degree: " << ycount << endl;
			hDegreePartial.Add(ycount);

			//cout << "\tlabeled degree: " << ychanged << endl;
			hDegreeLabeled.Add(ychanged);

			xcount = ycount = 1;
			ychanged = 1;
		} else {
			xcount++;

			if (y != graph[i].y) {
				ychanged++;

				//cout << "\tpartial degree: " << ycount << endl;
				hDegreePartial.Add(ycount);
				;

				ycount = 1;
			} else {
				ycount++;
			}
		}
		
		

		//cout << graph[i].x << " " << graph[i].y << " " << graph[i].z << endl;

		x = graph[i].x;
		y = graph[i].y;
		z = graph[i].z;
	}

		//cout << "\tdegree: " << xcount <<endl;
		hDegree.Add(xcount);

		//cout << "\tpartial degree: " << ycount << endl;
		hDegreePartial.Add(ycount);

		//cout << "\tlabeled degree: " << ychanged << endl;
		hDegreeLabeled.Add(ychanged);
		
	hDegree.end();
	hDegreePartial.end();
	hDegreeLabeled.end();

	ofstream out;
	
	string direcc=""; //predicate total degree is neither in nor out
	if (parsing == SPO) direcc="out";
	else if (parsing == OPS) direcc="in";
	else direcc="predicate";
	
	

	//cout << endl << "Partial degree" << endl;
	
	if (parsing==PSO){
		out.open((path + "_Predicate").c_str(), ios::out);
		out << "# Predicate degree" << endl;
		hDegree.dumpStr(out);
		out.close();
		
		out.open((path + "_inPredicate").c_str(), ios::out);
		out << "# Predicate_in degree" << endl;
		hDegreeLabeled.dumpStr(out);
		out.close();
		
	}
	else if (parsing==POS){
		out.open((path + "_outPredicate").c_str(), ios::out);
		out << "# Predicate_out degree" << endl;
		hDegreeLabeled.dumpStr(out);
		out.close();
	}
	else if (parsing==SOP){
		out.open((path + "_DirectOut").c_str(), ios::out);
		out << "# Direct_out degree" << endl;
		hDegreeLabeled.dumpStr(out);
		out.close();
	}
	else if (parsing==OSP){
		out.open((path + "_DirectIn").c_str(), ios::out);
		out << "# Direct_in degree" << endl;
		hDegreeLabeled.dumpStr(out);
		out.close();
	}
	else{
		string direcc = (parsing == SPO) ? "out" : "in";
		//	cout << endl << "Degree" << endl;
		out.open((path + "_" + direcc).c_str(), ios::out);
		out << "# " << direcc << " degree" << endl;
		hDegree.dumpStr(out);
		out.close();
	
		out.open((path + "_p" + direcc).c_str(), ios::out);
		out << "# Partial " << direcc << " degree" << endl;
		hDegreePartial.dumpStr(out);
		out.close();
		//cout << endl << "Labeled degree" << endl;
		out.open((path + "_l" + direcc).c_str(), ios::out);
		out << "# Labeled" << direcc << " degree" << endl;
		hDegreeLabeled.dumpStr(out);
		out.close();
	}

}

/** Calculate Predicate Histogram
 * @param path Description of the param.
 * @return void
 */
void Triples::calculatePredicateHistogram(string path) {
	int npred = dictionary->getNpredicates();
	Histogram hPred(0, npred, npred);

	for (unsigned int i = 0; i < graph.size(); i++) {
		hPred.Add(graph[i].x);
		//cout<<graph[i].x<< " " << graph[i].y << " " << graph[i].z<<"\n";
	}
	hPred.end();

	ofstream out;
	out.open((path).c_str(), ios::out);
	
	hPred.dumpStr(out);
	out.close();
}

/** Calculate Degrees
 * @param path Description of the param.
 * @return void
 */
void Triples::calculateDegrees(string path) {

	cout << "Calculate OUT Degree" << endl;
	convertParsing( SPO);
	calculateDegree(path);

	cout << "Calculate IN Degree" << endl;
	convertParsing( OPS);
	calculateDegree(path);
	
	cout << "Calculate Direct OUT Degree" << endl;
	convertParsing( SOP);
	calculateDegree(path);
	
	cout << "Calculate Direct IN Degree" << endl;
	convertParsing( OSP);
	calculateDegree(path);
	
	cout << "Calculate Predicate Degree" << endl;
	cout << "Calculate Predicate IN Degree" << endl;
	convertParsing( PSO);
	calculateDegree(path);
	
	cout << "Calculate Predicate OUT Degree" << endl;
	convertParsing( POS);
	calculateDegree(path);
	
	
	
}
