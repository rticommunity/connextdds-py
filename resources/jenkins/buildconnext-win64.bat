SET SOURCE_DIR=%cd%\connextdds
SET BUILD_DIR=%cd%\connextdds-build

pip install -r %cd%\connextdds\resource.3.0\python\pip\build-requirements.txt
conan config install https://repo.rti.com/artifactory/thirdparty-local/rti/conan-config.zip
CALL C:\\buildtools\\Common7\\tools\\vsdevcmd.bat -arch=x64

conan install --profile x64Win64VS2017 %SOURCE_DIR% -if %BUILD_DIR% --build missing -u
cmake -S %SOURCE_DIR% -B%BUILD_DIR% -DCMAKE_INSTALL_PREFIX=%cd%\\stage -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCONNEXTDDS_SUPPORT_ADMINCONSOLE=OFF -DCONNEXTDDS_SUPPORT_WEB_INTEGRATION_SERVICE=OFF -DCONNEXTDDS_SUPPORT_CONNECTOR=OFF  -DCONNEXTDDS_BUILD_TESTS=OFF -DCONNEXTDDS_SUPPORT_ROUTING_SERVICE=OFF -DPLATFORM_SUPPORT_DOTNET_2=FALSE -DCONNEXTDDS_SUPPORT_JAVA_API=FALSE -DCONNEXTDDS_SUPPORT_MONITOR_UI=OFF -DCONNEXTDDS_SUPPORT_PERSISTENCE_SERVICE=OFF -DCONNEXTDDS_SUPPORT_XML_UTILS=OFF -DCONNEXTDDS_SUPPORT_LAUNCHER=OFF
cmake --build %BUILD_DIR% --config Release --target install -j 8

rmdir /q /s %SOURCE_DIR%
rmdir /q /s %BUILD_DIR%

python configure.py -j 8 -n stage\\rti_connext_dds-7.1.0 AMD64Windows