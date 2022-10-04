#!/bin/bash
set -e

SOURCE_DIR=${PWD}/connextdds
BUILD_DIR=${PWD}/connextdds-build
RTI_ARCH=$1

rm -rf ${PWD}/connextdds-build
pip install -r ${PWD}/connextdds/resource.3.0/python/pip/build-requirements.txt
conan config install https://repo.rti.com/artifactory/thirdparty-local/rti/conan-config.zip
conan install --profile ${RTI_ARCH} ${SOURCE_DIR} -if ${BUILD_DIR} --build missing
cmake -S${SOURCE_DIR} -B${BUILD_DIR} -DCMAKE_INSTALL_PREFIX=${PWD}/stage \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
  -DCONNEXTDDS_SUPPORT_ADMINCONSOLE=OFF -DCONNEXTDDS_SUPPORT_WEB_INTEGRATION_SERVICE=OFF \
  -DCONNEXTDDS_SUPPORT_CONNECTOR=OFF  \
  -DCONNEXTDDS_BUILD_TESTS=OFF -DCONNEXTDDS_SUPPORT_ROUTING_SERVICE=OFF \
  -DPLATFORM_SUPPORT_DOTNET_2=FALSE -DCONNEXTDDS_SUPPORT_JAVA_API=FALSE \
  -DCONNEXTDDS_SUPPORT_MONITOR_UI=OFF -DCONNEXTDDS_SUPPORT_PERSISTENCE_SERVICE=OFF \
  -DCONNEXTDDS_SUPPORT_XML_UTILS=OFF -DCONNEXTDDS_SUPPORT_LAUNCHER=OFF
cmake --build ${BUILD_DIR} --target install -j 8
