FROM debian:testing-slim

RUN apt-get update && apt-get install -y autoconf automake gcc clang \
  libtool libtool-bin make pkg-config libcunit1-dev libssl-dev \
  libgnutls28-dev exuberant-ctags git valgrind \
  graphviz doxygen libxml2-utils xsltproc docbook-xml docbook-xsl asciidoc
RUN apt-get clean
