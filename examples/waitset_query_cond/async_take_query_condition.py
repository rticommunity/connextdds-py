
#  (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form only
#  for use with RTI products.  The Software is provided "as is", with no warranty
#  of any type, including any warranty for fitness for any purpose. RTI is under
#  no obligation to maintain or support the Software.  RTI shall not be liable for
#  any incidental or consequential damages arising out of the use or inability to
#  use the software.


# This example implements the same functionality as waitset_query_cond_subscriber.py
# but uses async operations (take_data_async) instead of a WaitSet.

import textwrap
import argparse
import asyncio

import rti.connextdds as dds
import rti.asyncio

from waitset_query_cond import waitset_query_cond


async def read_data(reader, query_cond):
    async for data in reader.take_data_async(condition=query_cond):
        print(data)


async def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)
    topic = dds.Topic(
        participant, "Example waitset_query_cond", waitset_query_cond)
    reader = dds.DataReader(dds.Subscriber(participant), topic)

    # Query against even samples at the start
    query_parameters = ["'EVEN'"]
    query = dds.Query(reader, "name MATCH %0", query_parameters)
    query_condition = dds.QueryCondition(query, dds.DataState.any_data)

    read_task = asyncio.create_task(read_data(reader, query_condition))

    print(
        textwrap.dedent(
            """\
        >>> Timeout: {} sec
        >>> Query conditions: {}
        \t %0 = {}
        ----------------------------------

        """.format(
                1, query_condition.expression, query_parameters[0]
            )
        )
    )

    count = 0
    while (sample_count == 0) or (count < sample_count):
        await asyncio.sleep(1)

        if count == 7:
            # Update the query after 7 seconds
            query_parameters[0] = "'ODD'"
            query_condition.parameters = query_parameters
            print(
                textwrap.dedent(
                    """
                CHANGING THE QUERY CONDITION
                >>> Query conditions:
                {}
                \t %%0 = {}
                >>> We keep one sample in the history
                -------------------------------------
                """.format(
                        query_condition.expression, query_parameters[0]
                    )
                )
            )

        count += 1

    read_task.cancel()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Waitsets with Query Conditions (Subscriber)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    rti.asyncio.run(subscriber_main(args.domain, args.count))
