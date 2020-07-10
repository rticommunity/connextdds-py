import rti.connextdds as dds
import time
import pathlib
import argparse
import sys

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/PerfTest.xml"


def main(count, ind_test, all_test):
    if count <= 0:
        print("Count cannot be zero or below")
        sys.exit(1)
    print(f"Running test {count} time" + ("s" if count > 1 else ""))

    # By setting these to really high and low values, it ensures that we will have a
    # proper min and max value at the end
    min_time_all = float("inf")
    max_time_all = float("-inf")

    min_time_ind = float("inf")
    max_time_ind = float("-inf")

    total_time_all = 0
    total_time_ind = 0

    perf_test_type = dds.QosProvider(FILE).type("PerformanceTest")
    sample = dds.DynamicData(perf_test_type)
    for i in range(0, count):

        if all_test:
            # For testing assigning to a whole array at once
            start_all = time.time()
            sample["myOctSeq"] = [25] * 600000
            lst = sample["myOctSeq"]

            elapsed_all = time.time() - start_all

            min_time_all = min(min_time_all, elapsed_all)
            max_time_all = max(max_time_all, elapsed_all)
            total_time_all += elapsed_all

        if ind_test:
            # For testing assigning to an array element by element
            start_ind = time.time()
            loan = sample.loan_value("myOctSeq")
            for j in range(0, 600000):
                loan.data[j] = 25
                x = loan.data[j]
            loan.return_loan()

            elasped_ind = time.time() - start_ind

            min_time_ind = min(min_time_ind, elasped_ind)
            max_time_ind = max(max_time_ind, elasped_ind)
            total_time_ind += elasped_ind

    if all_test:
        print("\tAssigning all at once")
        average_all = total_time_all / count
        print(
            f"Average: {average_all}\nMinimum: {min_time_all}\nMaximum: {max_time_all}\nTotal:   {total_time_all}"
        )
    if ind_test:
        print("\tAssigning individually")
        average_ind = total_time_ind / count
        print(
            f"Average: {average_ind}\nMinimum: {min_time_ind}\nMaximum: {max_time_ind}\nTotal:   {total_time_ind}"
        )


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
