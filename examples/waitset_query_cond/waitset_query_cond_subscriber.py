import rti.connextdds as dds
import textwrap
import time
import argparse


def query_handler(reader, query_cond):
    with reader.select().condition(query_cond).take() as samples:
        for sample in samples:
            output = sample.data if sample.info.valid else 'Got metadata'
            print(output)


def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    wsqc_type = dds.QosProvider('waitset_query_cond.xml').type('wsqc_lib', 'waitset_query_cond')
    topic = dds.DynamicData.Topic(participant, 'Example waitset_query_cond', wsqc_type)
    reader_qos = dds.QosProvider.default().datareader_qos
    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic, reader_qos)

    query_parameters = ["'EVEN'"]
    query = dds.Query(reader, 'name MATCH %0', query_parameters)
    query_condition = dds.QueryCondition(query, dds.DataState.any_data())

    waitset = dds.WaitSet()
    waitset += query_condition

    print(textwrap.dedent("""\
        >>> Timeout: {} sec
        >>> Query conditions: {}
        \t %0 = {}
        ----------------------------------
        
        """.format(1, query_condition.expression, query_parameters[0])))

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if count == 7:
            query_parameters[0] = "'ODD'"
            query_condition.parameters = query_parameters
            print(textwrap.dedent(
                """
                CHANGING THE QUERY CONDITION
                >>> Query conditions:
                {}
                \t %%0 = {}
                >>> We keep one sample in the history
                -------------------------------------
                """.format(query_condition.expression, query_parameters[0])))

        active = waitset.wait((1, 0))
        if len(active) > 0:
            if query_condition in active:
                query_handler(reader, query_condition)

        count += 1


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Waitsets with Query Conditions (Subscriber)')
parser.add_argument(
    '-d',
    '--domain',
    type=int,
    default=0,
    help='DDS Domain ID')
parser.add_argument(
    '-c',
    '--count',
    type=int,
    default=0,
    help='Number of samples to send')

args = parser.parse_args()
assert(0 <= args.domain < 233)
assert(args.count >= 0)

subscriber_main(args.domain, args.count)
