# SAT Solver
## Project Overview
In this project, we implement a SAT solver based on the DPLL backtracking algorithm with Boolean Constraint Propagation. The two additional heuristics we plan to implement are watched literals and DLIS.

## Compilation
Use this command to compile the project:
```bash
g++ -std=c++17 DPLL.cpp -o DPLL
```
Use this command to compile automaticlly:
```bash
make clean
make all
```

## Input File
The solver accepts DIMACS CNF format of input.

Example:
```text
p cnf 3 2
1 -3 0
2 3 -1 0
```
Additional CNF benchmarks could be found in here: https://www.cs.ubc.ca/%7Ehoos/SATLIB/benchm.html

## Run SAT Solver
Use this command to run the implemented SAT Solver. The default setting enables both watched literals and DLIS.
```bash
./DPLL test.cnf
```
Some additional arguments can be used to change the settings:
```bash
--DLIS                Enables or disables the DLIS branching heuristic.
--watched-literals    Enables or disables watched-literal based unit propagation.
--monitor             Enables or disables performance monitor output.
```
Example:
```bash
./DPLL test.cnf --DLIS 0
# this runs the SAT solver without the DLIS technique
./DPLL test.cnf --watched-literals 0
# this runs the SAT solver without the watched-literals technique
./DPLL test.cnf --DLIS 0 --watched-literals 0
# this runs the SAT solver without any advanced techniques
```

## Output Format
SAT example:

```text
RESULT:SAT
ASSIGNMENT:1=1 2=0 3=1
```

UNSAT example:

```text
RESULT:UNSAT
```

## Run batch test
```bash
python3 batch.py --timeout 180 --DLIS 0 --watched-literals 0 --sat_test UF75.325.100 --output UF75_sat_180_W0D0.csv
```
```text
--timeout                         is the stop time
--DLIS and --watched-literals     are flags
--sat_test                        is the folder in the /sat folder
--output                          indicate the csv file to save the output
```
By running the python batch test, the program will trace the performance and correcness.

## Run in mult terminal
Base on the fact that some testcase need long time, there is a bash script can run all python script parallel.
Just put the python script in the COMMANDS list.
