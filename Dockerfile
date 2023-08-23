FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

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

RUN useradd -m user
RUN echo "user:user" | chpasswd
RUN adduser user sudo

RUN groupadd vpp
RUN adduser user vpp

# Ensure sudo group users are not asked for a password when using 
# sudo command by ammending sudoers file.
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER user

WORKDIR /home/user/vpp
COPY --chown=user:vpp . .

# Circumvent dubious ownership of git
RUN git config --global --add safe.directory /home/user/vpp

# Add upstream version (the build script looks for tags)
RUN git remote add upstream https://github.com/FDio/vpp.git
RUN git fetch upstream

RUN yes | make install-deps
RUN make build-release

SHELL [ "/bin/bash" ]
CMD [ "/bin/bash" ]