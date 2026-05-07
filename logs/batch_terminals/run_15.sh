#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 15] started at $(date)"
echo "[batch 15] command: python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test bmc-sat --output bmc_sat_1200_W0D1.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 1\ --watched-literals\ 0\ --sat_test\ bmc-sat\ --output\ bmc_sat_1200_W0D1.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_15.log
status=${PIPESTATUS[0]}
echo
echo "[batch 15] finished at $(date), exit code: $status"
echo "[batch 15] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_15.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
