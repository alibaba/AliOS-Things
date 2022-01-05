FROM obgm/libcoap:build-env

ENV libcoap_dir=/libcoap
ADD . $libcoap_dir
WORKDIR $libcoap_dir

RUN ./autogen.sh --clean && ./autogen.sh
