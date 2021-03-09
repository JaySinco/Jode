#!/bin/bash

source "../../../source_in_bash_profile.sh"
cwd=`readlink -f .`

cd node-addon-api-3.1.0 \
&& \
mkdir -p ../../include \
&& \
cp *.h ../../include/ \
