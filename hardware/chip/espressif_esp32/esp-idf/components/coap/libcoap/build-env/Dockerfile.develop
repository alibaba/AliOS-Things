FROM obgm/libcoap:build-env

RUN apt-get update && apt-get install -y cmake git g++
RUN apt-get clean

ENV libcoap_dir=/home/libcoap
ADD . $libcoap_dir
WORKDIR $libcoap_dir

RUN ./autogen.sh --clean && ./autogen.sh
RUN ./configure --disable-documentation --enable-tests --enable-examples --with-openssl && make install clean

WORKDIR /home
RUN git clone --depth 1 https://github.com/cabo/cn-cbor.git && cd cn-cbor && ./build.sh all doc install

# The image for development with libcoap
FROM debian:testing-slim

RUN apt-get update && apt-get install -y autoconf automake gcc g++ gdb libtool libtool-bin make \
 pkg-config libssl-dev libgnutls28-dev
RUN apt-get install -y iproute2 lsof net-tools inetutils-ping netcat-openbsd less vim
RUN apt-get clean

COPY --from=0 /usr/local/include/coap2 /usr/local/include/coap2
COPY --from=0 /usr/local/lib /usr/local/lib
COPY --from=0 /usr/local/bin/coap-client /usr/local/bin/
COPY --from=0 /usr/local/include/cn-cbor /usr/local/include/cn-cbor
RUN echo "/usr/local/lib" >>/etc/ld.so.conf.d/usr_local.conf && ldconfig /usr/local/lib

ARG user=user
RUN adduser --disabled-password --gecos '' $user
RUN chown -R $user:$user /home/$user
WORKDIR /home/$user
USER $user

EXPOSE 5683 5684
