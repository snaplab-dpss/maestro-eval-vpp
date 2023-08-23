#!/bin/bash

SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
VPP_DIR="$SCRIPT_DIR/../"

cd $VPP_DIR
DOCKER_BUILDKIT=1 docker-compose build
docker-compose run vpp /home/user/vpp/maestro-eval-utils/scripts/run-vpp.sh