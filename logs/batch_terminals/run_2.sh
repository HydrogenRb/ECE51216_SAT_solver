#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 2] started at $(date)"
echo "[batch 2] command: python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test UF75.325.100 --unsat_test UUF75.325.100 --output UF75_both_1200_W0D1.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 1\ --watched-literals\ 0\ --sat_test\ UF75.325.100\ --unsat_test\ UUF75.325.100\ --output\ UF75_both_1200_W0D1.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_2.log
status=${PIPESTATUS[0]}
echo
echo "[batch 2] finished at $(date), exit code: $status"
echo "[batch 2] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_2.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
