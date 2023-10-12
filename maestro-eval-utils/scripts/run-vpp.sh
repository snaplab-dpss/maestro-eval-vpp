#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)

VPP_DIR="$SCRIPT_DIR/../../"

VPP_BIN="$VPP_DIR/build-root/install-vpp-native/vpp/bin/vpp"
STARTUP_CONF="$SCRIPT_DIR/../vpp-startup.conf"

if [ ! -z "$1" ]; then
    # E.g.: "16,17,18,19"
    cores="$1"
    num_cores=$(python3 -c "print(len('$cores'.split(',')))")

    sudo sed -i "s/corelist-workers.*/corelist-workers $cores/g" $STARTUP_CONF
    sudo sed -i "s/num-rx-queues.*/num-rx-queues $num_cores/g"   $STARTUP_CONF
    sudo sed -i "s/num-tx-queues.*/num-tx-queues $num_cores/g"   $STARTUP_CONF
fi

build() {
    pushd $VPP_DIR >/dev/null
        make build-release
    popd
}

run() {
    pushd $SCRIPT_DIR/../ >/dev/null
        sudo $VPP_BIN -c $STARTUP_CONF
    popd

}

build
run