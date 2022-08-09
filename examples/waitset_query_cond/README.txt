QueryCondition example
----------------------

This example has one publisher application (waitset_query_cond_publisher.py)
and a subscriber application. There are two equivalent versions of the
subscriber application:
    - waitset_query_cond_subscriber.py - uses a WaitSet to wait for data
    - async_take_query_condition.py - uses async operations (take_data_async)

The example type is defined in IDL in waitset_query_cond.idl and its Python
definition is in waitset_query_cond.py.