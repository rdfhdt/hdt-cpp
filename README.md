[![Join the chat at https://gitter.im/rdfhdt](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rdfhdt)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.580298.svg)](https://doi.org/10.5281/zenodo.580298)

# C++ implementation of the HDT compression format

Header Dictionary Triples (HDT) is a compression format for RDF data
that can also be queried for Triple Patterns.

## Getting Started

### Prerequisites

In order to compile this library, you need to have the following
dependencies installed:

- [GNU Autoconf](https://www.gnu.org/software/autoconf/autoconf.html)

  - `sudo apt install autoconf` on Debian-based distros (e.g., Ubuntu)
  - `sudo dnf install autoconf` on Red Hat-based distros (e.g.,
    Fedora)

- [GNU Libtool](https://www.gnu.org/software/libtool/)

  - `sudo apt install libtool` on Debian-based distros (e.g., Ubuntu)
  - `sudo dnf install libtool` on Red Hat-based distros (e.g., Fedora)

- [GNU zip (gzip)](http://www.zlib.net/) Allows GNU zipped RDF input
  files to be ingested, and allows GNU zipped HDT files to be loaded.

  - `sudo apt install zlib1g zlib1g-dev` on Debian-based distros (e.g., Ubuntu)
  - `sudo dnf install gzip` on Red Hat-based distros (e.g., Fedora)

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
  A helper tool for compiling applications and libraries.

  - `sudo apt install pkg-config` on Debian-based distros (e.g.,
    Ubuntu)
  - `sudo dnf install pkgconf-pkg-config` on Red Hat-based distros
    (e.g., Fedora)

- [Serd v0.28+](https://github.com/drobilla/serd) The default parser
  that is used to process RDF input files.  It supports the N-Quads,
  N-Triples, TriG, and Turtle serialization formats.

  - `sudo apt install libserd-0-0 libserd-dev` on Debian-based distros
    (e.g., Ubuntu)
  - `sudo dnf install serd serd-devel` on Red Hat-based distros (e.g.,
    Fedora)
    
  Sometimes the version of Serd that is distributed by package managers
  is too old.  In that case, Serd can also be built manually: see
  https://github.com/drobilla/serd for the installation instructions.

### Installation

To compile and install, run the following commands under the directory
`hdt-cpp`.  This will also compile and install some handy tools.

```
./autogen.sh
./configure
make -j2
sudo make install
```

### Installation issues

Sometimes, the above instructions do not result in a working HDT
installation.  This section enumerates common issues and their
workaround.

#### Compilation issues using Kyoto Cabinet

The support for Kyoto Cabinet was never finished and is currently suspended. It is for the time being not possible to compile HDT with KyotoCabinet.

Common error: 
  
    In file included from src/dictionary/KyotoDictionary.cpp:38:0:
    src/dictionary/KyotoDictionary.hpp:108:18: error: conflicting return type specified for 'virtual unsigned int hdt::KyotoDictionary::getMapping()'
     unsigned int getMapping();
                  ^

#### Package requirements (serd-0 >= 0.28.0) were not met
When getting 

    Package requirements (serd-0 >= 0.28.0) were not met: Requested 'serd-0 >= 0.28.0' but version of Serd is 0.X

Serd is not 0.28+, probably because of the package manager. Built it manually at https://github.com/drobilla/serd.

#### `./configure` cannot find Serd

While running `./configure` you get a message similar to the
following:

```
Package 'serd-0', required by 'virtual:world', not found
```

This means that `./configure` cannot find the location of the
`serd-0.pc` file on your computer.  You have to find this location
yourself, e.g., in the following way:

```sh
find /usr/ -name serd-0.pc
```

Once you have found the directory containing the `serd-0.pc` file, you
have to inform the `./configure` script about this location by setting
the following environment variable (where directory
`/usr/local/lib/pkgconfig/` is adapted to your situation):

```sh
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
```

## Using HDT

After compiling and installing, you can use the handy tools that are
located in `hdt-cpp/libhdt/tools`.  We show some common tasks that can
be performed with these tools.

### RDF-2-HDT: Creating an HDT

HDT files can only be created for standards-compliant RDF input files.
If your input file is not standards-compliant RDF, it is not possible
to create an HDT files out of it.

```
$ ./rdf2hdt data.nt data.hdt
```

### HDT-2-RDF: Exporting an HDT

You can export an HDT file to an RDF file in one of the supported
serialization formats (currently: N-Quads, N-Triples, TriG, and
Turtle).  The default serialization format for exporting is N-Triples.

```
$ ./hdt2rdf data.hdt data.nt
```

### Querying for Triple Patterns

You can issue Triple Pattern (TP) queries in the terminal by
specifying a subject, predicate, and/or object term.  The questions
mark (`?`) denotes an uninstantiated term.  For example, you can
retrieve _all_ the triples by querying for the TP `? ? ?`:

    $ ./hdtSearch data.hdt
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

### Exporting the header

The header component of an HDT contains metadata describing the data
contained in the HDT, as well as the creation metadata about the HDT
itself.  The contents of the header can be exported to an N-Triples
file:

```
$ ./hdtInfo data.hdt > header.nt
```

### Replacing the Header

It can be useful to update the header information of an HDT.  This can
be done by generating a new HDT file (`new.hdt`) out of an existing
HDT file (`old.hdt`) and an N-Triples file (`new-header.nt`) that
contains the new header information:

```
$ ./replaceHeader old.hdt new.hdt new-header.nt
```

## Contributing

Contributions are welcome!  Please base your contributions and pull
requests (PRs) on the `develop` branch, and not on the `master`
branch.

## License

`hdt-cpp` is free software licensed as GNU Lesser General Public
License (GPL). See `libhdt/COPYRIGHT`.
