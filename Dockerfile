FROM ubuntu:18.04

RUN apt update --fix-missing
RUN apt install -y \
    iproute2 \
    git \
    make \
    sudo \
    vim \
    pciutils \
    net-tools \
    python2.7 \
    python3

WORKDIR /vpp
COPY . .

RUN yes | make install-deps
RUN make build-release

VOLUME maestro-eval-utils

SHELL [ "/bin/bash" ]
CMD [ "/bin/bash" ]