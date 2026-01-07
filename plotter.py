import re
import matplotlib.pyplot as plt
from collections import defaultdict
import statistics

LINE_RE = re.compile(
    r"threads=(\d+).*ops_per_sec=([\d\.]+)"
)

def parse_result_file(path):
    data = defaultdict(list)

    with open(path) as f:
        for line in f:
            m = LINE_RE.search(line)
            if m:
                threads = int(m.group(1))
                ops_per_sec = float(m.group(2))
                data[threads].append(ops_per_sec)

    return data


def compute_avg_std(data):
    avg = {}
    std = {}

    for threads, values in data.items():
        avg[threads] = sum(values) / len(values)
        std[threads] = statistics.stdev(values) if len(values) > 1 else 0.0

    return avg, std

def plot_oversubscription(env_files, params):
    plt.figure()

    for label, path in env_files.items():
        ops = []
        err = []
        exps = []
        for exp, apps in params.items():
            if (apps != 8 or not label.endswith("VM")):
                data = parse_result_file(path + "-" + exp + ".txt")
                avg, std = compute_avg_std(data)

                exps.append(apps)
                ops.append(avg[4])
                err.append(std[4])

        plt.errorbar(
            exps,
            ops,
            yerr=err,
            marker='o',
            capsize=4,
            label=label
        )

    plt.xlabel("Application number")
    plt.ylabel("Throughput (ops/sec)")
    plt.title("Throughput vs Application Count (4 threads)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

def plot(env_files):
    plt.figure()

    for label, path in env_files.items():
        data = parse_result_file(path)
        avg, std = compute_avg_std(data)

        threads = sorted(avg.keys())
        ops = [avg[t] for t in threads]
        err = [std[t] for t in threads]

        plt.errorbar(
            threads,
            ops,
            yerr=err,
            marker='o',
            capsize=4,
            label=label
        )

    plt.xlabel("Threads per application")
    plt.ylabel("Throughput (ops/sec)")
    plt.title("Throughput vs Thread Count")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    env_files = {
        "Baseline": "results/baseline.txt",
        "Sharded": "results/sharded.txt",
    }
    
    oversubscription_files = {
        "Baseline": "results/baseline",
        "Sharded": "results/sharded",
        "Baseline Container": "results/container-baseline",
        "Sharded Container": "results/container-sharded",
        "Baseline VM": "results/vm-baseline",
        "Sharded VM": "results/vm-sharded",
    }

    oversubscription_params = {
        "N": 2,
        "2N": 4,
        "4N": 8,
    }

    plot(env_files)
    plot_oversubscription(oversubscription_files, oversubscription_params)


