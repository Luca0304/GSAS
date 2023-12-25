#!/bin/bash
set -o errexit
set -o pipefail
trap - INT TERM

LOCAL_UID=$(id -u $USER) LOCAL_GID=$(id -g $USER) docker-compose run cdnify