# C++ library for the HDT triple format
HDT keeps big RDF datasets compressed while maintaining efficient search and browse operations.

To compile the library run `make` under the directory `hdt-lib`, this will generate the library and tools.

The implementation has the following optional dependencies:
- [Raptor RDF Parser Library 2.x](http://librdf.org/raptor/) (optional) This enables importing RDF data in many serialization formats, e.g., RDF/XML, Turtle, N3, etc. The dependency is activated by default; to deactivate it, comment out the line `RAPTOR_SUPPORT=true` in the `Makefile`. If Raptor or Serd is not used, the library will only be able to load RDF data in N-Triples format.
- [Serd](http://drobilla.net/software/serd/) (optional) This enables importing RDF data in the Turtle and N-Triples serialization formats specifically. The dependency is activated by default; to deactivate it, comment out the line `SERD_SUPPORT=true` in the `Makefile`. If Raptor or Serd is not used, the library will only be able to load RDF data in N-Triples format.
- [libz](http://www.zlib.net/) (optional) Enables loading N-Triples files compressed with GZIP (e.g., `file.nt.gz`) and gzipped HDTs (`file.hdt.gz`). The dependency is activated by default; to deactivate it, comment out the line `LIBZ_SUPPORT=true` in the `Makefile`.
- [Kyoto Cabinet](http://fallabs.com/kyotocabinet/) (optional) Enables generating big RDF datasets on machines without much RAM memory, by creating a temporary Kyoto Cabinet database. The dependency is deactivated by default; to activate it, uncomment the line `KYOTO_SUPPORT=true` in the `Makefile` and edit the library include path (`INCLUDES=`) as needed.

After building, these are the typical operations that you will perform:
Create the HDT representation of your RDF data:

    $ tools/rdf2hdt data/test.nt data/test.hdt

Convert an HDT to another RDF serialization format, such as N-Triples:

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

Replace the Header of an HDT file with a new one. For example, by editing the existing one as extracted using `hdtInfo`:

    $ tools/replaceHeader data/test.hdt data/testOutput.hdt newHeader.nt
