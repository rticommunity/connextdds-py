import rti.connextdds as dds

def main(domain_id, sample_count):
    with dds.DomainParticipant(domain_id) as participant:
        provider = dds.QosProvider(FILE)

if __name__ == '__main__':
    main()
