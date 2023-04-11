.. py:currentmodule:: rti.connextdds

XML Application Creation
~~~~~~~~~~~~~~~~~~~~~~~~

RTI Connext supports the use of XML for the complete system definition.
This includes not only the definition of the data types and Quality of Service
settings, but also the definition of the Topics, DomainParticipants, and all
the Entities they contain (Publishers, Subscribers, DataWriters and
DataReaders).

The application calls :meth:`QosProvider.create_participant_from_config` to
indicate the participant configuration name of the DomainParticipant that the
application wants to create. This method takes care of the rest: creating the
DomainParticipant, registering the types and creating Topics, and populating all
the configured Entities. When the application needs to read or write data,
register listeners, or perform any other action, it simply looks up the
appropriate Entity by name and uses it.


Defining your system in a configuration file
--------------------------------------------

The following example configures types and DDS Entities in XML:

.. code-block:: xml

    <!-- my_dds_system.xml -->

    <dds>
        <types>
            <struct name="Foo">
                <member name="x" type="int32"/>
            </struct>
        </types>

        <domain_library name="ExampleDomainLibrary" >
            <domain name="ExampleDomain" domain_id="0">
            <register_type name="Foo" type_ref="Foo"/>
            <topic name="ExampleTopic" register_type_ref="Foo"/>
            </domain>
        </domain_library>

        <domain_participant_library name="ExampleParticipantLibrary">
            <domain_participant name="ExamplePublicationParticipant" domain_ref="ExampleDomainLibrary::ExampleDomain">
                <publisher name="ExamplePublisher">
                    <data_writer name="ExampleWriter" topic_ref="ExampleTopic"/>
                </publisher>
            </domain_participant>
            <domain_participant name="ExampleSubscriptionParticipant" domain_ref="ExampleDomainLibrary::ExampleDomain">
                <data_reader name="ExampleReader" topic_ref="ExampleTopic"/>
            </domain_participant>
        </domain_participant_library>
    </dds>

See the `XML-Based Application Creation Getting Started Guide <https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/connext_dds_professional/xml_application_creation/index.htm>`_
for more information.

Creating the Entities from the configuration
--------------------------------------------

To load one of the DomainParticipants defined above, use the following code:

.. code-block:: python

    qos_provider = dds.QosProvider("my_dds_system.xml")
    participant = qos_provider.create_participant_from_config(
        "ExampleParticipantLibrary::ExamplePublicationParticipant"
    )

This creates all the participant's contained entities as well.

The type in this example was also defined in XML, and therefore the DataReaders
and DataWriters that use it are :class:`DynamicData.DataReader` and
:class:`DynamicData.DataWriter`. The following code looks up the DataWriter,
creates a data sample, and writes it:

.. code-block:: python

    writer = dds.DynamicData.DataWriter(
        participant.find_datawriter("ExamplePublisher::ExampleWriter")
    )
    sample = writer.create_data()
    sample["x"] = 10
    writer.write(sample)

You can look up the ``DataReader`` with
:meth:`DomainParticipant.find_datareader`.

Using IDL and Python data types
-------------------------------

You can also define your types in IDL and Python (as described in
:ref:`types:Data Types`) and refer to them in the XML configuration. To use a
Python type (whether it is defined directly in Python or generated from IDL),
the application must previously register it with the same name used in the XML
configuration file.

For example, given the following Python type:

.. code-block:: python

    @idl.struct
    class Point:
        x: int = 0
        y: int = 0

We can rewrite the ``<domain_library>`` in the previous XML configuration to
refer to ``Point`` as follows:

.. code-block:: xml

        <domain_library name="ExampleDomainLibrary" >
            <domain name="ExampleDomain" domain_id="0">
                <register_type name="Point"/>
                <topic name="ExampleTopic" register_type_ref="Point"/>
            </domain>
        </domain_library>


And then register the type before creating the participant:

.. code-block:: python

    dds.DomainParticipant.register_idl_type(Point, "Point")

    qos_provider = dds.QosProvider("my_dds_system.xml")
    participant = qos_provider.create_participant_from_config("ExamplePublicationParticipant")


Now the writer writes ``Point`` objects instead of ``DynamicData``:

.. code-block:: python

    writer = dds.DataWriter(
        participant.find_datawriter("ExamplePublisher::ExampleWriter")
    )
    writer.write(Point(x=10, y=20))
