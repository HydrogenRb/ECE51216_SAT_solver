# SAT Solver
## Project Overview
In this project, we will implement a SAT solver based on the
DPLL backtracking algorithm with Boolean Constraint Prop-
agation. The two additional heuristics we plan to implement
are Watched literals DLIS.

## Compilation
Use this command to compile the project:
```bash
g++ -std=c++17 DPLL.cpp -o DPLL
```

## Run SAT Solver
Use this command to run the implemented SAT Solver. The default setting enables both watched literals and DLIS.
```bash
./DPLL test.cnf
```
Some additional aurguments can be used to change the settings:
```bash
--DLIS Enables or disables the DLIS branching heuristic.
--watched-literals Enables or disables watched-literal based unit propagation.
--monitor Enables or disables performance monitor output.
```


g++ -std=c++17 DPLL.cpp -o DPLL
./DPLL test.cnf

功能1：选择特定的文件和数量跑，打开batch.py
PRIORITY_FOLDERS = [
    "sat/BMS_k3_n100_m429", //修改这一行
    "unsat/UUF50.218.1000", //修改这一行
]
TESTS_PER_PRIORITY_FOLDER = 14 //修改这一行

功能2：自选各种参数
usage: batch.py [-h] [--solver SOLVER] [--test-dir TEST_DIR] [--output OUTPUT]
                [--timeout TIMEOUT] [--DLIS {0,1}] [--watched-literals {0,1}]

输入python3 batch.py运行

例子：python3 batch.py --timeout 10 --DLIS 0 //设置10秒timeout，关闭DLIS功能，运行前14个相关文件夹中的cnf

测试用例子
make all //重新编译
//人类：去修改python中的文件夹等
python3 batch.py --timeout 300 --DLIS 0 --watched-literals 0 --output W0D0.csv //运行
python3 batch.py --timeout 300 --DLIS 1 --watched-literals 0 --output W0D1.csv
python3 batch.py --timeout 300 --DLIS 0 --watched-literals 1 --output W1D0.csv
python3 batch.py --timeout 300 --DLIS 1 --watched-literals 1 --output W1D1.csv

python3 batch.py --timeout 180 --DLIS 0 --watched-literals 0 --sat_test UF75.325.100 --output UF75_sat_180_W0D0.csv
python3 batch.py --timeout 180 --DLIS 1 --watched-literals 0 --sat_test UF75.325.100 --output UF75_sat_180_W0D1.csv
python3 batch.py --timeout 180 --DLIS 0 --watched-literals 1 --sat_test UF75.325.100 --output UF75_sat_180_W1D0.csv
python3 batch.py --timeout 180 --DLIS 1 --watched-literals 1 --sat_test UF75.325.100 --output UF75_sat_180_W1D1.csv
python3 batch.py --timeout 180 --DLIS 0 --watched-literals 0 --unsat_test UUF75.325.100 --output UF75_unsat_180_W0D0.csv
python3 batch.py --timeout 180 --DLIS 1 --watched-literals 0 --unsat_test UUF75.325.100 --output UF75_unsat_180_W0D1.csv
python3 batch.py --timeout 180 --DLIS 0 --watched-literals 1 --unsat_test UUF75.325.100 --output UF75_unsat_180_W1D0.csv
python3 batch.py --timeout 180 --DLIS 1 --watched-literals 1 --unsat_test UUF75.325.100 --output UF75_unsat_180_W1D1.csv

# 5/7
## 标准测试
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test UF75.325.100 --output UF75_sat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test UF75.325.100 --unsat_test UUF75.325.100 --output UF75_both_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --sat_test UF75.325.100 --unsat_test UUF75.325.100 --output UF75_both_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --sat_test UF75.325.100 --unsat_test UUF75.325.100 --output UF75_both_1200_W1D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test UUF75.325.100 --output UF75_unsat_1200_W0D0.csv"

## 测试UF150
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test UF150.645.100 --output UF150_sat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test UF150.645.100 --output UF150_sat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --sat_test UF150.645.100 --output UF150_sat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --sat_test UF150.645.100 --output UF150_sat_1200_W1D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test UUF150.645.100 --output UF150_unsat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test UUF150.645.100 --output UF150_unsat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test UUF150.645.100 --output UF150_unsat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test UUF150.645.100 --output UF150_unsat_1200_W1D1.csv"

## BMC和bf测试第一部分
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test bmc-sat --output bmc_sat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test bmc-sat --output bmc_sat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --sat_test bmc-sat --output bmc_sat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --sat_test bmc-sat --output bmc_sat_1200_W1D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test bf --output bf_unsat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test bf --output bf_unsat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test bf --output bf_unsat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test bf --output bf_unsat_1200_W1D1.csv"

## BMC和bf测试第二部分
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test bmc-sat-2 --output bmc_2_sat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test bmc-sat-2 --output bmc_2_sat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --sat_test bmc-sat-2 --output bmc_2_sat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --sat_test bmc-sat-2 --output bmc_2_sat_1200_W1D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test bf-2 --output bf_2_unsat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test bf-2 --output bf_2_unsat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test bf-2 --output bf_2_unsat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test bf-2 --output bf_2_unsat_1200_W1D1.csv"

## ssa测试
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --sat_test ssa-sat --output ssa_sat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --sat_test ssa-sat --output ssa_sat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --sat_test ssa-sat --output ssa_sat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --sat_test ssa-sat --output ssa_sat_1200_W1D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test ssa-unsat --output ssa_unsat_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test ssa-unsat --output ssa_unsat_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test ssa-unsat --output ssa_unsat_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test ssa-unsat --output ssa_unsat_1200_W1D1.csv"

## Python对比测试
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 0 --unsat_test UUF50.218.1000 --output UUF50_python_1200_W0D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 0 --unsat_test UUF50.218.1000 --output UUF50_python_1200_W0D1.csv"
"python3 batch.py --timeout 1200 --DLIS 0 --watched-literals 1 --unsat_test UUF50.218.1000 --output UUF50_python_1200_W1D0.csv"
"python3 batch.py --timeout 1200 --DLIS 1 --watched-literals 1 --unsat_test UUF50.218.1000 --output UUF50_python_1200_W1D1.csv"
