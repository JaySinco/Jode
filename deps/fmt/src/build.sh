#!/bin/bash

source "../../../source_in_bash_profile.sh"
cwd=`readlink -f .`

mkdir -p ${CMAKE_OUT_DIR}/ \
&& \
cd ${CMAKE_OUT_DIR} \
&& \
${CMAKE_CMD} "${CMAKE_GENERATOR}" ../fmt-7.1.3 \
    -DFMT_TEST=off \
    -DFMT_DOC=off \
    -DCMAKE_INSTALL_PREFIX="${cwd}/../" \
&& \
${MSVC_BUILD} && ${MSVC_INSTALL}
