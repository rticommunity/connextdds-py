import rti.connextdds as dds
import time
import pathlib
import argparse
import sys
import math

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/PerfTest.xml"


def main(count, run_ind_test, run_all_test):
    if count <= 0:
        print("Count cannot be zero or below")
        sys.exit(1)
    print(f"Running test {count} time" + ("s" if count > 1 else ""))

    if run_all_test:
        all_test(count)
    if run_ind_test:
        ind_test(count)


def all_test(count):
    # For testing assigning to a whole array at once

    perf_test_type = dds.QosProvider(FILE).type("PerformanceTest")
    sample = dds.DynamicData(perf_test_type)
    # By setting these to really high and low values, it ensures that we will have a
    # proper min and max value at the end
    min_time_all = float("inf")
    max_time_all = float("-inf")

    total_time_all = 0
    full_lst = [25] * 600000
    # First test setter
    for i in range(0, count):
        start_all = time.time()
        sample["myOctSeq"] = full_lst
        elapsed_all = time.time() - start_all

        min_time_all = min(min_time_all, elapsed_all)
        max_time_all = max(max_time_all, elapsed_all)
        total_time_all += elapsed_all
        if (i % math.ceil(count / 20)) == 0:
            print(".", end="", flush=True)
    print("")

    print("\tAssigning all at once setter")
    average_all = total_time_all / count
    print(f"Average: {average_all}\nMinimum: {min_time_all}")
    print(f"Maximum: {max_time_all}\nTotal:   {total_time_all}")

    min_time_all = float("inf")
    max_time_all = float("-inf")

    total_time_all = 0

    for i in range(0, count):
        start_all = time.time()
        lst = sample["myOctSeq"]
        elapsed_all = time.time() - start_all

        min_time_all = min(min_time_all, elapsed_all)
        max_time_all = max(max_time_all, elapsed_all)
        total_time_all += elapsed_all
        if (i % math.ceil(count / 20)) == 0:
            print(".", end="", flush=True)
    print("")

    print("\tAssigning all at once getter")
    average_all = total_time_all / count
    print(f"Average: {average_all}\nMinimum: {min_time_all}")
    print(f"Maximum: {max_time_all}\nTotal:   {total_time_all}")


def ind_test(count):
    perf_test_type = dds.QosProvider(FILE).type("PerformanceTest")
    sample = dds.DynamicData(perf_test_type)
    # For testing assigning to an array element by element
    min_time_ind = float("inf")
    max_time_ind = float("-inf")
    total_time_ind = 0

    for i in range(0, count):
        loan = sample.loan_value("myOctSeq")
        start_ind = time.time()
        for j in range(0, 600000):
            loan.data[j] = 25

        elasped_ind = time.time() - start_ind
        loan.return_loan()

        min_time_ind = min(min_time_ind, elasped_ind)
        max_time_ind = max(max_time_ind, elasped_ind)
        total_time_ind += elasped_ind
        if i % math.ceil(count / 20) == 0:
            print(".", end="", flush=True)
    print("")

    print("\tAssigning individually setter")
    average_ind = total_time_ind / count
    print(f"Average: {average_ind}\nMinimum: {min_time_ind}")
    print(f"Maximum: {max_time_ind}\nTotal:   {total_time_ind}")

    min_time_ind = float("inf")
    max_time_ind = float("-inf")
    total_time_ind = 0

    for i in range(0, count):
        start_ind = time.time()
        loan = sample.loan_value("myOctSeq")
        for j in range(0, 600000):
            x = loan.data[j]
        loan.return_loan()

        elasped_ind = time.time() - start_ind

        min_time_ind = min(min_time_ind, elasped_ind)
        max_time_ind = max(max_time_ind, elasped_ind)
        total_time_ind += elasped_ind
        if i % math.ceil(count / 20) == 0:
            print(".", end="", flush=True)
    print("")

    print("\tAssigning individually getter")
    average_ind = total_time_ind / count
    print(f"Average: {average_ind}\nMinimum: {min_time_ind}")
    print(f"Maximum: {max_time_ind}\nTotal:   {total_time_ind}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("count", type=int, nargs="?", default=1000)
    parser.add_argument("-all", action="store_true")
    parser.add_argument("-ind", action="store_true")
    args = parser.parse_args()
    if args.all == args.ind:
        main(args.count, True, True)
    elif args.all:
        main(args.count, False, True)
    elif args.ind:
        main(args.count, True, False)