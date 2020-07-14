# connextdds-py

> A full Python binding for RTI Connext DDS using pybind11

[![Build Status](https://travis-ci.com/rticommunity/connextdds-py.svg?token=BnoLsZyPsyeg3f6xwqc6&branch=master)](https://travis-ci.com/rticommunity/connextdds-py)

## Requirements

- macOS or Linux (Windows support coming soon)
- CMake 3.15+
- Python 2.7, Python 3.x with pip
- RTI Connext DDS 5.3.1, 6.0.0, 6.0.1

## Installation

> clone repo and submodules and cd to the project root

```shell
$ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
$ cd connextdds-py
```

> set environment variables

```shell
$ export NDDSHOME=<install location of Connext Pro>
$ export CONNEXTDDS_ARCH=<target arch>
$ export LD_LIBRARY_PATH=<location of target libs>:$LD_LIBRARY_PATH
$ export NJOBS=<number of concurrent make jobs, default 1>
```

> build and install

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

## Example

```python
import rti.connextdds as dds
import time

participant = dds.DomainParticipant(0)
topic = dds.StringTopicType.Topic(participant, 'example')
writer = dds.StringTopicType.DataWriter(
    participant.implicit_publisher,
    topic)

while True:
    writer.write("Hello World!")
    time.sleep(1)
```

## Features

- Python DDS API supporting DynamicData and built-in topic types
- Object oriented design based on Modern C++ API
- Supports QoS modifications in code
- Supports listeners
- Supports waitsets and all conditions
- Supports RTI Distributed Logger and integration with Python `logging` module

## TODO

- Add unit tests
- Add API documentation
- Implement support for Request/Reply
- Implement support for Routing Service processors/adapters

## Notes

- Because Python only supports generics in Python 3.5+ and there doesn't seem to be any support for them in the current version of pybind11, C++ template classes are generally implemented as nested classes for a topic data type. For example:
    - rti.connextdds.DynamicData.Topic
    - rti.connextdds.DynamicData.DataReader
    - rti.connextdds.DynamicData.DataWriter
    - etc.
- The Python GIL prevents multiple CPU cores from executing Python code in the same application in parallel. This means that if a single-threaded Python application is blocked in a native method (e.g. by calling rti.connextdds.WaitSet.wait()) then Ctrl-C may not be sufficient to break execution. Workarounds include:
    - using Python threading to ensure that the GIL is acquired when a native method releases it.
    - implement a custom signal handler
    - send a SIGQUIT via keyboard shortcut instead of SIGINT
    - kill the Python process via the command line
- SIP on macOS prevents Python from using DYLD_LIBRARY_PATH to find the target libraries, so it is recommended to put them somewhere on the standard search path (e.g. $HOME/lib)
- Getter/setter APIs have been replaced by Python properties in most cases. The exception is write-only properties, which are still normal method calls with the new value as the argument.
