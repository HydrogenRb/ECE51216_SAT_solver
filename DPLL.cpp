#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>

using namespace std;

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
    for (int i=0; i<clause.size(); i++){
        int literal = clause[i];
        if (literalValue(literal, assignment) == 1) return true;
    }
    return false;
}

bool isClauseConflict(vector<int>& clause, vector<int>& assignment){
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
    int var = abs(literal);
    if (literal > 0) assignment[var] = 1; 
    else assignment[var] = -1; 
}

bool unitPropagation(vector<vector<int>>& clauses, vector<int>& assignment){
    bool changed = true;
    while (changed){
        changed = false;
        for (int i=0; i<clauses.size(); i++){
            vector<int> clause = clauses[i];
            if (isClauseSat(clause, assignment)) continue;
            if (isClauseConflict(clause, assignment)) return false;
            int unit_Literal;
            if (getUnitLiteral(clause, assignment, unit_Literal)){
                assignLiteral(unit_Literal, assignment);
                changed = true;
            }
        }
    }
    return true;
}

bool allClauseSat(vector<vector<int>>& clauses, vector<int>& assignment){
    for (int i=0; i<clauses.size(); i++){
        vector<int> clause = clauses[i];
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
    if (!unitPropagation(clauses, assignment)) return false;
    if (allClauseSat(clauses, assignment)) return true;
    int var = chooseVar(assignment);
    if (var == -1) return false;
    vector<int> pre_node = assignment;
    assignment[var] = 1;
    if (dpll(clauses, assignment)){
        return true;
    }
    assignment = pre_node;
    assignment[var] = -1;
    if (dpll(clauses, assignment)){
        return true;
    }
    assignment = pre_node;
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

    return 0;
}