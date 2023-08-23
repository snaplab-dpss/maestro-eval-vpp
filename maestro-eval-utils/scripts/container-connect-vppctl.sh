#!/bin/bash

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
VPP_DIR="$SCRIPT_DIR/../"

cd $VPP_DIR
docker-compose run vpp /home/user/vpp/build-root/build-vpp-native/vpp/bin/vppctl