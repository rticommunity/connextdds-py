# connextdds-py

A full Python binding for RTI Connext DDS using pybind11

[![Build Status](https://travis-ci.com/rticommunity/connextdds-py.svg?token=BnoLsZyPsyeg3f6xwqc6&branch=master)](https://travis-ci.com/rticommunity/connextdds-py)

## Requirements

- Windows (requires Visual Studio 2015 or newer), macOS, and Linux
- Python 2.7, Python 3.x with pip
- RTI Connext DDS 5.3.1, 6.0.0, 6.0.1

## Installation

Clone repo and submodules and cd to the project root

```shell
$ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
$ cd connextdds-py
```

Set environment variables:

```shell
$ export NDDSHOME=<install location of Connext Pro>
$ export CONNEXTDDS_ARCH=<target arch>
$ export NJOBS=<number of concurrent make jobs or VS build processes, default 1>
```

Build and install

```shell
$ pip install .
```

## Running the tests
To run the tests, first install pytest using pip.

```shell
$ pytest test/
```
If you want to run the performance test, you can run
```shell
$ python3 test/python/perf.py
```
You can add a number after it to indicate how many times to run it.
You can also add either the ```-ind``` flag or the ```-all``` flag
to choose which test you want to run. If you omit the flags, both
will be executed.


## Building for development
To build for development, you must have the `wheel` package installed.
You also need all of the enviornment variables that you need for the
regular install. If you already have an install of any form, run
`pip3 uninstall rti -y`

```shell
$ pip3 install wheel
$ pip3 install setuptools
$ pip3 install cmake
$ python3 setup.py bdist_wheel
$ pip3 install dist/*
```

## Building and viewing the documentation
To build the documentation, run

```shell
$ cd docs
$ make html
```
You must have the development build built and installed for this to work.
To view the documentation, open in your web browser the file located at
`docs/build/html/index.html`

## Hello World Example

```python
import rti.connextdds as dds
import time

participant = dds.DomainParticipant(0)
topic = dds.StringTopicType.Topic(participant, 'example')
writer = dds.StringTopicType.DataWriter(participant.implicit_publisher, topic)

while True:
    writer.write("Hello World!")
    time.sleep(1)
```

## Documentation

The [Connext DDS Python API Reference](https://community.rti.com/static/documentation/connext-dds/current/api/connext_dds/api_python/index.html)
provides getting started instructions, additional examples, an API overview and
an API reference.

## Features

- Python DDS API supporting DynamicData and built-in topic types
- Object-oriented design based on Modern C++ API
- Supports QoS modifications in code and via `QosProvider`.
- Supports listeners
- Supports waitsets and all conditions
- Supports RTI Distributed Logger and integration with Python `logging` module
