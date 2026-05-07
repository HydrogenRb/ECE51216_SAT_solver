#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 20] started at $(date)"
echo "[batch 20] command: python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test bf --output bf_unsat_1200_W1D0.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 0\ --watched-literals\ 1\ --unsat_test\ bf\ --output\ bf_unsat_1200_W1D0.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_20.log
status=${PIPESTATUS[0]}
echo
echo "[batch 20] finished at $(date), exit code: $status"
echo "[batch 20] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_20.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
