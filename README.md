<br />
<p align="center">
  <a href="https://github.com/rticommunity/connextdds-py">
    <img src="docs/source/_static/rti-logo-tag-StackedRight.png" alt="RTI Connext Python API" height="50">
  </a>
</p>


<p align="center"><h1 align="center"><strong>RTI Connext Python API</strong></h1></p>

<p align="center">
  <em>A full Connext DDS binding for Python</em>
  <br />
  <a href="https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/index.html"><strong>Explore the documentation »</strong></a>
  <br />
  <a href="https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html">Getting Started</a>
  ·
  <a href="https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/building.html">Install</a>
  ·
  <a href="https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/overview.html">API Overview</a>
  ·
  <a href="https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/quick.html">API Reference</a>
  ·
  <a href="https://github.com/rticommunity/rticonnextdds-examples">Examples</a>
</p>

<br>

## Documentation

- The [Connext Getting Started Guide](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html)
helps you install the software and run your first RTI Connext Python application while learning general concepts of Connext.

- The [Connext Python API Reference](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/index.html)
provides additional examples, an overview of the API and the API reference.

- Additional Connext DDS documentation, including the User's Manual is available at the [Connext Community Portal](https://community.rti.com/static/documentation/).

## Hello World Example

The following is the basic code to publish and subscribe to a topic defined
by a simple data type. Similar code can be generated by *rtiddsgen* from an IDL
or XML file defining your data types.

Define your types:

```python
# hello.py

import rti.types as idl

@idl.struct
class HelloWorld:
    message: str = ""
```

Create a *DataWriter* to publish the HelloWorld Topic:

```python
# hello_publisher.py

import time
import rti.connextdds as dds
from hello import HelloWorld

participant = dds.DomainParticipant(domain_id=0)
topic = dds.Topic(participant, 'HelloWorld Topic', HelloWorld)
writer = dds.DataWriter(participant.implicit_publisher, topic)

for i in range(10):
    writer.write(HelloWorld(message=f'Hello World! #{i}'))
    time.sleep(1)
```

Create a *DataReader* to subscribe to the HelloWorld Topic:

```python
# hello_subscriber.py

import rti.connextdds as dds
import rti.asyncio
from hello import HelloWorld

participant = dds.DomainParticipant(domain_id=0)
topic = dds.Topic(participant, 'HelloWorld Topic', HelloWorld)
reader = dds.DataReader(participant.implicit_subscriber, topic)

async def print_data():
    async for data in reader.take_data_async():
        print(f"Received: {data}")

rti.asyncio.run(print_data())
```


## Feature highlights

- Python-friendly design.
- User data classes, defined in Python or generated from IDL.
- DynamicData and the built-in types.
- DDS Entities in code and in XML.
- QoS Policies in code and in XML.
- Content Filters with the ability to change the filter.
- Discovery topics.
- Status updates.
- Listeners, WaitSets and Conditions, and `async` reading.
- RTI Distributed Logger and integration with Python `logging` module
- Pluggable RTI components, such as Monitoring and DDS Secure.

## Building and Installing

The Connext Python API is currently provided as buildable source. See
[Building and Installing](https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/api/connext_dds/api_python/building.html) in the Connext DDS API Reference for instructions.

A future release will provide a pre-built pip distribution.