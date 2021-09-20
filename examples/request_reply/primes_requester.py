"""
 (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 """

import rti.connextdds as dds
import rti.request as request
import argparse
import time


def requester_main(domain_id, n, primes_per_reply):
    participant = dds.DomainParticipant(domain_id)
    qos_provider = dds.QosProvider.default
    type_provider = dds.QosProvider('Primes.xml')
    request_type = type_provider.type("PrimeNumberRequest")
    reply_type = type_provider.type("PrimeNumberReply")
    status_type = type_provider.type("PrimeNumberCalculationStatus")

    requester = request.Requester(
            request_type,
            reply_type,
            participant,
            service_name="PrimeCalculator",
            datawriter_qos=qos_provider.datawriter_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"),
            datareader_qos=qos_provider.datareader_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"))

    print("Waiting to discover replier on domain {}...".format(domain_id))
    
    while requester.matched_replier_count == 0:
        time.sleep(0.1)

    prime_number_request = dds.DynamicData(request_type)
    prime_number_request["n"] = n
    prime_number_request["primes_per_reply"] = primes_per_reply

    print(
        "Sending a request to calculate the prime numbers <= {} in sequences of {} or fewer elements".format(
            n,
            primes_per_reply
        )
    )

    request_id = requester.send_request(prime_number_request)

    max_wait = dds.Duration.from_seconds(20)
    in_progress = True
    while in_progress:
        if not requester.wait_for_replies(max_wait, related_request_id=request_id):
            raise dds.TimeoutError("Timed out waitinf for replies")

        for reply in (r.data for r in requester.take_replies(request_id) if r.info.valid):
            primes = reply["primes"]
            for prime in primes:
                print(prime)
            
            if reply["status"] != status_type["REPLY_IN_PROGRESS"]:
                in_progress = False
                if reply["status"] == status_type["REPLY_ERROR"]:
                    raise RuntimeError("Error in replier")
        
    print("DONE")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Coherent Presentation (Publisher)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID (default: 0)")
    parser.add_argument(
        "-c", "--count", type=int, default=5, help="Number of primes per reply (min: 5, default: 5)"
    )
    parser.add_argument("n", type=int, help="Request will retrieve prime numbers <= n")

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 5

    requester_main(args.domain, args.n, args.count)
