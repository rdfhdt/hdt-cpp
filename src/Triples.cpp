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

void 
Triples::insert(unsigned int subject, unsigned int predicate, unsigned int object)
{	
	if (parsing <= SOP)
	{
		graph[ntriples].x = subject;
	}
	else if (parsing <= POS)
	{
		graph[ntriples].x = predicate;
	}
	else
	{
		graph[ntriples].x = object;
	}
	
	if ((parsing == PSO) || (parsing == OSP))
	{
		graph[ntriples].y = subject;
	}
	else if ((parsing == SPO) || (parsing == OPS))
	{
		graph[ntriples].y = predicate;
	}
	else
	{
		graph[ntriples].y = object;
	}
	
	if ((parsing == POS) || (parsing == OPS))
	{
		graph[ntriples].z = subject;
	}
	else if ((parsing == SOP) || (parsing == OSP))
	{
		graph[ntriples].z = predicate;
	}
	else
	{
		graph[ntriples].z = object;
	}
	
	ntriples++;
}

TripleID
Triples::retrieveTriple(unsigned int x, unsigned int y, unsigned int z)
{
	TripleID triple;
	
	if (parsing <= SOP)
	{
		triple.x = x;
	}
	else if (parsing <= POS)
	{
		triple.y = x;
	}
	else
	{
		triple.z = x;
	}
		
	if ((parsing == PSO) || (parsing == OSP))
	{
			triple.x = y;
	}
	else if ((parsing == SPO) || (parsing == OPS))
	{
		triple.y = y;
	}
	else
	{
		triple.z = y;
	}
		
	if ((parsing == POS) || (parsing == OPS))
	{
		triple.x = z;
	}
	else if ((parsing == SOP) || (parsing == OSP))
	{
		triple.y = z;
	}
	else
	{
		triple.z = z;
	}
	
	return triple;
}

void
Triples::clustering()
{
	dictionary->beginClustering();
	
	unsigned int new_shared, new_subject, new_object;
	new_shared = 1;
	new_subject = dictionary->getSsubobj() + 1;
	
	if (dictionary->getMapping() == MAPPING1) new_object = dictionary->getNsubjects() + 1;
	else new_object = dictionary->getSsubobj() + 1;
	
	bool repeated = false;
	unsigned int x = 0, y = 0, z = 0;
	TripleID previous;

	for (unsigned int i=0; i<ntriples; i++)
	{		
		if (x == graph[i].x)
		{
			if (y == graph[i].y)
			{
				if (z == graph[i].z)
				{
					repeated = true;
					
					graph[i].x = previous.x; 
					graph[i].y = previous.y; 
					graph[i].z = previous.z;
				}
				else
				{
					z = graph[i].z;
				}
				
				graph[i].y = previous.y;
			}
			else
			{
				y = graph[i].y;
				z = graph[i].z;
				
				// Subject change: ckeck if it clusterized
				unsigned int new_id = dictionary->isClusterized(graph[i].y, VOC_SUBJECT);
				
				if (new_id == 0)
				{
					// It is not previously clusterized
					if (graph[i].y <= dictionary->getSsubobj())
					{
						dictionary->updateID(graph[i].y, new_shared, VOC_SUBJECT);						
						graph[i].y = new_shared;
						new_shared++;
					}
					else
					{				
						dictionary->updateID(graph[i].y, new_subject, VOC_SUBJECT);
						graph[i].y = new_subject;
						new_subject++;
					}
				}
				else
				{
					graph[i].y = new_id;
				}
			}
		}
		else
		{			
			x = graph[i].x;
			y = graph[i].y;
			z = graph[i].z;

			// Subject change: ckeck if it clusterized
			unsigned int new_id = dictionary->isClusterized(graph[i].y, VOC_SUBJECT);
			
			if (new_id == 0)
			{
				if (graph[i].y <= dictionary->getSsubobj())
				{
					dictionary->updateID(graph[i].y, new_shared, VOC_SUBJECT);
					graph[i].y = new_shared;
					new_shared++;
				}
				else
				{
					dictionary->updateID(graph[i].y, new_subject, VOC_SUBJECT);
					graph[i].y = new_subject;
					new_subject++;
				}
			}
			else
			{
				graph[i].y = new_id;
			}
		}
		
		if (repeated == false)
		{		
			// Object change: check if it clusterized
			unsigned int new_id = dictionary->isClusterized(graph[i].z, VOC_OBJECT);
			
			if (new_id == 0)
			{
				if (graph[i].z <= dictionary->getSsubobj())
				{
					dictionary->updateID(graph[i].z, new_shared, VOC_OBJECT);
					graph[i].z = new_shared;
					new_shared++;
				}
				else
				{
					dictionary->updateID(graph[i].z, new_object, VOC_OBJECT);
					graph[i].z = new_object;
					new_object++;
				}
			}
			else
			{
				graph[i].z = new_id;
			}
		}
		else
		{
			 repeated = false;
		}

		previous = graph[i];
	}
		
	dictionary->endClustering();
}

void
Triples::gnuplot(unsigned int npredicates, vector <string> predicates, unsigned int max, string path)
{
	char *xstring = new char[16];
	string filename = path;	
	FILE *data = NULL;
	
	// All Predicates
	filename.append("all.gnu");
	gnuplotHeader(1, npredicates, predicates[0], 1, max, 1, max, filename);
	
	unsigned int ymin = max, ymax = 0, zmin = max, zmax = 0;
	unsigned int x = 0, y = 0, z = 0;
	bool repeated = false;
	
	for (unsigned int i=0; i<ntriples; i++)
	{		
		if (x == graph[i].x)
		{
			if ((y == graph[i].y) && (z == graph[i].z)) repeated = true;
		}
		else
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
			x = graph[i].x, y = graph[i].y, z = graph[i].z;
			
			// Opening data file for this predicate
			filename = path;
			sprintf(xstring,"%d",x);
			filename.append(xstring);
			filename.append(".dat");
			data = fopen(filename.c_str(), "w");
		}
		
		if (!repeated)
		{
			y = graph[i].y;
			z = graph[i].z;
			
			
			if (parsing == PSO)
				fprintf(data, "%d %d\n", z, y);
			else
				fprintf(data, "%d %d\n", y, z);

			if (ymin > y)
			{
				ymin = y; 
			}
			else
			{
				if (ymax < y) ymax = y; 
			}
			
			if (zmin > z) 
			{
				zmin = z; 
			}
			else
			{
				if (zmax < z) zmax = z; 
			}
		}
		else
		{
			repeated = false;
		}
	}
	
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

void 
Triples::graphSort()
{
	sort(graph.begin(),graph.end(), Triples::sortXYZ);
}

unsigned int 
Triples::size()
{
	return ntriples;
}

void
Triples::gnuplotHeader(unsigned int firstP, unsigned int sizeP, string predicate, 
					   unsigned int xmin, unsigned int xmax, unsigned int ymin, unsigned int ymax, string filename)
{	
	FILE *header = fopen(filename.c_str(), "w");
	
	fprintf(header, "set terminal jpeg\n");
	fprintf(header, "set pointsize 0.35\n");
	fprintf(header, "#set logscale\n");
	fprintf(header, "set output \"%s.jpg\"\n", filename.c_str());
	fprintf(header, "set xlabel \"Objects\"\n");
	fprintf(header, "set ylabel \"Subjects\"\n");
	
	if (parsing == PSO)
	{
		fprintf(header, "set xrange [%d:%d]\n", ymin, ymax);
		fprintf(header, "set yrange [%d:%d]\n", xmin, xmax);
	}
	else
	{
		fprintf(header, "set xrange [%d:%d]\n", xmin, xmax);
		fprintf(header, "set yrange [%d:%d]\n", ymin, ymax);		
	}
	
	if (sizeP > 1)
	{	
		fprintf(header, "plot \"1.dat\" notitle pt 7, \\\n");
		
		for (unsigned int i=2; i<=sizeP-1; i++)
			fprintf(header, "\"%d.dat\" notitle pt 7, \\\n", i);
			
		fprintf(header, "\"%d.dat\" notitle pt 7\n", sizeP);
	}
	else
	{
		fprintf(header, "plot \"%d.dat\" title \"%s\"\n", firstP, predicate.c_str());
	}
	
	fclose(header);
}



void
Triples::SPOtoOPS() {
	register int tmp;
	
	if(parsing==SPO) {
		for (int i=0; i<ntriples; i++) {
			tmp = graph[i].x;
			graph[i].x = graph[i].z;
			graph[i].z = tmp;
		}
		parsing=OPS;
		graphSort();
	}
}

void
Triples::PSOtoSPO() {
	register int tmp;
	
	if(parsing==PSO) {
		for (int i=0; i<ntriples; i++) {
			tmp = graph[i].x;
			graph[i].x = graph[i].y;
			graph[i].y = tmp;
		}
		parsing=SPO;
		graphSort();
	}
}

void 
Triples::calculateDegree(string path) {
	const int maxval = 50000;
	const int nbins = 50000;
	
	Histogram hDegree(0, maxval, nbins);
	Histogram hDegreePartial(0, maxval, nbins);
	Histogram hDegreeLabeled(0, maxval, nbins);
	
	int xcount=1,ycount=1,ychanged=1;
	
	int x=graph[0].x;
	int y=graph[0].y;
	int z=graph[0].z;	
	
	//cout << graph[0].x << " " << graph[0].y << " " << graph[0].z << endl;	
	
	for (int i=1; i<ntriples; i++) {
		
		// Ignore duplicate triples
		if( (x==graph[i].x) && (y==graph[i].y) && (z==graph[i].z) ) {
			continue;
		}
		
		if (x != graph[i].x) {
			//cout << "\tdegree: " << xcount <<endl;
			hDegree.Add(xcount);
			
			//cout << "\tpartial degree: " << ycount << endl;
			hDegreePartial.Add(ycount);
			
			//cout << "\tlabeled degree: " << ychanged << endl;
			hDegreeLabeled.Add(ychanged);
			
			xcount=ycount=1;
			ychanged=1;
		} else {
			xcount++;
			
			if (y != graph[i].y) {
				ychanged++;
				
				//cout << "\tpartial degree: " << ycount << endl;
				hDegreePartial.Add(ycount);;
				
				ycount=1;
			} else {
				ycount++;
			}
		}
		
		//cout << graph[i].x << " " << graph[i].y << " " << graph[i].z << endl;
		
		x = graph[i].x;
		y = graph[i].y;
		z = graph[i].z;
	}	
	
	hDegree.end();
	hDegreePartial.end();	
	hDegreeLabeled.end();
	
	ofstream out;
	string direcc = (parsing==SPO) ? "out" : "in";
	
	//	cout << endl << "Degree" << endl;
	out.open((path+"_"+direcc).c_str(), ios::out);
	out << "# " << direcc << " degree" << endl;
	hDegree.dumpStr(out);
	out.close();
	
	//cout << endl << "Partial degree" << endl;
	out.open((path+"_p"+direcc).c_str(), ios::out);
	out << "# Partial " << direcc << " degree" << endl;
	hDegreePartial.dumpStr(out);
	out.close();
	
	//cout << endl << "Labeled degree" << endl;
	out.open((path+"_l"+direcc).c_str(), ios::out);
	out << "# Labeled" << direcc << " degree" << endl;
	hDegreeLabeled.dumpStr(out);
	out.close();
	
}

void
Triples::calculateDegrees(string path) {

	if(parsing != SPO && parsing != PSO) {
		cout << "Degree must be calculated from PSO or SPO parsing style" << endl;
		return;
	}
	
	if(parsing==PSO) {
		PSOtoSPO();
	}
	
	cout << "Calculate OUT Degree" << endl;
	calculateDegree(path);
	
	SPOtoOPS();
	
	cout << "Calculate IN Degree" << endl;
	calculateDegree(path);
		
}

void
Triples::dumpStats(string file) 
{
	ofstream out;
	
	out.open(file.c_str(), ios::out);
	out<<"Triples: "<<ntriples<<endl;
	out.close();
}
