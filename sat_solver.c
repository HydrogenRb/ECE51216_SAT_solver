// AI生成，用于功能参考
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '\\'
#else
#include <dirent.h>
#define PATH_SEP '/'
#endif

#ifdef _WIN32
#define TOKENIZE_NEXT strtok_s
#else
#define TOKENIZE_NEXT strtok_r
#endif

typedef struct {
    int **clauses;
    int *sizes;
    int clause_count;
} Formula;

typedef struct {
    char **items;
    int count;
    int cap;
} StringList;

static void list_init(StringList *list) {
    list->items = NULL;
    list->count = 0;
    list->cap = 0;
}

static char *str_dup(const char *s) {
    size_t n = strlen(s);
    char *out = (char *)malloc(n + 1);
    if (!out) return NULL;
    memcpy(out, s, n + 1);
    return out;
}

static int list_push(StringList *list, const char *path) {
    if (list->count == list->cap) {
        int new_cap = (list->cap == 0) ? 32 : list->cap * 2;
        char **new_items = (char **)realloc(list->items, sizeof(char *) * new_cap);
        if (!new_items) return 0;
        list->items = new_items;
        list->cap = new_cap;
    }
    list->items[list->count] = str_dup(path);
    if (!list->items[list->count]) return 0;
    list->count++;
    return 1;
}

static void list_free(StringList *list) {
    int i;
    for (i = 0; i < list->count; ++i) {
        free(list->items[i]);
    }
    free(list->items);
}

static int has_cnf_suffix(const char *name) {
    size_t n = strlen(name);
    if (n < 4) return 0;
    return (name[n - 4] == '.' &&
            (name[n - 3] == 'c' || name[n - 3] == 'C') &&
            (name[n - 2] == 'n' || name[n - 2] == 'N') &&
            (name[n - 1] == 'f' || name[n - 1] == 'F'));
}

static int cmp_strptr(const void *a, const void *b) {
    const char *sa = *(const char *const *)a;
    const char *sb = *(const char *const *)b;
    return strcmp(sa, sb);
}

static void formula_free(Formula *f) {
    int i;
    if (!f) return;
    for (i = 0; i < f->clause_count; ++i) {
        free(f->clauses[i]);
    }
    free(f->clauses);
    free(f->sizes);
    f->clauses = NULL;
    f->sizes = NULL;
    f->clause_count = 0;
}

static int formula_copy(const Formula *src, Formula *dst) {
    int i;
    dst->clause_count = src->clause_count;
    dst->clauses = (int **)calloc((size_t)dst->clause_count, sizeof(int *));
    dst->sizes = (int *)calloc((size_t)dst->clause_count, sizeof(int));
    if (!dst->clauses || !dst->sizes) {
        formula_free(dst);
        return 0;
    }
    for (i = 0; i < dst->clause_count; ++i) {
        int size = src->sizes[i];
        dst->sizes[i] = size;
        dst->clauses[i] = (int *)malloc(sizeof(int) * (size_t)size);
        if (!dst->clauses[i]) {
            formula_free(dst);
            return 0;
        }
        memcpy(dst->clauses[i], src->clauses[i], sizeof(int) * (size_t)size);
    }
    return 1;
}

static int parse_dimacs(const char *path, int *num_vars, Formula *out) {
    FILE *fp = fopen(path, "r");
    char line[4096];
    int cap = 256;
    int **clauses = NULL;
    int *sizes = NULL;
    int clause_count = 0;
    int *current = NULL;
    int cur_size = 0;
    int cur_cap = 0;

    if (!fp) return 0;

    clauses = (int **)calloc((size_t)cap, sizeof(int *));
    sizes = (int *)calloc((size_t)cap, sizeof(int));
    if (!clauses || !sizes) {
        fclose(fp);
        free(clauses);
        free(sizes);
        return 0;
    }

    *num_vars = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *tok;
        char *save = NULL;
        char *trim = line;

        while (*trim == ' ' || *trim == '\t' || *trim == '\n' || *trim == '\r') trim++;
        if (*trim == '\0' || *trim == 'c' || *trim == '%') continue;
        if (*trim == 'p') {
            char hdr[16], kind[16];
            int vars = 0, cls = 0;
            if (sscanf(trim, "%15s %15s %d %d", hdr, kind, &vars, &cls) == 4) {
                if (strcmp(kind, "cnf") == 0) *num_vars = vars;
            }
            continue;
        }

        tok = TOKENIZE_NEXT(trim, " \t\r\n", &save);
        while (tok) {
            char *endptr = NULL;
            long value;
            if (tok[0] == '%') break;
            value = strtol(tok, &endptr, 10);
            if (endptr == tok || *endptr != '\0') break;
            if (value > INT_MAX || value < INT_MIN) break;
            int lit = (int)value;
            if (lit == 0) {
                int *cl = NULL;
                if (clause_count == cap) {
                    int new_cap = cap * 2;
                    int **new_clauses = (int **)realloc(clauses, sizeof(int *) * (size_t)new_cap);
                    int *new_sizes = (int *)realloc(sizes, sizeof(int) * (size_t)new_cap);
                    if (!new_clauses || !new_sizes) {
                        free(new_clauses);
                        free(new_sizes);
                        fclose(fp);
                        free(current);
                        out->clauses = clauses;
                        out->sizes = sizes;
                        out->clause_count = clause_count;
                        formula_free(out);
                        return 0;
                    }
                    clauses = new_clauses;
                    sizes = new_sizes;
                    cap = new_cap;
                }
                cl = (int *)malloc(sizeof(int) * (size_t)cur_size);
                if (cur_size > 0 && !cl) {
                    fclose(fp);
                    free(current);
                    out->clauses = clauses;
                    out->sizes = sizes;
                    out->clause_count = clause_count;
                    formula_free(out);
                    return 0;
                }
                if (cur_size > 0) memcpy(cl, current, sizeof(int) * (size_t)cur_size);
                clauses[clause_count] = cl;
                sizes[clause_count] = cur_size;
                clause_count++;
                cur_size = 0;
            } else {
                if (cur_size == cur_cap) {
                    int new_cap2 = (cur_cap == 0) ? 8 : cur_cap * 2;
                    int *new_current = (int *)realloc(current, sizeof(int) * (size_t)new_cap2);
                    if (!new_current) {
                        fclose(fp);
                        free(current);
                        out->clauses = clauses;
                        out->sizes = sizes;
                        out->clause_count = clause_count;
                        formula_free(out);
                        return 0;
                    }
                    current = new_current;
                    cur_cap = new_cap2;
                }
                current[cur_size++] = lit;
            }
            tok = TOKENIZE_NEXT(NULL, " \t\r\n", &save);
        }
    }

    free(current);
    fclose(fp);

    out->clauses = clauses;
    out->sizes = sizes;
    out->clause_count = clause_count;
    return 1;
}

static int assign_lit(int *assignment, int lit) {
    int v = abs(lit);
    int val = (lit > 0) ? 1 : 0;
    if (assignment[v] != -1 && assignment[v] != val) return 0;
    assignment[v] = val;
    return 1;
}

static int reduce_formula(const Formula *src, int lit, Formula *dst) {
    int i, j, out_count = 0;
    int neg = -lit;

    dst->clauses = (int **)calloc((size_t)src->clause_count, sizeof(int *));
    dst->sizes = (int *)calloc((size_t)src->clause_count, sizeof(int));
    dst->clause_count = 0;
    if (!dst->clauses || !dst->sizes) {
        formula_free(dst);
        return 0;
    }

    for (i = 0; i < src->clause_count; ++i) {
        int size = src->sizes[i];
        int satisfied = 0;
        int keep_count = 0;

        for (j = 0; j < size; ++j) {
            if (src->clauses[i][j] == lit) {
                satisfied = 1;
                break;
            }
        }
        if (satisfied) continue;

        for (j = 0; j < size; ++j) {
            if (src->clauses[i][j] != neg) keep_count++;
        }
        if (keep_count == 0) {
            formula_free(dst);
            return -1;
        }

        dst->clauses[out_count] = (int *)malloc(sizeof(int) * (size_t)keep_count);
        if (!dst->clauses[out_count]) {
            formula_free(dst);
            return 0;
        }
        dst->sizes[out_count] = keep_count;

        keep_count = 0;
        for (j = 0; j < size; ++j) {
            int x = src->clauses[i][j];
            if (x != neg) dst->clauses[out_count][keep_count++] = x;
        }
        out_count++;
    }

    dst->clause_count = out_count;
    return 1;
}

static int choose_literal(const Formula *f, const int *assignment) {
    int i, j;
    for (i = 0; i < f->clause_count; ++i) {
        for (j = 0; j < f->sizes[i]; ++j) {
            int lit = f->clauses[i][j];
            if (assignment[abs(lit)] == -1) return lit;
        }
    }
    return 0;
}

static int find_unit(const Formula *f, int *out_lit) {
    int i;
    for (i = 0; i < f->clause_count; ++i) {
        if (f->sizes[i] == 1) {
            *out_lit = f->clauses[i][0];
            return 1;
        }
    }
    return 0;
}

static int find_pure(const Formula *f, const int *assignment, int num_vars, int *out_lit) {
    int *sign = NULL;
    int i, j;
    sign = (int *)calloc((size_t)(num_vars + 1), sizeof(int));
    if (!sign) return 0;

    for (i = 0; i < f->clause_count; ++i) {
        for (j = 0; j < f->sizes[i]; ++j) {
            int lit = f->clauses[i][j];
            int v = abs(lit);
            if (assignment[v] != -1) continue;
            if (lit > 0) sign[v] |= 1;
            else sign[v] |= 2;
        }
    }

    for (i = 1; i <= num_vars; ++i) {
        if (assignment[i] != -1) continue;
        if (sign[i] == 1) {
            *out_lit = i;
            free(sign);
            return 1;
        }
        if (sign[i] == 2) {
            *out_lit = -i;
            free(sign);
            return 1;
        }
    }
    free(sign);
    return 0;
}

static int has_empty_clause(const Formula *f) {
    int i;
    for (i = 0; i < f->clause_count; ++i) {
        if (f->sizes[i] == 0) return 1;
    }
    return 0;
}

static int dpll(const Formula *formula, int num_vars, int *assignment) {
    Formula working = {0};
    int changed = 1;

    if (!formula_copy(formula, &working)) return 0;

    while (changed) {
        int unit = 0, pure = 0;
        changed = 0;

        if (working.clause_count == 0) {
            formula_free(&working);
            return 1;
        }
        if (has_empty_clause(&working)) {
            formula_free(&working);
            return 0;
        }

        while (find_unit(&working, &unit)) {
            Formula next = {0};
            int res;
            if (!assign_lit(assignment, unit)) {
                formula_free(&working);
                return 0;
            }
            res = reduce_formula(&working, unit, &next);
            formula_free(&working);
            if (res <= 0) {
                formula_free(&next);
                return 0;
            }
            working = next;
            changed = 1;
            if (working.clause_count == 0) {
                formula_free(&working);
                return 1;
            }
        }

        if (find_pure(&working, assignment, num_vars, &pure)) {
            Formula next2 = {0};
            int res2;
            if (!assign_lit(assignment, pure)) {
                formula_free(&working);
                return 0;
            }
            res2 = reduce_formula(&working, pure, &next2);
            formula_free(&working);
            if (res2 <= 0) {
                formula_free(&next2);
                return 0;
            }
            working = next2;
            changed = 1;
        }
    }

    if (working.clause_count == 0) {
        formula_free(&working);
        return 1;
    }

    {
        int lit = choose_literal(&working, assignment);
        int branch_vals[2];
        int b;
        if (lit == 0) {
            formula_free(&working);
            return 1;
        }
        branch_vals[0] = lit;
        branch_vals[1] = -lit;

        for (b = 0; b < 2; ++b) {
            Formula reduced = {0};
            int *branch_assign = (int *)malloc(sizeof(int) * (size_t)(num_vars + 1));
            int ok_reduce, sat;
            if (!branch_assign) continue;
            memcpy(branch_assign, assignment, sizeof(int) * (size_t)(num_vars + 1));
            if (!assign_lit(branch_assign, branch_vals[b])) {
                free(branch_assign);
                continue;
            }
            ok_reduce = reduce_formula(&working, branch_vals[b], &reduced);
            if (ok_reduce <= 0) {
                formula_free(&reduced);
                free(branch_assign);
                continue;
            }
            sat = dpll(&reduced, num_vars, branch_assign);
            formula_free(&reduced);
            if (sat) {
                memcpy(assignment, branch_assign, sizeof(int) * (size_t)(num_vars + 1));
                free(branch_assign);
                formula_free(&working);
                return 1;
            }
            free(branch_assign);
        }
    }

    formula_free(&working);
    return 0;
}

static int solve_file(const char *path, int show_model) {
    Formula f = {0};
    int num_vars = 0;
    int sat = 0;
    int i;
    int *assignment = NULL;

    if (!parse_dimacs(path, &num_vars, &f)) {
        printf("%s: ERROR (cannot parse)\n", path);
        return -1;
    }
    if (num_vars < 0) num_vars = 0;

    assignment = (int *)malloc(sizeof(int) * (size_t)(num_vars + 1));
    if (!assignment) {
        formula_free(&f);
        printf("%s: ERROR (out of memory)\n", path);
        return -1;
    }
    for (i = 0; i <= num_vars; ++i) assignment[i] = -1;

    sat = dpll(&f, num_vars, assignment);
    printf("%s: %s\n", path, sat ? "SAT" : "UNSAT");
    if (sat && show_model) {
        printf("  v");
        for (i = 1; i <= num_vars; ++i) {
            int val = assignment[i];
            if (val == 1) printf(" %d", i);
            else printf(" %d", -i);
        }
        printf(" 0\n");
    }

    free(assignment);
    formula_free(&f);
    return sat ? 1 : 0;
}

#ifdef _WIN32
static int collect_recursive_win(const char *dir, StringList *out) {
    char pattern[MAX_PATH];
    WIN32_FIND_DATAA ffd;
    HANDLE hFind;

    snprintf(pattern, sizeof(pattern), "%s\\*", dir);
    hFind = FindFirstFileA(pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) return 1;

    do {
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) continue;
        {
            char full[MAX_PATH];
            snprintf(full, sizeof(full), "%s\\%s", dir, ffd.cFileName);
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                collect_recursive_win(full, out);
            } else if (has_cnf_suffix(ffd.cFileName)) {
                if (!list_push(out, full)) {
                    FindClose(hFind);
                    return 0;
                }
            }
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
    return 1;
}
#else
static int collect_recursive_posix(const char *dir, StringList *out) {
    DIR *dp = opendir(dir);
    struct dirent *ent;
    if (!dp) return 1;
    while ((ent = readdir(dp)) != NULL) {
        char full[PATH_MAX];
        struct stat st;
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        snprintf(full, sizeof(full), "%s/%s", dir, ent->d_name);
        if (stat(full, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            collect_recursive_posix(full, out);
        } else if (S_ISREG(st.st_mode) && has_cnf_suffix(ent->d_name)) {
            if (!list_push(out, full)) {
                closedir(dp);
                return 0;
            }
        }
    }
    closedir(dp);
    return 1;
}
#endif

static int collect_cnf(const char *target, StringList *out) {
    struct stat st;
    if (stat(target, &st) != 0) {
        fprintf(stderr, "Path not found: %s\n", target);
        return 0;
    }
    if (S_ISREG(st.st_mode)) {
        if (!has_cnf_suffix(target)) {
            fprintf(stderr, "Not a .cnf file: %s\n", target);
            return 0;
        }
        return list_push(out, target);
    }
    if (S_ISDIR(st.st_mode)) {
#ifdef _WIN32
        return collect_recursive_win(target, out);
#else
        return collect_recursive_posix(target, out);
#endif
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *target = ".";
    int show_model = 0;
    int sat_count = 0;
    int unsat_or_err = 0;
    int i;
    StringList files;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--model") == 0) {
            show_model = 1;
        } else {
            target = argv[i];
        }
    }

    list_init(&files);
    if (!collect_cnf(target, &files)) {
        list_free(&files);
        return 1;
    }
    if (files.count == 0) {
        printf("No .cnf files found under: %s\n", target);
        list_free(&files);
        return 0;
    }

    qsort(files.items, (size_t)files.count, sizeof(char *), cmp_strptr);

    for (i = 0; i < files.count; ++i) {
        int r = solve_file(files.items[i], show_model);
        if (r == 1) sat_count++;
        else unsat_or_err++;
    }

    printf("Summary: total=%d, SAT=%d, UNSAT/ERROR=%d\n", files.count, sat_count, unsat_or_err);
    list_free(&files);
    return 0;
}
