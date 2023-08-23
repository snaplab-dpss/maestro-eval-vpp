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
# COPY --chown=user:vpp . .

# Circumvent dubious ownership of git
# RUN git config --global --add safe.directory /home/user/vpp

# RUN yes | make install-deps
# RUN make build-release

# RUN touch maestro-eval-utils/vpp.log
# RUN chown user:vpp maestro-eval-utils/vpp.log
# VOLUME maestro-eval-utils/vpp.log

SHELL [ "/bin/bash" ]
# CMD [ "./maestro-eval-utils/scripts/run-vpp.sh" ]
CMD [ "/bin/bash" ]