
# HDT library libhdt

## Overview

Libhdt is a C++ library that implements the W3C Submission (http://www.w3.org/Submission/2011/03/) of the RDF HDT (Header-Dictionary-Triples) binary format for publishing and exchanging RDF data at large scale. Its compact representation allows storing RDF in fewer space, while providing direct access to the stored information. This is achieved by depicting the RDF graph in terms of three main components: Header, Dictionary and Triples. The Header includes extensible metadata required to describe the RDF data set and details of its internals. The Dictionary organizes the vocabulary of strings present in the RDF graph by assigning numerical IDs to each different string. The Triples component depicts the structure of the underlying graph in a compressed form.

The distribution provides two components:
- C++ library itself: Provides an API to use HDT files programmatically. It allows creating HDT files from RDF and converting HDT files back to RDF. It also provides a Search interface to find triples that match a specific triple pattern.
- Command line tools: Allow to convert between RDF and HDT, and also perform searches against HDT files.

## Command line tools

The tool provides three main command line tools:

- The tool `rdf2hdt` converts an RDF file to HDT format. The format of the input file will be NTriples by default, although it can be set by using the `-f` (format) flag. It is also possible to fine tune the internal format of the HDT file using a configfile. Several configfile examples are provided under the directory "presets" of the distribution.
```
$ rdf2hdt [options] <RDF input file> <HDT output file>
	-h			This help
	-i			Also generate index to solve all triple patterns.
	-c	<configfile>	HDT Config options file
	-o	<options>	HDT Additional options (option1=value1;option2=value2;...)
	-f	<format>	Format of the RDF input (ntriples, nquad, n3, turtle, rdfxml)
	-B	"<base URI>"	Base URI of the dataset.
	-V	Prints the HDT version number.
	-p	Prints a progress indicator.
	-v	Verbose output
```

- The tool `hdt2rdf` converts an HDT file back to RDF in the specified format. If not format specified, NTriples will be used.
```
$ hdt2rdf [options] <HDT input file> <RDF output file>
	-h			This help
	-f	<format>	RDF Format of the output
	-V	Prints the HDT version number.
	-p	Prints a progress indicator.
	-v	Verbose output
```

- The tool `hdtSearch` allows to search triple patterns against an HDT file. For example, to list all patterns, one can use the `? ? ?` query. To search all information about <myns:subject1> one can use `<myns:subject1> ? ?`.
```
$ hdtSearch [options] <hdtfile>
	-h			This help
	-q	<query>		Launch query and exit.
	-o	<output>	Save query output to file.
	-m			Do not show results, just measure query time.
	-V	Prints the HDT version number.
```
Search prompt:
```
>> ? ? ?
............. // List of all triples
```

- The tool `hdtInfo` extracts the header from an HDT file.
```
$ hdtInfo [options] <hdtFile>
	-h			This help
	-o	<output>	Save query output to file.
```

- The tool `replaceHeader` replaces the header of an HDT with the supplied one. You can use hdtInfo to extract the existing one, edit it, and then use replaceHeader to update to the new one. Note: you need to specify a different input and output HDT file.
```
$ replaceHeader <originalHDTfile> <newHDTfile> <newHeader>
```

## Usage examples

- ####  rdf2hdt
This creates the HDT representation:
```bash
$ tools/rdf2hdt data/test.nt data/test.hdt
```
- #### hdt2rdf
This converts back the HDT to RDF:
```bash
$ tools/hdt2rdf data/test.hdt data/test.hdtexport.nt
```
- #### hdtSearch
This allows browsing a dataset interactively. It will create a file data/test.hdt.index with an additional index to perform all kind of queries efficiently. The following times it will use this file:
```bash
$ tools/hdtSearch data/test.hdt
```
An example browsing upon this dataset is:
```
>> ? ? ?
http://example.org/uri3 http://example.org/predicate3 http://example.org/uri4
http://example.org/uri3 http://example.org/predicate3 http://example.org/uri5
http://example.org/uri1 http://example.org/predicate1 "literal1"
http://example.org/uri1 http://example.org/predicate1 "literalA"
http://example.org/uri1 http://example.org/predicate1 "literalB"
http://example.org/uri1 http://example.org/predicate1 "literalC"
http://example.org/uri1 http://example.org/predicate2 http://example.org/uri3
http://example.org/uri1 http://example.org/predicate2 http://example.org/uriA3
http://example.org/uri2 http://example.org/predicate1 "literal1"
9 results shown.
>> http://example.org/uri3 ? ?
http://example.org/uri3 http://example.org/predicate3 http://example.org/uri4
http://example.org/uri3 http://example.org/predicate3 http://example.org/uri5
2 results shown.
>> exit
```
You can also dump the results of a query to a file:
```
$ tools/hdtSearch -q "? http://example.org/predicate1 ?" data/test.hdt > output.txt
```
- #### hdtInfo
To browse the header of an HDT file:
```
$ tools/hdtInfo data/test.hdt
```
- #### replaceHeader
To replace the Header of an HDT file with a new one. For example by editing the existing one as extracted using hdtInfo:
```
$ tools/replaceHeader data/test.hdt data/testOutput.hdt newHeader.nt
```

## Using the Library

This section shows how to use the HDT Library in your own application.

```c++
// Creating an HDT file (examples/generate.cpp)

#include <HDTManager.hpp>

using namespace hdt;

int main(int argc, char *argv[]) {

	HDTSpecification spec;

	// Read RDF into an HDT file.
 	HDT *hdt = HDTManager::generateHDT("data/test.nt", "http://example.org/test", NTRIPLES, spec);

	// Add additional domain-specific properties to the header
	Header *header = hdt->getHeader();
	header->insert("myResource1", "property", "value");

	// Save HDT to a file
	hdt->saveToHDT("data/test.hdt");

	delete hdt;
}
```

```c++
// Load an HDT file and query (examples/search.cpp)

#include <iostream>
#include <HDTManager.hpp>

using namespace std;
using namespace hdt;

int main(int argc, char *argv[]) {

        // Load HDT file
        HDT *hdt = HDTManager::mapHDT("data/test.hdt");

        // Enumerate all triples matching a pattern ("" means any)
        IteratorTripleString *it = hdt->search("http://example.org/uri3","","");
        while(it->hasNext()){
                TripleString *triple = it->next();
                cout << "Result: " << triple->getSubject() << ", " << triple->getPredicate() << ", " << triple->getObject() << endl;
        }
        delete it;

        // Enumerate all different predicates
        cout << "Dataset contains " << hdt->getDictionary()->getNpredicates() << " predicates.";
        IteratorUCharString *itPred = hdt->getDictionary()->getPredicates();
        while(itPred->hasNext()) {
                unsigned char *str = itPred->next(); // Warning this pointer is only valid until next call to next();
                cout << str << endl;
                itPred->freeStr(str);
        }
        delete itPred;

        delete hdt;
}
```

## License

Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
All rights reserved.

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Visit our Web Page: http://www.rdfhdt.org

## Contacting the authors:

- Mario Arias:               mario.arias@deri.org

- Javier D. Fernandez:       jfergar@infor.uva.es

- Miguel A. Martinez-Prieto: migumar2@infor.uva.es
