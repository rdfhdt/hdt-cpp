# C++ library for the HDT triple format
HDT keeps big RDF datasets compressed while maintaining search and browse operations.

To compile the library run `make` under the dir hdt-lib, this will generate the library and tools.

The implementation can have the following optional dependencies:
- [Raptor RDF Parser Library Version 2+](http://librdf.org/raptor/) (optional) This allows importing RDF in many serialization formats, i.e RDF/XML, Turtle, N3. etc. To activate uncomment the line `USE_RAPTOR=true` in the Makefile. If raptor is not used, the library will only be able to load RDF in NTriples format.
- [libz](http://www.zlib.net/) (optional) Allows to load files in ntriples compressed with GZIP (i.e. file.nt.gz) and gzipped HDTs (file.hdt.gz). To activate uncomment the line `USE_LIBZ=true` in the Makefile.
- [Kyoto Cabinet](http://fallabs.com/kyotocabinet/) (optional) Allows generating big RDF Datasets on machines without much RAM Memory, by creating a temporary Kyoto Cabinet Database. To activate uncomment the line `USE_KYOTO=true` in the Makefile and edit the includepath / librarypath.

After compiling, these are the typical operations that you will perform:
Create the HDT representation of your RDF Data:

    $ tools/rdf2hdt data/test.nt data/test.hdt

Convert an HDT to another serialization format, such as NTriples:

    $ tools/hdt2rdf data/test.hdt data/test.hdtexport.nt

Open a terminal to search triple patterns within an HDT file:

    $ tools/hdtSearch data/test.hdt
    >> ? ? ?
    http://example.org/uri3 http://example.org/predicate3 http://example.org/uri4
    http://example.org/uri3 http://example.org/predicate3 http://example.org/uri5
    http://example.org/uri4 http://example.org/predicate4 http://example.org/uri5
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

Extract the Header of an HDT file:

    $ tools/hdtInfo data/test.hdt > header.nt

Replace the Header of an HDT file with a new one. For example by editing the existing one as extracted using `hdtInfo`

    $ tools/replaceHeader data/test.hdt data/testOutput.hdt newHeader.nt
