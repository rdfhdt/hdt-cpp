FROM gcc:6


# Install dependencies
RUN apt-get update && apt-get -y install \
    git \
	build-essential \
	libraptor2-dev \
	#libserd-dev \
	autoconf \
	libtool \
	liblzma-dev \
	liblzo2-dev \
	zlib1g-dev


WORKDIR /usr/local/src

ARG SERD_COMMIT=v0.30.10

# Install more recent serd
RUN git clone --depth 1 --branch $SERD_COMMIT https://github.com/drobilla/serd.git \
    && cd serd/ \
    && git submodule update --init --recursive \
    && ./waf configure && ./waf && ./waf install \
    && cd .. \
    && rm -rf serd

# get sources
COPY . /usr/local/src/hdt-cpp/

# Install HDT tools
RUN cd hdt-cpp && ./autogen.sh && ./configure && make -j2

# Expose binaries
ENV PATH /usr/local/src/hdt-cpp/libhdt/tools:$PATH

# reset WORKDIR
WORKDIR /workdir

# Default command
CMD ["/bin/echo", "Available commands: rdf2hdt hdt2rdf hdtSearch"]
