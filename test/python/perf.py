import rti.connextdds as dds
import time
import pathlib
import argparse
import sys
import utils

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/PerfTest.xml"


def main(count):
    if count <= 0:
        print("Count cannot be zero or below")
        sys.exit(1)
    print(f"Running test {count} times")

    # By setting these to really high and low values, it ensures that we will have a
    # proper min and max value at the end
    min_time = float("inf")
    max_time = float("-inf")

    total_time = 0

    system = utils.TestSystem(0, "PerformanceTest")
    for i in range(0, count):

        system.writer.set_number("myOctetSeq[%d]" % (600000), 25)
        system.writer.write()
        system.reader.take()

        start = time.time()

        sample = system.reader.samples.get_dictionary(0)
        outArray = sample["myOctSeq"]

        elapsed = time.time() - start

        min_time = min(min_time, elapsed)
        max_time = max(max_time, elapsed)
        total_time += elapsed

    average = total_time / count

    print(f"Average: {average}\nMinimum: {min_time}\nMaximum: {max_time}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("count", type=int, nargs="?", default=1000)
    arg = parser.parse_args()
    main(arg.count)
