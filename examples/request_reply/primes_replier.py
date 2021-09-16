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
import rti.request
import argparse
import time
import math

try:
    xrange
except NameError:
    xrange = range

def is_prime(val):
    if val <= 1:
        return False
    if val == 2:
        return True
    if val > 2 and val % 2 == 0:
        return False

    max_div = int(math.floor(math.sqrt(val)))
    for i in xrange(3, 1 + max_div, 2):
        if val % i == 0:
            return False
    return True


def replier_main(domain_id):
    participant = dds.DomainParticipant(domain_id)
    qos_provider = dds.QosProvider.default
    type_provider = dds.QosProvider('Primes.xml')
    request_type = type_provider.type("PrimeNumberRequest")
    reply_type = type_provider.type("PrimeNumberReply")
    status_type = type_provider.type("PrimeNumberCalculationStatus")

    replier = rti.request.Replier(
            request_type,
            reply_type,
            participant,
            service_name="PrimeCalculator",
            datawriter_qos=qos_provider.datawriter_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"),
            datareader_qos=qos_provider.datareader_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"))

    print("Prime calculation replier started on domain {}...".format(domain_id))
    max_wait = dds.Duration.from_seconds(20)
    requests = replier.receive_requests(max_wait)

    while len(requests) > 0:
        for request in (sample for sample in requests if sample.info.valid):
            if (request.data["n"] <= 0 or 
                    request.data["primes_per_reply"] <= 0 or
                    request.data["primes_per_reply"] > reply_type["primes"].type.bounds):
                error_reply = dds.DynamicData(reply_type)
                error_reply["status"] = status_type["REPLY_ERROR"]
                replier.send_reply(error_reply, request.info)
            else:
                print("Calculating prime numbers below {}...".format(request.data["n"]))
                n = request.data["n"]
                max_count = request.data["primes_per_reply"]
                primes = dds.Int32Seq()

                reply = dds.DynamicData(reply_type)
                for m in xrange(1, n + 1):
                    if is_prime(m):
                        primes.append(m)
                        if len(primes) == max_count:
                            reply["primes"] = primes
                            reply["status"] = status_type["REPLY_IN_PROGRESS"]
                            replier.send_reply(reply, request.info, final=False)
                            primes.clear()

                reply["primes"] = primes
                reply["status"] = status_type["REPLY_COMPLETED"]
                replier.send_reply(reply, request.info)
                print("DONE")

        requests = replier.receive_requests(max_wait)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Coherent Presentation (Publisher)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID (default: 0)")

    args = parser.parse_args()
    assert 0 <= args.domain < 233

    replier_main(args.domain)
