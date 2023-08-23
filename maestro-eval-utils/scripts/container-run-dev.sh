#!/bin/bash

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
VPP_DIR="$SCRIPT_DIR/../"

cd $VPP_DIR
docker-compose build
docker-compose run vpp /vpp/maestro-eval-utils/scripts/run-vpp-dev.sh --rm