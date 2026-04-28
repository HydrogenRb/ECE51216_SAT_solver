#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <chrono>

using namespace std;

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
    printf("\n[monitor output] ========== PERFORMANCE ANALYSIS RESULTS ==========\n");
    printf("[monitor output] Total Execution Time: %.6f seconds\n", monitor_stats.monitor_total_time);
    printf("[monitor output] Unit Propagation Time: %.6f seconds (%.2f%% of total)\n", 
           monitor_stats.monitor_unit_propagation_time,
           (monitor_stats.monitor_total_time > 0) ? 
           (monitor_stats.monitor_unit_propagation_time / monitor_stats.monitor_total_time * 100) : 0);
    
    printf("\n[monitor output] --- Operation Counters ---\n");
    printf("[monitor output] Total Clause Checks: %lld\n", monitor_stats.monitor_clause_checks);
    printf("[monitor output] Variable Assignments: %lld\n", monitor_stats.monitor_variable_assignments);
    printf("[monitor output] Backtrack Operations: %lld\n", monitor_stats.monitor_backtrack_count);
    printf("[monitor output] State Copies: %lld\n", monitor_stats.monitor_state_copies);
    printf("[monitor output] Unit Propagation Calls: %lld\n", monitor_stats.monitor_unit_propagation_calls);
    printf("[monitor output] Unit Propagation Rounds: %lld\n", monitor_stats.monitor_unit_propagation_rounds);
    
    printf("\n[monitor output] --- Recursion Analysis ---\n");
    printf("[monitor output] Maximum Recursion Depth: %d\n", monitor_stats.monitor_max_recursion_depth);
    
    printf("\n[monitor output] --- Efficiency Metrics ---\n");
    if (monitor_stats.monitor_variable_assignments > 0) {
        printf("[monitor output] Avg Checks per Assignment: %.2f\n", 
               (double)monitor_stats.monitor_clause_checks / monitor_stats.monitor_variable_assignments);
    }
    if (monitor_stats.monitor_unit_propagation_calls > 0) {
        printf("[monitor output] Avg Rounds per UP Call: %.2f\n", 
               (double)monitor_stats.monitor_unit_propagation_rounds / monitor_stats.monitor_unit_propagation_calls);
    }
    printf("[monitor output] ============================================================\n\n");
}
// ==============================================================

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

int chooseVar(vector<int>& assignment){
    for (int i=1; i<assignment.size(); i++){
        if (assignment[i] == 0) return i;
    }
    return -1;
}

bool dpll(vector<vector<int>>& clauses, vector<int>& assignment){
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
    int var = chooseVar(assignment);
    if (var == -1) {
        monitor_stats.monitor_current_recursion_depth--;
        return false;
    }
    
    vector<int> pre_node = assignment;
    monitor_stats.monitor_state_copies++;
    
    assignment[var] = 1;
    monitor_stats.monitor_backtrack_count++;
    if (dpll(clauses, assignment)){
        monitor_stats.monitor_current_recursion_depth--;
        return true;
    }
    
    assignment = pre_node;
    monitor_stats.monitor_state_copies++;
    
    assignment[var] = -1;
    monitor_stats.monitor_backtrack_count++;
    if (dpll(clauses, assignment)){
        monitor_stats.monitor_current_recursion_depth--;
        return true;
    }
    
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

int main(int argc, char* argv[]){
    if (argc<2){
        cout << "Input format: ./mysAT benchmark.cnf\n";
        return 1;
    }
    
    monitor_start();
    
    int num_var = 0;
    int num_clause = 0;
    vector<vector<int>> clauses;

    if(!readCNF(argv[1], num_var, num_clause, clauses)) return 1;

    vector<int> assignment(num_var+1,0);

    if (dpll(clauses, assignment)){
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
    monitor_print_results();

    return 0;
}