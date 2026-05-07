#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 21] started at $(date)"
echo "[batch 21] command: python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test bf --output bf_unsat_1200_W1D1.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 1\ --watched-literals\ 1\ --unsat_test\ bf\ --output\ bf_unsat_1200_W1D1.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_21.log
status=${PIPESTATUS[0]}
echo
echo "[batch 21] finished at $(date), exit code: $status"
echo "[batch 21] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_21.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
