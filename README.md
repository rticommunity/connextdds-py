# connextdds-py

A full Python binding for RTI Connext DDS using pybind11

## Documentation

The [Connext DDS Python API Reference](https://community.rti.com/static/documentation/connext-dds/current/api/connext_dds/api_python/index.html)
provides getting started instructions, additional examples, an API overview and
an API reference.

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

## Feature highlights

- Python-friendly design built on the Modern C++ API.
- Supports DynamicData and the built-in types.
- Supports DDS Entities in code and in XML.
- Supports QoS Policies in code and in XML.
- Supports Content Filters with the ability to change the filter.
- Supports the built-in discovery Topics.
- Supports all status updates.
- Supports listeners, WaitSets and all Conditions
- Supports RTI Distributed Logger and integration with Python `logging` module
- Supports pluggable RTI components, such as Monitoring and DDS Secure.

## Requirements

- Windows (requires Visual Studio 2015 or newer), macOS, and Linux
- Python 2.7, Python 3.x with pip
- RTI Connext DDS 5.3.1, 6.0.0, 6.0.1
- [patchelf](https://github.com/NixOS/patchelf) (Python 2.7.x on Linux)

## Configuration

Before building/installing connextdds-py, it is necessary to run the configuration.py
script.

```shell
$ python configure.py [options...] platform
```

Where "platform" specifies the RTI architecture/platform string and the options are
specified in the following table.

| Short Option | Long Option            | Description                                                                          |
| ------------ | ---------------------- | ------------------------------------------------------------------------------------ |
| -n NDDSHOME  | --nddshome NDDSHOME    | NDDSHOME directory. Defaults to NDDSHOME environment variable                        |
| -j JOBS      | --jobs JOBS            | Number of concurrent build jobs/processes                                            |
| -t           | --tcp                  | Add the TCP transport plugin                                                         |
| -m           | --monitoring           | Add the RTI Monitoring plugin                                                        |
| -s           | --secure               | Add the RTI DDS Secure plugin + openssl libraries                                    |
| -p PLUGIN    | --plugin PLUGIN        | Add a user-defined plugin. This option can be specified multiple times               |
| -o OPENSSL   | --openssl OPENSSL      | Location of openssl libraries (defaults to platform library location under NDDSHOME) |
| -r DIR       | --python-root DIR      | Root directory of Python (prefers 3.x over 2.x if both are under root)               |
| -c OPENSSL   | --cmake-toolchain FILE | CMake toolchain file to use when cross compiling                                     |
| -d           | --debug                | Use debug libraries and build debug modules for connext-py                           |
| -h           | --help                 | show help message and exit                                                           |

## Installation

Clone repo and submodules and cd to the project root

```shell
$ git clone https://github.com/rticommunity/connextdds-py.git
$ cd connextdds-py
```

Run configuration script:

```shell
$ python configure.py [options...] <platform>
```

Build and install

```shell
$ pip install .
```

## Building for development
To build for development, you must have the `wheel` package installed.
You also need all of the enviornment variables that you need for the
regular install. If you already have an install of any form, run
`pip3 uninstall rti -y`

```shell
$ pip3 install wheel
$ pip3 install setuptools
$ pip3 install cmake
$ pip3 install patchelf-wrapper # Linux builds only
$ pip3 install delocate         # macOS builds only
$ python3 setup.py bdist_wheel
$ pip3 install pybind11==2.6.1
$ pip3 install dist/*
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

## Building and viewing the documentation
To build the documentation, run

```shell
$ cd docs
$ make html
```
You must have the development build built and installed for this to work.
To view the documentation, open in your web browser the file located at
`docs/build/html/index.html`

