#include <stdio.h>
#include <stdlib.h>

int** read_graph_from_file(const char* filename, int* n_out);
void display_graph(int** g, int n);
void usage(char *program_name);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    int n;
    int** g = read_graph_from_file(argv[1], &n);

    display_graph(g, n);

    for (int i = 0; i < n; i++)
        free(g[i]);
    free(g);


    return 0;
}

int** read_graph_from_file(const char* filename, int* n_out) {
    FILE* f;

    f = fopen(filename, "r");

    if (!f) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    char line[1024];
    if (!fgets(line, sizeof(line), f)) {
        fprintf(stderr, "file is empty\n");
        exit(EXIT_FAILURE);
    }

    char* p = line;
    int tmp;
    while (sscanf(p, "%d", &tmp) == 1) {
        count++;
        while (*p && *p != ' ' && *p != '\t') p++;
        while (*p == ' ' || *p == '\t') p++;
    }

    int n = count;
    *n_out = n;

    int** graph = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
        graph[i] = malloc(n * sizeof(int));

    rewind(f);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fscanf(f, "%d", &graph[i][j]);

    fclose(f);

    return graph;
}

void display_graph(int** g, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%4d ", g[i][j]);
        printf("\n");
    }
}

void usage(char *program_name)
{
    printf("usage: %s <file>\n", program_name);
}
