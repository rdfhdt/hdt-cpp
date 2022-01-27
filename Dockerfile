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
RUN ./autogen.sh && ./configure
RUN make -j4
RUN make install

FROM debian:bullseye-slim

# Install runtime dependencies
RUN apt update; \
    apt install -y --no-install-recommends \
        libserd-0-0 \
    ; \
    rm -rf /var/lib/apt/lists/*;

# Copy in libraries and binaries from build stage.
COPY --from=build \
        /usr/local/lib/libcds* \
        /usr/local/lib/libhdt* \
        /usr/local/lib/
COPY --from=build \
        /usr/local/lib64/libstdc++.* \
        /usr/local/lib64/
COPY --from=build \
        /usr/local/bin/hdt2rdf \
        /usr/local/bin/hdtInfo \
        /usr/local/bin/hdtSearch \
        /usr/local/bin/modifyHeader \
        /usr/local/bin/rdf2hdt \
        /usr/local/bin/replaceHeader \
        /usr/local/bin/searchHeader \
        /usr/local/bin/

# Add /usr/local/lib to LD_LIBRARY_PATH.
ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:$LD_LIBRARY_PATH

CMD ["/bin/echo", "Available commands: rdf2hdt hdt2rdf hdtInfo hdtSearch modifyHeader replaceHeader searchHeader"]
