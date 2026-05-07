#!/usr/bin/env bash
cd /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver || exit 1
echo "[batch 6] started at $(date)"
echo "[batch 6] command: python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test UF150.645.100 --output UF150_sat_1200_W0D0.csv"
echo
set -o pipefail
eval python3\ batch.py\ --timeout\ 1200\ --DLIS\ 0\ --watched-literals\ 0\ --sat_test\ UF150.645.100\ --output\ UF150_sat_1200_W0D0.csv 2>&1 | tee /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_6.log
status=${PIPESTATUS[0]}
echo
echo "[batch 6] finished at $(date), exit code: $status"
echo "[batch 6] log: /home/ecegridfs/a/337ta03/ECE51216_project/ECE51216_SAT_solver/logs/batch_terminals/run_6.log"
echo
echo "Press Enter to close this terminal..."
read -r _
exit "$status"
