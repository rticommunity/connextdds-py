# pyrti

> A full Python binding for RTI Connext DDS using pybind11

## Requirements

- macOS or Linux (Windows support coming soon)
- CMake 3.15+
- Python 3.x (might work with 2.7) with pip
- RTI Connext DDS 6.0.0 (6.0.1 compatibility TBD)

## Installation

> clone repo and submodules and cd to the project root

```shell
$ git clone --recurse-submodules git@gitlab.com:rti-private/pyrti.git 
$ cd pyrti
```

> set environment variables

```shell
$ export NDDSHOME=<install location of Connext 6>
$ export CONNEXTDDS_ARCH=<target arch>
$ export LD_LIBRARY_PATH=<location of target libs>:$LD_LIBRARY_PATH
$ export NJOBS=<number of concurrent make jobs, default 1>
```

> build and install

```shell
$ pip install .
```

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

- Because Python does not have a templating/generics mechanism, template classes are generally implemented as nested classes for a topic data type. For example:
    - rti.connextdds.DynamicData.Topic
    - rti.connextdds.DynamicData.DataReader
    - rti.connextdds.DynamicData.DataWriter
    - etc.
- The Python GIL prevents multiple CPU cores from executing Python code in the same application in parallel. This means that if the code is blocked in a native method that has released the GIL (e.g. rti.connextdds.WaitSet.wait()) then Ctrl-C may not be sufficient to break execution. Workarounds include:
    - using Python threading to ensure that the GIL is acquired when a native method releases it.
    - implement a custom signal handler
    - send a SIGQUIT via keyboard shortcut instead of SIGINT
    - kill the Python process via the command line
- SIP on macOS prevents Python from using DYLD_LIBRARY_PATH to find the target libraries, so it is recommended to put them somewhere on the standard search path (e.g. $HOME/lib)
