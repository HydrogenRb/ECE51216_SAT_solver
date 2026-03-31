# 数据结构
# 三层： formula包含clause， clause包含literal， literal是int（正负表示正负字面）
# assignment: dict {变量: 布尔值}， 变量是正整数， 这个dict的长度是变量的数量

import typing
Assignment = typing.Dict[int, bool]
Clause = typing.List[int]
Formula = typing.List[Clause]

def find_pure_lit(Target_Formula: Formula) -> list[int]:
    """
    寻找纯文字（pure literal）：
    - 变量只以正极性出现 -> 可直接令其为真（返回 +v）
    - 变量只以负极性出现 -> 可直接令其为假（返回 -v）
    """
    # 创建两个列表，纯文字列表和排除列表
    # 看到一个lit就
    # 1 如果在排除列表中，那就继续
    # 2 如果在纯文字列表中，并且极性一样，那就继续
    # 3 如果在纯文字列表中，但极性不一样，那就从纯文字列表中删除，并加入排除列表
    # 4 如果不在纯文字列表中，也不在排除列表中，那就加入纯文字列表
    # 最后返回纯文字列表
    pure_literals = []
    excluded = []
    for clause in Target_Formula:
        for lit in clause:
            if lit in excluded or -lit in excluded:
                continue
            if lit in pure_literals:
                continue
            if -lit in pure_literals:
                pure_literals.remove(-lit)
                excluded.append(lit)
                excluded.append(-lit)
            else:
                pure_literals.append(lit)
    return pure_literals

def remove_pure_lit(Target_Formula: Formula, pure_literals: list[int]) -> Formula:
    """
    将纯文字从公式中删除。
    纯文字所在的子句已经满足，可以直接删除这些子句。
    """
    # 循环所有lit，如果有lit是纯文字，就不把这个clause加入新的formula
    reduced_formula = []
    for clause in Target_Formula:
        flag = True
        for lit in pure_literals:
            if lit in clause:
                flag = False
        if flag: #如果flag是1 加入新的formula
            reduced_formula.append(clause)
    return reduced_formula

def find_unit_clauses(Target_Formula: Formula) -> list[int]:
    """
    寻找单子句（长度为 1 的子句）中的 literal。
    这些 literal 必须被赋值以满足对应的子句。
    """
    # 将所有长度为1的clause中的lit加入unit_literals列表
    # 去重
    unit_literals = []
    for clause in Target_Formula:
        if len(clause) == 1:
            lit = clause[0]
            if lit not in unit_literals and -lit not in unit_literals: # 如果正反都不在，就加
                #注： 这一步如果有反的 其实直接就是UNSAT 下一步再处理
                unit_literals.append(lit)
    return unit_literals

def remove_unit_clauses(Target_Formula: Formula, unit_literals: list[int]) -> Formula:
    """
    将单子句中的 literal 从公式中删除。
    这些 literal 必须被赋值以满足对应的子句，因此相关子句已经满足，可以直接删除这些子句。
    """
    # 遍历每一个lit
    # 如果其在unit列表中 对应的clause不加入新的formula
    # 如果其反在unit中 对应的clause去掉这个lit后加
    reduced_formula = []
    for clause in Target_Formula:
        flag = True
        temp_clause = clause.copy()
        for lit in clause:
            if lit in unit_literals:
                flag = False #如果同时有lit和-lit 因为flag为0 所以直接删掉这个clause
            if -lit in unit_literals:
                temp_clause.remove(lit)
        if flag:
            reduced_formula.append(temp_clause)
    return reduced_formula

def remove_lit(Target_Formula: Formula, lit_for_remove: int) -> Formula:
    """
    将 literal 从公式中删除。
    这个 lit 是我们手动设置的 也就是第二个参数
    """
    reduced_formula = []
    opposite = -lit_for_remove
    for clause in Target_Formula:
        temp_clause = clause.copy()
        flag = True
        for lit in clause:
            if lit == lit_for_remove:
                flag = False
            if lit == opposite:
                temp_clause.remove(lit)
        if flag:
            reduced_formula.append(temp_clause)
    return reduced_formula

def test_unsat(Target_Formula: Formula) -> bool:
    """
    检测是否存在空子句。
    存在空子句说明当前分支冲突，公式不可满足。
    """
    for clause in Target_Formula:
        if len(clause) == 0:
            return True
    return False

def DPLL(Target_Formula: Formula) -> typing.Tuple[Formula, bool]:
    """
    DPLL的主流程
    """
    # 单子句并remove #使用find_unit_clauses和remove_unit_clauses
    # 纯文字并remove #使用find_pure_lit和remove_pure_lit
    # 使用test_unsat检测是否有空子句
    # 如果没有空子句 返回True
    # 如果有空子句 返回False
    # 将某个子句变成1 递归 #使用remove_lit
    
    while find_unit_clauses(Target_Formula):
        Target_Formula = remove_unit_clauses(Target_Formula, find_unit_clauses(Target_Formula))
        # 如果有unit clauses 循环删掉所有的unit clauses
        if main_flag:
            print("处理子句，目前的公式为:", Target_Formula)
    
    Target_Formula = remove_pure_lit(Target_Formula, find_pure_lit(Target_Formula))
    #删掉所有的纯文字
    if main_flag:
        print("处理纯文字，目前的公式为:", Target_Formula)
       
    #输出结果
    if test_unsat(Target_Formula):
        return Target_Formula, False
    if not Target_Formula:
        return Target_Formula, True
    #没有结果 开始递归       

    lit = Target_Formula[0][0]
    new_formula = remove_lit(Target_Formula, lit) #左边
    if main_flag:
        print("当前递归层数进入左边")
    _, result = DPLL(new_formula)
    if result:
        return new_formula, True
    new_formula = remove_lit(Target_Formula, -lit) #右边
    if main_flag:
        print("当前递归层数进入右边")
    _, result = DPLL(new_formula)
    if result:
        return new_formula, True
    
    #输出结果
    if test_unsat(Target_Formula):
        return Target_Formula, False
    if not Target_Formula:
        return Target_Formula, True  
    return Target_Formula, False        
    

    

def input_cnf(file_path: str) -> Formula:
    """
    从文件中读取 CNF 公式（DIMACS）。
    处理:
    - 注释行 c ...
    - 头信息行 p cnf ...
    - 结束标记行 % ...
    - 子句结束符 0
    """
    formula: Formula = []
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            # 注释
            if line.startswith('c') or line.startswith('p cnf'):
                continue
            # 一些数据集用 % 表示正文结束
            if line.startswith('%'):
                break
            clause: Clause = []
            for tok in line.split():
                if tok == '0':      # 子句结束
                    break
                if tok.startswith('%'):
                    break
                clause.append(int(tok))  # 这里应只剩合法整数字面量
            if clause:
                formula.append(clause)
    return formula

def core_entrence(file_path: str) -> bool:
    formula = input_cnf(file_path)
    _, is_sat = DPLL(formula)
    return is_sat

def main ():
    file_path = 'uf20-01.cnf'
    formula = input_cnf(file_path)
    result, is_sat = DPLL(formula)
    if is_sat:
        print("SATISFIABLE")
    else:
        print("UNSATISFIABLE")
        
if __name__ == "__main__":
    main_flag = True
    main() #运行入口