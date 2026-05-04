#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <queue>

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

/*bool isClauseSat(vector<int>& clause, vector<int>& assignment){
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
}*/

//x: x=1, x': x=-1
void assignLiteral(int literal, vector<int>& assignment, queue<int>& propQ){
    int var = abs(literal);
    if (literal > 0) assignment[var] = 1; 
    else assignment[var] = -1;
    propQ.push(literal); 
}

/*bool unitPropagation(vector<vector<int>>& clauses, vector<int>& assignment, queue<int>& propQ){
    bool changed = true;
    while (changed){
        changed = false;
        for (int i=0; i<clauses.size(); i++){
            vector<int> clause = clauses[i];
            if (isClauseSat(clause, assignment)) continue;
            if (isClauseConflict(clause, assignment)) return false;
            int unit_Literal;
            if (getUnitLiteral(clause, assignment, unit_Literal)){
                assignLiteral(unit_Literal, assignment, propQ);
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
}*/

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
                if (otherWVal == 0) assignLiteral(otherWLiteral, assignment, propQ);
                //the other W is false
                else return false;
            }
        }
    }
    return true;
}

bool dpll(vector<vector<int>>& clauses, vector<int>& assignment, vector<int>& watch1, vector<int>& watch2, vector<vector<int>>& watchList, queue<int>& propQ, int num_var){
    if (!watchedUnitPropagation(clauses, assignment, watch1, watch2, watchList, propQ, num_var)) return false;
    if (allVarAssigned(assignment))return true;
    int var = chooseVar(assignment);
    if (var == -1) return false;

    vector<int> pre_assign = assignment;
    vector<int> pre_watch1 = watch1;
    vector<int> pre_watch2 = watch2;
    vector<vector<int>> pre_watchList = watchList;
    queue<int> pre_propQ = propQ;

    assignLiteral(var, assignment, propQ);
    if (dpll(clauses, assignment, watch1, watch2, watchList, propQ, num_var)){
        return true;
    }
    assignment = pre_assign;
    watch1 = pre_watch1;
    watch2 = pre_watch2;
    watchList = pre_watchList;
    propQ = pre_propQ;

    assignLiteral(-var, assignment, propQ);
    if (dpll(clauses, assignment, watch1, watch2, watchList, propQ, num_var)){
        return true;
    }
    assignment = pre_assign;
    watch1 = pre_watch1;
    watch2 = pre_watch2;
    watchList = pre_watchList;
    propQ = pre_propQ;

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

bool Change_DLIS_FLAG(const string& value, bool& useDLIS) {
    if (value == "0") {
        useDLIS = false;
        return true;
    }
    if (value == "1") {
        useDLIS = true;
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

int main(int argc, char* argv[]){
    bool useDLIS_FLAG = false;
    bool useWatchedLit_FLAG = false;
    for (int i = 0; i < argc; i++) {
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
    }
        
    int num_var = 0;
    int num_clause = 0;
    vector<vector<int>> clauses;

    vector<int> watch1;
    vector<int> watch2;
    vector<vector<int>> watchList;

    queue<int> propQ;

    if(!readCNF(argv[1], num_var, num_clause, clauses)) return 1;

    vector<int> assignment(num_var+1,0);
    initWatchLists(clauses, watch1, watch2, watchList, num_var);
    for (int i = 0; i < clauses.size(); i++) {
        if (clauses[i].size() == 1) {
            assignLiteral(clauses[i][0], assignment, propQ);
        }
    }

    /*cout << "Watch lists initialized\n";
    for (int i = 1; i <= 2 * num_var; i++) {
        cout << "watchList[" << i << "]: ";
        for (int j = 0; j < watchList[i].size(); j++) {
            cout << watchList[i][j] << " ";
        }
        cout << "\n";
    }*/

    if (dpll(clauses, assignment, watch1, watch2, watchList, propQ, num_var)){
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