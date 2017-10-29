[![Join the chat at https://gitter.im/rdfhdt](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rdfhdt)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.580298.svg)](https://doi.org/10.5281/zenodo.580298)

# C++ library for the HDT triple format

HDT keeps big RDF datasets compressed while maintaining efficient search and browse operations.

## Getting Started
### Prerequisites

The implementation has the following dependencies:
- [Serd v0.28+](http://drobilla.net/software/serd/) This enables importing RDF data in the Turtle and N-Triples serialization formats specifically. The dependency is activated by default.
- [libz](http://www.zlib.net/) Enables loading N-Triples files compressed with GZIP (e.g., `file.nt.gz`) and gzipped HDTs (`file.hdt.gz`). The dependency is activated by default.
- [Kyoto Cabinet](http://fallabs.com/kyotocabinet/) (optional) Enables generating big RDF datasets on machines without much RAM memory, by creating a temporary Kyoto Cabinet database. The dependency is deactivated by default; to activate it, call `configure` with `--with-kyoto=yes` flag during installation.

The installation process has the following dependencies:

- [autoconf](https://www.gnu.org/software/autoconf/autoconf.html)
- [libtool](https://www.gnu.org/software/libtool/)

The following commands should install both packages:

    sudo apt-get update
    sudo apt-get install autoconf libtool

### Installing

To compile and install, run the following commands under the directory `hdt-cpp`. This will generate the library and tools.

First run the following script to generate all necessary installation files with autotools:

    ./autogen.sh

Then, run:

    ./configure
    make -j2

## Running

After building, these are the typical operations that you will perform:

- Convert your RDF data to HDT:

    $ tools/rdf2hdt data/test.nt data/test.hdt

NB: the input stream is assumed to be valid RDF, so you should validate your data before feeding it into rdf2hdt.

- Convert an HDT to another RDF serialization format, such as N-Triples:

    $ tools/hdt2rdf data/test.hdt data/test.hdtexport.nt

- Open a terminal to search triple patterns within an HDT file:

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

- Extract the Header of an HDT file:

    $ tools/hdtInfo data/test.hdt > header.nt

- Replace the Header of an HDT file with a new one. For example, by editing the existing one as extracted using `hdtInfo`:

    $ tools/replaceHeader data/test.hdt data/testOutput.hdt newHeader.nt

## Contributing

Contributions and PRs should be sent to the `develop` branch, and not to `master`.

## License

`hdt-cpp` is free software licensed as GNU Affero General Public License, either version 2 or (at your option) any later version.
