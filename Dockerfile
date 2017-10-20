FROM gcc:6

WORKDIR /usr/local/src
COPY . /usr/local/src/hdt-cpp/

# Install dependencies
RUN apt-get update && apt-get -y install \
	build-essential \
	libraptor2-dev \
	#libserd-dev \
	liblzma-dev \
	liblzo2-dev \
	zlib1g-dev


# Install more recent serd
RUN wget https://github.com/drobilla/serd/archive/v0.28.0.tar.gz \
	&& tar -xvzf *.tar.gz \
	&& rm *.tar.gz \
	&& cd serd-* \
	&& ./waf configure && ./waf && ./waf install

# Install HDT tools
RUN cd hdt-cpp/hdt-lib && make

# Expose binaries
ENV PATH /usr/local/src/hdt-cpp/hdt-lib/tools:$PATH

# reset WORKDIR
WORKDIR /

# Default command
CMD ["/bin/echo", "Available commands: rdf2hdt hdt2rdf hdtSearch"]

