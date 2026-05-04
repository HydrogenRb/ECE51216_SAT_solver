#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <chrono>

using namespace std;

namespace {

int literalValue(int literal, const vector<int>& assignment) {
    int var = abs(literal);
    if (assignment[var] == 0) return 0;
    if (literal > 0) return assignment[var];
    return -assignment[var];
}

bool isClauseSat(const vector<int>& clause, const vector<int>& assignment) {
    for (int literal : clause) {
        if (literalValue(literal, assignment) == 1) return true;
    }
    return false;
}
}

int chooseDLISLiteral(const vector<vector<int>>& clauses, const vector<int>& assignment) {
    vector<int> positive_count(assignment.size(), 0);
    vector<int> negative_count(assignment.size(), 0);

    for (const vector<int>& clause : clauses) {
        if (isClauseSat(clause, assignment)) continue;

        for (int literal : clause) {
            int var = abs(literal);
            if (assignment[var] != 0) continue;

            if (literal > 0) positive_count[var]++;
            else negative_count[var]++;
        }
    }

    int best_literal = 0;
    int best_count = -1;

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
