#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

int **read_matrix_from_file(const char *filename, int *n_out);
void brute_force_tsp(int *path, int depth, int *visited, long long cost);
long long approximate_tsp();

static int **matrix = NULL;
static int n = 0;
static long long best_cost = LLONG_MAX;
static bool output_bruteforce = false;

int **read_matrix_from_file(const char *filename, int *n_out)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("failed to open file");
        exit(1);
    }

    int count = 0, tmp;
    char line[8192], *p;
    fgets(line, sizeof(line), f);

    for (p = line; sscanf(p, "%d", &tmp) == 1;) {
        count++;
        while (*p && *p != ' ' && *p != '\t' && *p != '\n') p++;
        while (*p == ' ' || *p == '\t') p++;
    }
    *n_out = count;

    rewind(f);
    int **m = malloc(count * sizeof(int *));
    for (int i = 0; i < count; i++) {
        m[i] = malloc(count * sizeof(int));
        for (int j = 0; j < count; j++)
            fscanf(f, "%d", &m[i][j]);
    }
    fclose(f);
    return m;
}

void brute_force_tsp(int *path, int depth, int *visited, long long cost)
{
    if (cost >= best_cost)
        return;

    if (depth == n) {
        cost += matrix[path[depth - 1]][0];
        if (cost < best_cost)
            best_cost = cost;

        return;
    }
    for (int v = 1; v < n; v++) {
        if (visited[v])
            continue;

        visited[v] = 1;
        path[depth] = v;
        brute_force_tsp(path, depth + 1, visited, cost + matrix[path[depth - 1]][v]);
        visited[v] = 0;
    }
}

long long approximate_tsp()
{
    int *parent = malloc(n * sizeof(int));
    long long *key = malloc(n * sizeof(long long));
    bool *used = calloc(n, sizeof(bool));

    for (int i = 0; i < n; i++) {
        key[i] = LLONG_MAX;
        parent[i] = -1;
    }

    key[0] = 0;
    for (int i = 0; i < n - 1; i++) {
        int u = -1;
        long long min = LLONG_MAX;

        for (int v = 0; v < n; v++)
            if (!used[v] && key[v] < min)
                min = key[u = v];
        used[u] = true;

        for (int v = 0; v < n; v++)
            if (matrix[u][v] && !used[v] && matrix[u][v] < key[v]) {
                key[v] = matrix[u][v];
                parent[v] = u;
            }
    }

    int **mst = calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++) 
        mst[i] = calloc(n, sizeof(int));

    for (int i = 1; i < n; i++) {
        int u = parent[i];
        if (u != -1) {
            mst[u][i] = matrix[u][i];
            mst[i][u] = matrix[u][i];
        }
    }

    bool *visited = calloc(n, sizeof(bool));
    int *path = malloc((n + 1) * sizeof(int));
    int path_len = 0;
    int current = 0, prev_index = -1;

    path[path_len++] = current;
    visited[current] = true;

    while (path_len < n) {
        bool found = false;

        for (int next = 0; next < n; next++) {
            if (mst[current][next] == 0 && mst[next][current] == 0)
                continue;
            if (visited[next])
                continue;
            path[path_len++] = next;
            visited[next] = true;
            current = next;
            prev_index = -1;
            found = true;
            break;
        }

        if (!found) {
            if (prev_index == -1) prev_index = path_len - 2;
            current = path[prev_index];
            prev_index--;
        }
    }

    path[path_len++] = 0;

    long long cost = 0;
    for (int i = 0; i < path_len - 1; i++)
        cost += matrix[path[i]][path[i + 1]];

    for (int i = 0; i < n; i++)
        free(mst[i]);
    free(mst);
    free(parent);
    free(key);
    free(used);
    free(visited);
    free(path);
    return cost;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <file> [-b|--brute-force]\n", argv[0]);
        printf("\n\t-b\t--brute-force\t\tRun and display brute force algorithm results.\n");
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--brute-force") == 0)
            output_bruteforce = true;
    }

    matrix = read_matrix_from_file(argv[1], &n);

    int *path = malloc((n + 1) * sizeof(int));
    int *visited = calloc(n, sizeof(int));
    path[0] = 0;
    visited[0] = 1;

    double bf_time = 0.0, mst_time = 0.0;
    long long mst_cost = 0;

    if (output_bruteforce) {
        clock_t start = clock();
        brute_force_tsp(path, 1, visited, 0);
        clock_t end = clock();
        bf_time = (double)(end - start) / CLOCKS_PER_SEC;
    }

    clock_t start = clock();
    mst_cost = approximate_tsp();
    clock_t end = clock();
    mst_time = (double)(end - start) / CLOCKS_PER_SEC;

    if (output_bruteforce) {
        printf("brute_force_cost: %lld\n", best_cost);
        printf("brute_force_time: %.6f s\n", bf_time);
    }
    printf("approx_tsp_cost: %lld\n", mst_cost);
    printf("approx_tsp_time: %.6f s\n", mst_time);

    for (int i = 0; i < n; i++) free(matrix[i]);
    free(matrix);
    free(path);
    free(visited);
    return 0;
}
