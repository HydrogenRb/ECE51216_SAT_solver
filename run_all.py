#!/usr/bin/env python3
import argparse
import os
import re
import subprocess
import sys
import time
from pathlib import Path


def sanitize_name(value):
    value = value.strip().strip("/")
    if not value:
        return "all"
    name = Path(value).name
    match = re.match(r"U?UF([0-9]+)", name, re.IGNORECASE)
    if match:
        return f"UF{match.group(1)}"
    return re.sub(r"[^A-Za-z0-9]+", "_", name).strip("_") or "suite"


def default_suite_name(sat_tests, unsat_tests):
    tests = sat_tests + unsat_tests
    if not tests:
        return "all"

    names = []
    for test in tests:
        name = sanitize_name(test)
        if name not in names:
            names.append(name)

    return "_".join(names)


def build_command(args, dlis, watched_literals, output_path):
    command = [
        sys.executable,
        str(args.batch_script),
        "--solver",
        str(args.solver),
        "--test-dir",
        str(args.test_dir),
        "--output",
        str(output_path),
        "--DLIS",
        str(dlis),
        "--watched-literals",
        str(watched_literals),
        "--tests-per-folder",
        str(args.tests_per_folder),
    ]

    if args.timeout is not None:
        command.extend(["--timeout", str(args.timeout)])

    for sat_test in args.sat_test:
        command.extend(["--sat_test", sat_test])
    for unsat_test in args.unsat_test:
        command.extend(["--unsat_test", unsat_test])

    return command


def build_tasks(args):
    suite_name = args.name or default_suite_name(args.sat_test, args.unsat_test)
    tasks = []

    for dlis in args.dlis:
        for watched_literals in args.watched_literals:
            stem = f"{suite_name}_D{dlis}W{watched_literals}"
            output_path = args.output_dir / f"{stem}.csv"
            log_path = args.log_dir / f"{stem}.log"
            command = build_command(args, dlis, watched_literals, output_path)
            tasks.append(
                {
                    "name": stem,
                    "command": command,
                    "output_path": output_path,
                    "log_path": log_path,
                }
            )

    return tasks


def print_command(command):
    print(subprocess.list2cmdline([str(part) for part in command]))


def run_tasks(args, tasks):
    pending = list(tasks)
    running = []
    failed = []

    try:
        while pending or running:
            while pending and len(running) < args.jobs:
                task = pending.pop(0)
                log_file = task["log_path"].open("w")
                process = subprocess.Popen(
                    task["command"],
                    stdout=log_file,
                    stderr=subprocess.STDOUT,
                    text=True,
                )
                task["process"] = process
                task["log_file"] = log_file
                running.append(task)
                print(
                    f"Started {task['name']} pid={process.pid} "
                    f"csv={task['output_path']} log={task['log_path']}",
                    flush=True,
                )

            time.sleep(args.poll_interval)

            still_running = []
            for task in running:
                return_code = task["process"].poll()
                if return_code is None:
                    still_running.append(task)
                    continue

                task["log_file"].close()
                if return_code == 0:
                    print(f"Finished {task['name']} OK", flush=True)
                else:
                    failed.append(task)
                    print(
                        f"Finished {task['name']} FAIL return_code={return_code} "
                        f"log={task['log_path']}",
                        flush=True,
                    )

            running = still_running
    except KeyboardInterrupt:
        stop_running_tasks(running)
        raise

    return failed


def stop_running_tasks(running):
    for task in running:
        task["process"].terminate()

    for task in running:
        try:
            task["process"].wait(timeout=10)
        except subprocess.TimeoutExpired:
            task["process"].kill()
            task["process"].wait()
        task["log_file"].close()


def parse_args():
    parser = argparse.ArgumentParser(
        description="Launch multiple batch.py experiments with separate CSV and log files."
    )
    parser.add_argument("--batch-script", type=Path, default=Path("batch.py"), help="Path to batch.py.")
    parser.add_argument("--solver", type=Path, default=Path("./mySAT"), help="Path to the DPLL solver executable.")
    parser.add_argument("--test-dir", type=Path, default=Path("test"), help="Directory containing sat/ and unsat/.")
    parser.add_argument("--timeout", type=float, default=180, help="Timeout per CNF file, in seconds.")
    parser.add_argument(
        "--sat_test",
        action="append",
        default=[],
        help="SAT test folder under test/sat/. Can be repeated.",
    )
    parser.add_argument(
        "--unsat_test",
        action="append",
        default=[],
        help="UNSAT test folder under test/unsat/. Can be repeated.",
    )
    parser.add_argument(
        "--tests-per-folder",
        type=int,
        default=10000,
        help="Maximum CNF files to run from each selected folder.",
    )
    parser.add_argument("--name", help="Prefix for generated CSV/log filenames.")
    parser.add_argument("--output-dir", type=Path, default=Path("."), help="Directory for CSV files.")
    parser.add_argument("--log-dir", type=Path, default=Path("logs"), help="Directory for log files.")
    parser.add_argument("--jobs", type=int, default=min(4, os.cpu_count() or 1), help="Maximum parallel batch jobs.")
    parser.add_argument("--dlis", type=int, choices=[0, 1], nargs="+", default=[0, 1], help="DLIS values to run.")
    parser.add_argument(
        "--watched-literals",
        "--watched_literals",
        dest="watched_literals",
        type=int,
        choices=[0, 1],
        nargs="+",
        default=[0, 1],
        help="Watched-literals values to run.",
    )
    parser.add_argument("--poll-interval", type=float, default=2.0, help="Seconds between status checks.")
    parser.add_argument("--dry-run", action="store_true", help="Print commands without running them.")
    return parser.parse_args()


def main():
    args = parse_args()
    if args.jobs < 1:
        print("error: --jobs must be at least 1", file=sys.stderr)
        return 1

    args.output_dir.mkdir(parents=True, exist_ok=True)
    args.log_dir.mkdir(parents=True, exist_ok=True)

    tasks = build_tasks(args)
    print(f"Prepared {len(tasks)} tasks; running up to {args.jobs} at a time.", flush=True)

    if args.dry_run:
        for task in tasks:
            print(f"{task['name']}:")
            print_command(task["command"])
        return 0

    try:
        failed = run_tasks(args, tasks)
    except KeyboardInterrupt:
        print("Interrupted; stopping running jobs...", file=sys.stderr)
        return 130

    if failed:
        print(f"{len(failed)} task(s) failed. Check logs for details.", file=sys.stderr)
        return 1

    print("All tasks finished successfully.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
