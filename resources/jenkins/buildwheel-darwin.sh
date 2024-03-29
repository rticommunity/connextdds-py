#!/bin/bash

/opt/tools/python/3.6/bin/python3 -m venv .venv
. .venv/bin/activate

pip install --upgrade pip
pip install cibuildwheel cmake pybind11==2.9.0

cibuildwheel --platform macos --archs auto64 --config-file resources/jenkins/config.toml
