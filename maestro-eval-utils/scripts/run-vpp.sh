#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)

VPP_DIR="$SCRIPT_DIR/../../"

VPP_BIN="$VPP_DIR/build-root/install-vpp-native/vpp/bin/vpp"
STARTUP_CONF="$SCRIPT_DIR/../vpp-startup.conf"

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