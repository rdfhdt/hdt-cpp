FROM gcc:bullseye as build

# Install dependencies
RUN apt-get update && apt-get -y install \
	build-essential \
	libraptor2-dev \
	libserd-dev \
	autoconf \
	libtool \
	liblzma-dev \
	liblzo2-dev \
	zlib1g-dev

WORKDIR /usr/local/src/hdt-cpp
COPY . .

# Install HDT tools
RUN cd hdt-cpp && ./autogen.sh && ./configure && make -j2

# Expose binaries
ENV PATH /usr/local/src/hdt-cpp/libhdt/tools:$PATH

# reset WORKDIR
WORKDIR /

# Default command
CMD ["/bin/echo", "Available commands: rdf2hdt hdt2rdf hdtSearch"]

