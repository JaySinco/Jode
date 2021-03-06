#!/bin/bash

source "./source_in_bash_profile.sh"
cwd=`readlink -f .`
cloc="${cwd}/node_modules/.bin/cloc.exe"
fmt="${cwd}/node_modules/.bin/clang-format.exe"

case $1 in
"")
    target=""
    ${cloc} --quiet src lib
    ;;
clean)
    rm -rf out/ bin/
    exit 0
    ;;
*)
    target="-t:$1"
    ;;
esac

yarn run eslint lib/ --config ${cwd}/lib/config/.eslintrc.json --fix \
&& \
find src/ lib/ -type f ! -path '*.json' -exec ${fmt} -i {} \; \
&& \
mkdir -p ${CMAKE_OUT_DIR}/ bin/ \
&& \
cd ${CMAKE_OUT_DIR}/ \
&& \
msbuild_type=`echo ${MSVC_BUILD_TYPE} | tr '[:lower:]' '[:upper:]'` \
&& \
${CMAKE_CMD} "${CMAKE_GENERATOR}" ../ \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_${msbuild_type}=${cwd}/bin/ \
&& \
${MSVC_BUILD} ${target}
