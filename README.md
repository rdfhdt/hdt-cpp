[![Join the chat at https://gitter.im/rdfhdt](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rdfhdt)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.580298.svg)](https://doi.org/10.5281/zenodo.580298)

# C++ library for the HDT triple format
HDT keeps big RDF datasets compressed while maintaining efficient search and browse operations.

To compile the library run `make` under the directory `hdt-lib`, this will generate the library and tools.

The implementation has the following dependencies:
- [Serd](http://drobilla.net/software/serd/) This enables importing RDF data in the Turtle and N-Triples serialization formats specifically. The dependency is activated by default; to deactivate it, comment out the line `SERD_SUPPORT=true` in the `Makefile`.
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
