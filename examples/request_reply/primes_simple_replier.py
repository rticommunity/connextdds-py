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
except:
    xrange = range

request_serviced = True

def is_prime(val):
    if val <= 1:
        return False
    if val == 2:
        return True
    if val > 2 and val % 2 == 0:
        return False

    max_div = math.floor(math.sqrt(val))
    for i in xrange(3, 1 + max_div, 2):
        if val % i == 0:
            return False
    return True


def replier_main(domain_id):
    global request_serviced
    participant = dds.DomainParticipant(domain_id)
    qos_provider = dds.QosProvider.default
    type_provider = dds.QosProvider('Primes.xml')
    request_type = type_provider.type("PrimeNumberRequest")
    reply_type = type_provider.type("PrimeNumberReply")
    status_type = type_provider.type("PrimeNumberCalculationStatus")

    def request_handler(request):
        global request_serviced
        request_serviced = True
        if (request["n"] <= 0 or 
                request["primes_per_reply"] <= 0 or
                request["primes_per_reply"] > reply_type["primes"].type.bounds):
            error_reply = dds.DynamicData(reply_type)
            error_reply["status"] = status_type["REPLY_ERROR"]
            print("Error, requester asked for too many primes per reply")
            return error_reply
        else:
            print("Calculating prime numbers below {}...".format(request["n"]))
            n = request["n"]
            max_count = request["primes_per_reply"]
            primes = dds.Int32Seq()

            reply = dds.DynamicData(reply_type)
            for m in xrange(1, n + 1):
                if is_prime(m):
                    primes.append(m)
                    if len(primes) > max_count:
                        reply["status"] = status_type["REPLY_ERROR"]
                        print("Error: too many calculated primes for a single reply")
                        return reply

            reply["primes"] = primes
            reply["status"] = status_type["REPLY_COMPLETED"]
            return reply

    replier = rti.request.SimpleReplier(
            request_type,
            reply_type,
            participant,
            request_handler,
            service_name="PrimeCalculator",
            datawriter_qos=qos_provider.datawriter_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"),
            datareader_qos=qos_provider.datareader_qos_from_profile("RequestReplyExampleProfiles::RequesterExampleProfile"))
    
    while replier.matched_requester_count == 0:
        time.sleep(0.1)

    while request_serviced:
        request_serviced = False
        time.sleep(20)

    print("Timed out waiting for requests")
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Coherent Presentation (Publisher)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID (default: 0)")

    args = parser.parse_args()
    assert 0 <= args.domain < 233

    replier_main(args.domain)
