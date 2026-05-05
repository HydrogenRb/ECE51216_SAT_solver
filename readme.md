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
python3 batch.py --timeout 15 --DLIS 0 --watched-literals 0 --output W0D0.csv //运行
python3 batch.py --timeout 15 --DLIS 1 --watched-literals 0 --output W0D1.csv
python3 batch.py --timeout 15 --DLIS 0 --watched-literals 1 --output W1D0.csv
python3 batch.py --timeout 15 --DLIS 1 --watched-literals 1 --output W1D1.csv