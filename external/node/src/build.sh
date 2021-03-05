#!/bin/bash

source "../../../source_in_bash_profile.sh"
cwd=`readlink -f .`

cd node-v12.20.1
vcbuild.bat release static x64 vs2019
