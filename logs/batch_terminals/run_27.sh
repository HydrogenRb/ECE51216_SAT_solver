#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 27] started at $(date)"
echo "[batch 27] command: python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test bf-2 --output bf_2_unsat_1200_W0D1.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 1\ --watched-literals\ 0\ --unsat_test\ bf-2\ --output\ bf_2_unsat_1200_W0D1.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_27.log
status=${PIPESTATUS[0]}
echo
echo "[batch 27] finished at $(date), exit code: $status"
echo "[batch 27] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_27.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
