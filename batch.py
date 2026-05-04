#!/usr/bin/env python3
import argparse
import csv
import re
import subprocess
import sys
from pathlib import Path


MONITOR_COLUMNS = [
    "monitor_start_time",
    "monitor_total_time",
    "monitor_unit_propagation_time",
    "monitor_clause_checks",
    "monitor_variable_assignments",
    "monitor_backtrack_count",
    "monitor_state_copies",
    "monitor_unit_propagation_rounds",
    "monitor_unit_propagation_calls",
    "monitor_current_recursion_depth",
    "monitor_max_recursion_depth",
]


MONITOR_PATTERNS = {
    "monitor_start_time": r"Start Time:\s*([0-9.+eE-]+)",
    "monitor_total_time": r"Total Execution Time:\s*([0-9.+eE-]+)",
    "monitor_unit_propagation_time": r"Unit Propagation Time:\s*([0-9.+eE-]+)",
    "monitor_clause_checks": r"Total Clause Checks:\s*([0-9]+)",
    "monitor_variable_assignments": r"Variable Assignments:\s*([0-9]+)",
    "monitor_backtrack_count": r"Backtrack Operations:\s*([0-9]+)",
    "monitor_state_copies": r"State Copies:\s*([0-9]+)",
    "monitor_unit_propagation_calls": r"Unit Propagation Calls:\s*([0-9]+)",
    "monitor_unit_propagation_rounds": r"Unit Propagation Rounds:\s*([0-9]+)",
    "monitor_current_recursion_depth": r"Current Recursion Depth:\s*([0-9]+)",
    "monitor_max_recursion_depth": r"Maximum Recursion Depth:\s*([0-9]+)",
}


def expected_result(cnf_path, test_dir):
    parts = [part.lower() for part in cnf_path.relative_to(test_dir).parts]
    parts.extend(part.lower() for part in test_dir.parts)
    if "unsat" in parts:
        return "UNSAT"
    if "sat" in parts:
        return "SAT"
    return "UNKNOWN"


def parse_result(stdout):
    match = re.search(r"^RESULT:(SAT|UNSAT)\s*$", stdout, re.MULTILINE)
    if match:
        return match.group(1)
    return "UNKNOWN"


def parse_monitor(stdout):
    stats = {column: "" for column in MONITOR_COLUMNS}
    for column, pattern in MONITOR_PATTERNS.items():
        match = re.search(pattern, stdout)
        if match:
            stats[column] = match.group(1)
    return stats


def run_solver(solver, cnf_path, test_dir, timeout, dlis):
    try:
        completed = subprocess.run(
            [str(solver), str(cnf_path), "--DLIS", str(dlis)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=timeout,
            check=False,
        )
        stdout = completed.stdout
        stderr = completed.stderr
        exit_code = completed.returncode
        timed_out = False
    except subprocess.TimeoutExpired as exc:
        stdout = exc.stdout or ""
        stderr = exc.stderr or ""
        exit_code = ""
        timed_out = True

    expected = expected_result(cnf_path, test_dir)
    actual = parse_result(stdout)
    stats = parse_monitor(stdout)
    correct = (
        not timed_out
        and exit_code == 0
        and expected != "UNKNOWN"
        and actual == expected
        and "Wrong assignment" not in stdout
    )

    row = {
        "cnf_file": str(cnf_path),
        "category": cnf_path.relative_to(test_dir).parts[0],
        "DLIS": dlis,
        "expected_result": expected,
        "actual_result": actual,
        "correct": "TRUE" if correct else "FALSE",
        "exit_code": exit_code,
        "timed_out": "TRUE" if timed_out else "FALSE",
        "stderr": stderr.strip(),
    }
    row.update(stats)
    return row


def main():
    parser = argparse.ArgumentParser(
        description="Run mysAT on every CNF file under test/ and save monitor stats to CSV."
    )
    parser.add_argument("--solver", default="./mysAT", help="Path to the DPLL solver executable.")
    parser.add_argument("--test-dir", default="test", help="Directory containing sat/ and unsat/ CNF files.")
    parser.add_argument("--output", default="batch_results.csv", help="CSV output path.")
    parser.add_argument("--timeout", type=float, default=None, help="Optional timeout per CNF file, in seconds.")
    parser.add_argument("--DLIS", type=int, choices=[0, 1], default=0, help="Set to 1 to enable DLIS variable selection.")
    args = parser.parse_args()

    solver = Path(args.solver)
    test_dir = Path(args.test_dir)
    output = Path(args.output)

    if not solver.exists():
        print(f"error: solver not found: {solver}", file=sys.stderr)
        return 1
    if not test_dir.is_dir():
        print(f"error: test directory not found: {test_dir}", file=sys.stderr)
        return 1

    cnf_files = sorted(test_dir.rglob("*.cnf"))
    if not cnf_files:
        print(f"error: no .cnf files found under {test_dir}", file=sys.stderr)
        return 1

    rows = []
    for cnf_path in cnf_files:
        row = run_solver(solver, cnf_path, test_dir, args.timeout, args.DLIS)
        rows.append(row)
        status = "OK" if row["correct"] == "TRUE" else "FAIL"
        print(
            f"[{status}] {cnf_path}: expected={row['expected_result']} "
            f"actual={row['actual_result']} DLIS={row['DLIS']}"
        )

    fieldnames = [
        "cnf_file",
        "category",
        "DLIS",
        "expected_result",
        "actual_result",
        "correct",
        "exit_code",
        "timed_out",
    ] + MONITOR_COLUMNS + ["stderr"]

    with output.open("w", newline="") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    passed = sum(1 for row in rows if row["correct"] == "TRUE")
    print(f"Wrote {len(rows)} rows to {output}")
    print(f"Correct results: {passed}/{len(rows)}")
    return 0 if passed == len(rows) else 1


if __name__ == "__main__":
    sys.exit(main())
