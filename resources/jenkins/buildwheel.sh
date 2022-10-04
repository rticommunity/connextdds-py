#!/bin/bash

python3 -m venv .venv
. .venv/bin/activate
pip install cibuildwheel

cibuildwheel --platform linux --archs auto64 --config-file resources/jenkins/config.toml
