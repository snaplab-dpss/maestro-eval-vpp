#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
VPP_DIR="$SCRIPT_DIR/../"

cd $VPP_DIR

container_id=$(docker ps | grep "maestro-eval-vpp_vpp" | awk '{ print $1 }')
docker stop $container_id