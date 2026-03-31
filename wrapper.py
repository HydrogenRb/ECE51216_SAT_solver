import sat_core
from pathlib import Path

def main ():
    folder_path = list(Path('testset/uuf50-218').glob('*.cnf'))
    for file_path in folder_path:
        is_sat = sat_core.core_entrence(file_path)
        if is_sat:
            print(f"{file_path}: SATISFIABLE")
        else:
            print(f"{file_path}: UNSATISFIABLE")
        
if __name__ == "__main__":
    main() #运行入口