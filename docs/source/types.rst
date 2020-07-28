.. py:currentmodule:: rti.connextdds

Types
~~~~~

Types in connextdds-py are similar to the types in other RTI APIs
but with a few differences. Due to Python's dynamic nature, Dynamic
Data is used. You cannot directly use IDL files for your types,
however, you can generate XML files from your IDL files. To do so,
at the command line run:

.. code-block:: shell

    $ rtiddsgen -convertToXml your_idl_file.idl 

Generating types this way is very easy. After you have an XML file,
you can get the type from the file like this:

.. code-block:: python

    import rti.connextdds as dds
    FILE = "path to your xml file"
    provider = dds.QosProvider(FILE)
    provider_type = provider.type("NameOfType")

Now in the provider_type variable you have the type defined in the IDL file.
To make an instance of it, all you have to do is this:

.. code-block:: python
    
    sample = dds.DynamicData(provider_type)
    # Let's say we have a field x that takes a 32 bit integer,
    # we can assign it like this
    sample['x'] = 42
    
Now you would be able to publish the sample which is discussed in
:ref:`writer:Publications`
