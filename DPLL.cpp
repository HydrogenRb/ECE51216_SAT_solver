#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <queue>

using namespace std;

int chooseDLISLiteral(const vector<vector<int>>& clauses, const vector<int>& assignment);
int useDLIS_g = 1;

// ==================== Performance Monitor ====================
// All monitor variables start with monitor_ prefix
struct PerformanceMonitor {
    // Time measurements
    chrono::high_resolution_clock::time_point monitor_start_time;
    double monitor_total_time = 0.0;
    double monitor_unit_propagation_time = 0.0;
    
    // Operation counters
    long long monitor_clause_checks = 0;
    long long monitor_variable_assignments = 0;
    long long monitor_backtrack_count = 0;
    long long monitor_state_copies = 0;
    long long monitor_unit_propagation_rounds = 0;
    long long monitor_unit_propagation_calls = 0;
    
    // Recursion tracking
    int monitor_current_recursion_depth = 0;
    int monitor_max_recursion_depth = 0;
} monitor_stats;

void monitor_start() {
    monitor_stats.monitor_start_time = chrono::high_resolution_clock::now();
}

void monitor_end() {
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - monitor_stats.monitor_start_time;
    monitor_stats.monitor_total_time = elapsed.count();
}

void monitor_print_results() {
    std::printf("\n[monitor output] ========== PERFORMANCE ANALYSIS RESULTS ==========\n");
    std::printf("[monitor output] Start Time: %.6f\n",
           chrono::duration<double>(monitor_stats.monitor_start_time.time_since_epoch()).count());
    std::printf("[monitor output] Total Execution Time: %.6f seconds\n", monitor_stats.monitor_total_time);
    std::printf("[monitor output] Unit Propagation Time: %.6f seconds (%.2f%% of total)\n", 
           monitor_stats.monitor_unit_propagation_time,
           (monitor_stats.monitor_total_time > 0) ? 
           (monitor_stats.monitor_unit_propagation_time / monitor_stats.monitor_total_time * 100) : 0);
    
    std::printf("\n[monitor output] --- Operation Counters ---\n");
    std::printf("[monitor output] Total Clause Checks: %lld\n", monitor_stats.monitor_clause_checks);
    std::printf("[monitor output] Variable Assignments: %lld\n", monitor_stats.monitor_variable_assignments);
    std::printf("[monitor output] Backtrack Operations: %lld\n", monitor_stats.monitor_backtrack_count);
    std::printf("[monitor output] State Copies: %lld\n", monitor_stats.monitor_state_copies);
    std::printf("[monitor output] Unit Propagation Calls: %lld\n", monitor_stats.monitor_unit_propagation_calls);
    std::printf("[monitor output] Unit Propagation Rounds: %lld\n", monitor_stats.monitor_unit_propagation_rounds);
    
    std::printf("\n[monitor output] --- Recursion Analysis ---\n");
    std::printf("[monitor output] Current Recursion Depth: %d\n", monitor_stats.monitor_current_recursion_depth);
    std::printf("[monitor output] Maximum Recursion Depth: %d\n", monitor_stats.monitor_max_recursion_depth);
    
    std::printf("\n[monitor output] --- Efficiency Metrics ---\n");
    if (monitor_stats.monitor_variable_assignments > 0) {
        std::printf("[monitor output] Avg Checks per Assignment: %.2f\n", 
               (double)monitor_stats.monitor_clause_checks / monitor_stats.monitor_variable_assignments);
    }
    if (monitor_stats.monitor_unit_propagation_calls > 0) {
        std::printf("[monitor output] Avg Rounds per UP Call: %.2f\n", 
               (double)monitor_stats.monitor_unit_propagation_rounds / monitor_stats.monitor_unit_propagation_calls);
    }
    std::printf("[monitor output] ============================================================\n\n");
}
// ==============================================================


// ======DLIS相关方法 begin === 
int DLIS_literalValue(int literal, const vector<int>& assignment) {
    int var = abs(literal);
    if (assignment[var] == 0) return 0;
    if (literal > 0) return assignment[var];
    return -assignment[var];
}

bool DLIS_isClauseSat(const vector<int>& clause, const vector<int>& assignment) {
    for (int literal : clause) {
        if (DLIS_literalValue(literal, assignment) == 1) return true;
    }
    return false;
}

int chooseDLISLiteral(const vector<vector<int>>& clauses, const vector<int>& assignment) {
    vector<int> positive_count(assignment.size(), 0);
    vector<int> negative_count(assignment.size(), 0);


    for (const vector<int>& clause : clauses) {
        if (DLIS_isClauseSat(clause, assignment)) continue;

        for (int literal : clause) {
            int var = abs(literal);
            if (assignment[var] != 0) continue;

            if (literal > 0) positive_count[var]++;
            else negative_count[var]++;
        }
    }
    int best_literal = 0;
    int best_count = -1;
//重点！DLIS的目标是一次性尽可能满足更多，所以统计neg和pos是非常重要的，遍历的时候先遍历能先被满足的！
    for (int var = 1; var < static_cast<int>(assignment.size()); var++) {
        if (assignment[var] != 0) continue;

        if (positive_count[var] > best_count) {
            best_count = positive_count[var];
            best_literal = var;
        }
        if (negative_count[var] > best_count) {
            best_count = negative_count[var];
            best_literal = -var;
        }
    }

    if (best_literal != 0) return best_literal;

    for (int var = 1; var < static_cast<int>(assignment.size()); var++) {
        if (assignment[var] == 0) return var;
    }
    return 0;
}

bool DLIS_Init(vector<int>& DLIS_order, const vector<vector<int>>& clauses){ //或许不需要这个
    int maxVar = DLIS_order.size();

    vector<int> temp_counter(maxVar, 0);

    for (const auto& clause : clauses) {
        for (int lit : clause) {
            int var = abs(lit);   // 正反 literal 一起算
            temp_counter[var]++;
        }
    }

    DLIS_order.clear();

    for (int var = 1; var <= maxVar; var++) {
        if (temp_counter[var] > 0) {
            DLIS_order.push_back(var);
        }
    }

    //排序 DLIS_counter，但依据是 temp_counter[var]
    sort(DLIS_order.begin(), DLIS_order.end(),
         [&](int a, int b) {
             if (temp_counter[a] != temp_counter[b]) {
                 return temp_counter[a] < temp_counter[b]; 
                 // 出现次数少的排前面
             }
             return a < b;
             // 出现次数相同，变量编号小的排前面
         });

    return true;
}

//note vector 支持随机访问
//note v[v.size() - 2]访问倒数第二个
 
// =========
bool readCNF(string filename, int& num_var, int& num_clause, vector<vector<int>>& clauses){
    ifstream fin(filename);
    if (!fin) {
        cerr << "fail to open file\n";
        return false;
    }

    string line;
    while (getline(fin, line)){
        if ((line.empty()) || (line[0] == 'c')) continue;
        if (line[0] == '%') break;

        //get number of literals and clauses
        if (line[0] == 'p') {
            string s1, s2;
            stringstream ss(line);
            ss >> s1 >> s2 >> num_var >> num_clause;
            continue;
        }
        
        //get clauses
        stringstream ss(line);
        int literal;
        vector<int> clause;
        while (ss >> literal) {
            if (literal == 0) break;
            clause.push_back(literal);
        }
         if (!clause.empty()) {
            clauses.push_back(clause);
        }
    }
    return true;
}

int literalValue(int literal, vector<int>& assignment){
    int var = abs(literal);
    if (assignment[var] == 0) return 0; //not assigned:0
    if (literal > 0) return assignment[var]; //x
    return -assignment[var]; //-x
}

bool isClauseSat(vector<int>& clause, vector<int>& assignment){
    monitor_stats.monitor_clause_checks++;
    for (int i=0; i<clause.size(); i++){
        int literal = clause[i];
        if (literalValue(literal, assignment) == 1) return true;
    }
    return false;
}

bool isClauseConflict(vector<int>& clause, vector<int>& assignment){
    monitor_stats.monitor_clause_checks++;
    for (int i=0; i<clause.size(); i++){
        int literal = clause[i];
        if(literalValue(literal, assignment) != -1) return false;
    }
    return true;
}

bool getUnitLiteral(vector<int>& clause, vector<int>& assignment, int& unit_literal){
    int unassignedLitCount = 0;
    int lastUnassigned = 0;
    for (int i=0; i<clause.size(); i++){
        int literal = clause[i];
        int val = literalValue(literal, assignment);
        if (val == 1) return false;
        if (val == 0){
            unassignedLitCount++;
            lastUnassigned = literal;
        }
    }
    if(unassignedLitCount == 1){
        unit_literal = lastUnassigned;
        return true;
    }
    return false;
}

//x: x=1, x': x=-1
void assignLiteral(int literal, vector<int>& assignment){
    monitor_stats.monitor_variable_assignments++;
    int var = abs(literal);
    if (literal > 0) assignment[var] = 1; 
    else assignment[var] = -1; 
}

bool unitPropagation(vector<vector<int>>& clauses, vector<int>& assignment){
    monitor_stats.monitor_unit_propagation_calls++;
    auto up_start = chrono::high_resolution_clock::now();
    
    bool changed = true;
    while (changed){
        monitor_stats.monitor_unit_propagation_rounds++;
        changed = false;
        for (int i=0; i<clauses.size(); i++){
            vector<int> clause = clauses[i];
            if (isClauseSat(clause, assignment)) continue;
            if (isClauseConflict(clause, assignment)) {
                auto up_end = chrono::high_resolution_clock::now();
                chrono::duration<double> up_elapsed = up_end - up_start;
                monitor_stats.monitor_unit_propagation_time += up_elapsed.count();
                return false;
            }
            int unit_Literal;
            if (getUnitLiteral(clause, assignment, unit_Literal)){
                assignLiteral(unit_Literal, assignment);
                changed = true;
            }
        }
    }
    
    auto up_end = chrono::high_resolution_clock::now();
    chrono::duration<double> up_elapsed = up_end - up_start;
    monitor_stats.monitor_unit_propagation_time += up_elapsed.count();
    return true;
}

bool allClauseSat(vector<vector<int>>& clauses, vector<int>& assignment){
    for (int i=0; i<clauses.size(); i++){
        vector<int> clause = clauses[i];
        monitor_stats.monitor_clause_checks++;
        if (!isClauseSat(clause, assignment)) return false;
    }
    return true;
}

int chooseVar(vector<vector<int>>& clauses, vector<int>& assignment, bool useDLIS, int& var){
    if (useDLIS) {
        var = chooseDLISLiteral(clauses, assignment); //var作为传入 返回值是是否成功
        return 1;
    } else {
        for (int i=1; i<assignment.size(); i++){
            if (assignment[i] == 0){
                var = i;
                return 1;
            }
        }
    }
    return 2; //1表示成功 2表示没有变量了
}

// Simple variable selection (used by watched literals dpll)
int chooseVar(vector<int>& assignment){
    for (int i=1; i<assignment.size(); i++){
        if (assignment[i] == 0) return i;
    }
    return -1;
}

//index used for watch list
int litToIndex(int literal, int num_var){
    if (literal > 0) return literal;
    return num_var + abs(literal);
}

//set up watch list (which literal is being watched)
void initWatchLists(vector<vector<int>>& clauses, vector<int>& watch1, vector<int>& watch2, vector<vector<int>>& watchList, int num_var){
    watch1.resize(clauses.size());
    watch2.resize(clauses.size());
    watchList.resize(2*num_var+1);

    for (int i=0; i<clauses.size(); i++){
        watch1[i] = 0;
        if (clauses[i].size() > 1) watch2[i] = 1;
        else watch2[i] = 0;

        int literal1 = clauses[i][watch1[i]];
        int literal2 = clauses[i][watch2[i]];

        watchList[litToIndex(literal1, num_var)].push_back(i);
        if (literal2 != literal1){
            watchList[litToIndex(literal2, num_var)].push_back(i);
        }
    }
}

bool allVarAssigned(vector<int>& assignment){
    for (int i=1; i<assignment.size(); i++){
        if (assignment[i] == 0) return false;
    }
    return true;
}

bool watchedUnitPropagation(vector<vector<int>>& clauses, vector<int>& assignment, vector<int>& watch1, vector<int>& watch2, vector<vector<int>>& watchList, queue<int>& propQ, int num_var){
    while(!propQ.empty()){
        //assign literal
        int literal = propQ.front();
        propQ.pop();
        int falseLit = -literal;
        int falseIndex = litToIndex(falseLit, num_var);
        vector<int> watchingClauses = watchList[falseIndex];

        for (int i=0; i<watchingClauses.size(); i++){
            //find w1, w2 index and value
            int clauseIndex = watchingClauses[i];
            int w1Index = watch1[clauseIndex];
            int w2Index = watch2[clauseIndex];
            int w1Literal = clauses[clauseIndex][w1Index];
            int w2Literal = clauses[clauseIndex][w2Index];

            int otherWIndex;
            int falseWNum;
            if (w1Literal == falseLit){ //w1 is false
                otherWIndex = w2Index;
                falseWNum = 1;
            }
            else if (w2Literal == falseLit){ //w2 is false
                otherWIndex = w1Index;
                falseWNum = 2;
            }
            else continue; //w1 and w2 are not assigned

            //for one w being assigned determine the next step
            int otherWLiteral = clauses[clauseIndex][otherWIndex];
            int otherWVal = literalValue(otherWLiteral, assignment);
            //1.the other w is true//
            if (otherWVal == 1) continue;
            //2.the other w is not true, able to find new w//
            bool findNewW = false;
            for (int j=0; j<clauses[clauseIndex].size(); j++){
                if (j == otherWIndex) continue;
                int newW = clauses[clauseIndex][j];
                if (literalValue(newW, assignment) != -1){
                    if (falseWNum == 1) watch1[clauseIndex] = j;
                    else watch2[clauseIndex] = j;

                    watchList[litToIndex(newW, num_var)].push_back(clauseIndex);
                    findNewW = true;
                    break;
                }
            }
            if (findNewW) continue;
            //3.the other w is not true, unable to find new w//
            else{
                //the other W is not assigned
                if (otherWVal == 0){
                    assignLiteral(otherWLiteral, assignment); 
                    propQ.push(otherWLiteral);  
                } 
                //the other W is false
                else return false;
            }
        }
    }
    return true;
}

bool dpll_watchedLit(vector<vector<int>>& clauses, vector<int>& assignment, vector<int>& watch1, vector<int>& watch2, vector<vector<int>>& watchList, queue<int>& propQ, int num_var){
    if (!watchedUnitPropagation(clauses, assignment, watch1, watch2, watchList, propQ, num_var)) return false;
    //if (allVarAssigned(assignment)) return true;

    if (allClauseSat(clauses, assignment)) return true;

    int var = 0;
    if (chooseVar(clauses, assignment, useDLIS_g, var) == 2) {
        monitor_stats.monitor_current_recursion_depth--;
        return false;
    }
    int firstLit = var;
    int secondLit = -var;

    vector<int> pre_assign = assignment;
    vector<int> pre_watch1 = watch1;
    vector<int> pre_watch2 = watch2;
    vector<vector<int>> pre_watchList = watchList;
    queue<int> pre_propQ = propQ;

    assignLiteral(firstLit, assignment);
    propQ.push(firstLit);
    if (dpll_watchedLit(clauses, assignment, watch1, watch2, watchList, propQ, num_var)){
        return true;
    }
    monitor_stats.monitor_backtrack_count++;
    assignment = pre_assign;
    watch1 = pre_watch1;
    watch2 = pre_watch2;
    watchList = pre_watchList;
    propQ = pre_propQ;

    assignLiteral(secondLit, assignment);
    propQ.push(secondLit);
    if (dpll_watchedLit(clauses, assignment, watch1, watch2, watchList, propQ, num_var)){
        return true;
    }
    monitor_stats.monitor_backtrack_count++;
    assignment = pre_assign;
    watch1 = pre_watch1;
    watch2 = pre_watch2;
    watchList = pre_watchList;
    propQ = pre_propQ;

    return false;
}

bool dpll(vector<vector<int>>& clauses, vector<int>& assignment, bool useDLIS, vector<int>& DLIS_order){
    monitor_stats.monitor_current_recursion_depth++;
    if (monitor_stats.monitor_current_recursion_depth > monitor_stats.monitor_max_recursion_depth) {
        monitor_stats.monitor_max_recursion_depth = monitor_stats.monitor_current_recursion_depth;
    }
    
    if (!unitPropagation(clauses, assignment)) {
        monitor_stats.monitor_current_recursion_depth--;
        return false;
    }
    if (allClauseSat(clauses, assignment)) {
        monitor_stats.monitor_current_recursion_depth--;
        return true;
    }

    int var = 0;
    if (chooseVar(clauses, assignment, useDLIS, var) == 2) {
        monitor_stats.monitor_current_recursion_depth--;
        return false;
    }
    //到这一步，var是选择的变量
    int first_value = var > 0 ? 1 : -1;
    var = abs(var);
    
    vector<int> pre_node = assignment;
    monitor_stats.monitor_state_copies++;
    
    assignment[var] = first_value;
    if (dpll(clauses, assignment, useDLIS, DLIS_order)){
        monitor_stats.monitor_current_recursion_depth--;
        return true;
    }
    monitor_stats.monitor_backtrack_count++;
    
    assignment = pre_node;
    monitor_stats.monitor_state_copies++;
    
    assignment[var] = -first_value;
    if (dpll(clauses, assignment, useDLIS, DLIS_order)){
        monitor_stats.monitor_current_recursion_depth--;
        return true;
    }
    monitor_stats.monitor_backtrack_count++;
    
    assignment = pre_node;
    monitor_stats.monitor_state_copies++;
    
    monitor_stats.monitor_current_recursion_depth--;
    return false;
}

bool verifySolution(vector<vector<int>>& clauses, vector<int>& assignment)
{
    for (int i = 0; i < clauses.size(); i++) {
        bool clauseSat = false;

        for (int j = 0; j < clauses[i].size(); j++) {
            if (literalValue(clauses[i][j], assignment) == 1) {
                clauseSat = true;
                break;
            }
        }

        if (!clauseSat) {
            cout << "Clause " << i + 1 << " is NOT satisfied: ";
            for (int j = 0; j < clauses[i].size(); j++) {
                cout << clauses[i][j] << " ";
            }
            cout << "0\n";
            return false;
        }
    }

    return true;
}
/*
void completeAssignment(vector<int>& assignment) {
    for (int i = 1; i < assignment.size(); i++) {
        if (assignment[i] == 0) {
            assignment[i] = 1;  //默认赋 true
        }
    }
}*/

bool Change_DLIS_FLAG(const string& value, bool& useDLIS) {
    if (value == "0") {
        useDLIS = false;
        useDLIS_g = 0;
        return true;
    }
    if (value == "1") {
        useDLIS = true;
        useDLIS_g = 1;
        return true;
    }
    return false;
}

bool Change_WatchedLit_FLAG(const string& value, bool& useWatchedLit) {
    if (value == "0") {
        useWatchedLit = false;
        return true;
    }
    if (value == "1") {
        useWatchedLit = true;
        return true;
    }
    return false;
}

bool Change_monitor_FLAG(const string& value, bool& monitor_FLAG) {
    if (value == "0") {
        monitor_FLAG = false;
        return true;
    }
    if (value == "1") {
        monitor_FLAG = true;
        return true;
    }
    return false;
}

int main(int argc, char* argv[]){ //输入参数有
    //示例 DPLL --DLIS 1
    //示例 DPLL --watched-literals 1
    //示例 DPLL --DLIS 1 --watched-literals 1
    bool useDLIS_FLAG = true;
    bool useWatchedLit_FLAG = true;
    bool monitor_FLAG = false;
    char* cnf_file = nullptr;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        string value;

        if (arg == "--DLIS") {
            if (i + 1 >= argc) { //检查是否有后续参数
                cerr << "error: --DLIS requires 0 or 1\n";
                return 1;
            }
            value = argv[++i];
            Change_DLIS_FLAG(value, useDLIS_FLAG);
        }

        if (arg == "--watched-literals") {
            if (i + 1 >= argc) { //检查是否有后续参数
                cerr << "error: --watched-literals requires 0 or 1\n";
                return 1;
            }
            value = argv[++i];
            Change_WatchedLit_FLAG(value, useWatchedLit_FLAG);
        }

        if (arg == "--monitor") {
            if (i + 1 >= argc) { //检查是否有后续参数
                cerr << "error: --monitor requires 0 or 1\n";
                return 1;
            }
            value = argv[++i];
            Change_monitor_FLAG(value, monitor_FLAG);
        }

        if (arg[0] != '-') {
            cnf_file = argv[i];
        }
    }

    if (cnf_file == nullptr) {
        cerr << "error: no CNF file specified\n";
        return 1;
    }
    
    monitor_start();
    
    int num_var = 0;
    int num_clause = 0;
    vector<vector<int>> clauses;

    if(!readCNF(cnf_file, num_var, num_clause, clauses)) return 1;

    vector<int> assignment(num_var+1,0); //assignment for global
    vector<int> DLIS_order(num_var+1,0); //assignment for counter of DLIS

    DLIS_Init(DLIS_order, clauses);

    bool result;
    if (useWatchedLit_FLAG){
        vector<int> watch1;
        vector<int> watch2;
        vector<vector<int>> watchList;
        queue<int> propQ;

        initWatchLists(clauses, watch1, watch2, watchList, num_var);
        for (int i = 0; i < (int)clauses.size(); i++) {
            if (clauses[i].size() == 1) {
                assignLiteral(clauses[i][0], assignment);
                propQ.push(clauses[i][0]);
            }
        }
        result = dpll_watchedLit(clauses, assignment, watch1, watch2, watchList, propQ, num_var);
    }
    else{
        result = dpll(clauses, assignment, useDLIS_FLAG, DLIS_order);
    }

    if (result){
        cout << "RESULT:SAT\n";
        cout << "ASSIGNMENT:";
        for (int i=1; i<= num_var; i++){
            if (assignment[i] == 1){
                cout << i << "=1";
            }
            else{
                cout << i << "=0";
            }
            if (i != num_var) cout << " ";
        }
        cout << "\n";

        if (!verifySolution(clauses, assignment)) {
            cout << "Wrong assignment\n";
        }
    }
    else{
        cout << "RESULT:UNSAT\n";
    }
    
    monitor_end();
    if (monitor_FLAG){monitor_print_results();}

    return 0;
}