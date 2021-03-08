#!/bin/bash

source "../../../source_in_bash_profile.sh"
cwd=`readlink -f .`

mkdir -p ${CMAKE_OUT_DIR}/ \
&& \
cd ${CMAKE_OUT_DIR} \
&& \
${CMAKE_CMD} "${CMAKE_GENERATOR}" ../gflags-2.2.2 \
    -DCMAKE_INSTALL_PREFIX="${cwd}/../" \
&& \
${MSVC_BUILD} && ${MSVC_INSTALL}
