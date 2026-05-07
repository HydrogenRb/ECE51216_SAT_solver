#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 18] started at $(date)"
echo "[batch 18] command: python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test bf --output bf_unsat_1200_W0D0.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 0\ --watched-literals\ 0\ --unsat_test\ bf\ --output\ bf_unsat_1200_W0D0.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_18.log
status=${PIPESTATUS[0]}
echo
echo "[batch 18] finished at $(date), exit code: $status"
echo "[batch 18] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_18.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
