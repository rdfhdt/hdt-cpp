FROM gcc:bullseye as build

# Install build dependencies
RUN apt update; \
    apt install -y --no-install-recommends \
	    autoconf \
		build-essential \
	    liblzma-dev \
	    liblzo2-dev \
	    libraptor2-dev \
	    libserd-dev \
	    libtool \
	    zlib1g-dev \
    ; \
    rm -rf /var/lib/apt/lists/*;

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

