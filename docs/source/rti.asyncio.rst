rti.asyncio
~~~~~~~~~~~

.. note::

    This module requires Python 3.7 or newer

This module must be imported in order to use the methods
:meth:`rti.connextdds.DataReader.take_async`
and :meth:`rti.connextdds.DataReader.take_data_async`.

These two methods are added to the DataReader class when this module is
imported.

The module also defines a convenience function ``rti.asyncio.run``, which
is similar to ``asyncio.run``, and can synchronously run the main async function
in an application.

See :ref:`reader:Subscriptions`.

.. automodule:: rti.asyncio
   :members:

