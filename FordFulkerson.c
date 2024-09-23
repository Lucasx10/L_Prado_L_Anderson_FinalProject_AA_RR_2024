#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

int numberOfNodes, numberOfTasks, numberOfWorkers;
int *parent;

// Função para gerar a saída, mostrando quais tarefas foram atribuídas a quais trabalhadores
void generateOutput(int **residualGraph, int **originalGraph) {
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        for (int j = numberOfTasks + 3; j < numberOfNodes; j++) {
            if (originalGraph[i][j] == 1 && residualGraph[i][j] == 0) {
                printf("TaskID %d : Worker ID %d\n", i - 1, j - numberOfTasks - 2);
            }
        }
    }
}

void generateAssignmentDotFile(int **residualGraph, int **originalGraph, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao abrir o arquivo %s para escrita!\n", filename);
        return;
    }

    fprintf(file, "digraph Assignment {\n");
    fprintf(file, "  node [shape=circle];\n");

    for (int taskNode = 2; taskNode <= numberOfTasks + 1; taskNode++) {
        for (int workerNode = numberOfTasks + 3; workerNode < numberOfNodes; workerNode++) {
            if (originalGraph[taskNode][workerNode] == 1 && residualGraph[taskNode][workerNode] == 0) {
                fprintf(file, "  Tarefa%d -> Funcionario%d;\n", taskNode - 1, workerNode - numberOfTasks - 2);
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);
}

bool dfsVisit(int **graph, int u, int sink, bool *visited) {
    visited[u] = true;
    if (u == sink) {
        return true;
    }

    for (int v = 1; v <= numberOfNodes; v++) {
        if (!visited[v] && graph[u][v] > 0) {
            parent[v] = u;
            if (dfsVisit(graph, v, sink, visited)) {
                return true;
            }
        }
    }
    return false;
}

bool hasPath(int **graph, int source, int sink) {
    bool *visited = (bool *)calloc(numberOfNodes + 1, sizeof(bool));
    for (int i = 0; i <= numberOfNodes; i++) {
        parent[i] = -1;
    }
    bool result = dfsVisit(graph, source, sink, visited);
    free(visited);
    return result;
}

void fordFulkerson(int **graph, int source, int sink) {
    int **residualGraph = (int **)malloc((numberOfNodes + 1) * sizeof(int *));
    for (int i = 0; i <= numberOfNodes; i++) {
        residualGraph[i] = (int *)malloc((numberOfNodes + 1) * sizeof(int));
    }

    for (int i = 1; i <= numberOfNodes; i++) {
        for (int j = 1; j <= numberOfNodes; j++) {
            residualGraph[i][j] = graph[i][j];
        }
    }

    while (hasPath(residualGraph, source, sink)) {
        int pathFlow = INT_MAX;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = pathFlow < residualGraph[u][v] ? pathFlow : residualGraph[u][v];
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }
    }
    
    //Para visualizar as atribuições das tarefas via terminal basta descomentar a linha abaixo
    // generateOutput(residualGraph, graph); 
    //Para gerar o arquivo DotFile basta descomentar a linha abaixo 
    // generateAssignmentDotFile(residualGraph, graph, "FordFulkersonGraph.dot");
 
    for (int i = 0; i <= numberOfNodes; i++) {
        free(residualGraph[i]);
    }
    free(residualGraph);
}

void getGraph(const char *filename) {
    clock_t inicio, fim;
    double tempo_decorrido;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", &numberOfWorkers) != 1) {
        fprintf(stderr, "Erro ao ler o número de trabalhadores.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", &numberOfTasks) != 1) {
        fprintf(stderr, "Erro ao ler o número de tarefas.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    numberOfNodes = numberOfWorkers + numberOfTasks + 3;

    // Alocação dinâmica de memória para 'parent'
    parent = (int *)malloc((numberOfNodes + 1) * sizeof(int));

    // Alocação dinâmica de memória para 'graph'
    int **graph = (int **)malloc((numberOfNodes + 1) * sizeof(int *));
    for (int i = 0; i <= numberOfNodes; i++) {
        graph[i] = (int *)malloc((numberOfNodes + 1) * sizeof(int));
        for (int j = 0; j <= numberOfNodes; j++) {
            graph[i][j] = 0;
        }
    }

    for (int i = 2; i <= numberOfTasks + 1; i++) {
        graph[1][i] = 1;
    }

    for (int i = numberOfTasks + 3; i < numberOfNodes; i++) {
        graph[i][numberOfTasks + 2] = 1;
    }

    for (int i = 0; i < numberOfTasks; i++) {
        int workerID;
        while (fscanf(file, "%d", &workerID) == 1 && workerID != 0) {
            if (workerID > 0 && workerID <= numberOfWorkers) {
                int columnNumber = workerID + numberOfTasks + 2;
                graph[i + 2][columnNumber] = 1;
            }
        }
        while (fgetc(file) != '\n' && !feof(file));
    }

    fclose(file);

    inicio = clock();
    fordFulkerson(graph, 1, numberOfTasks + 2);
    fim = clock();

    tempo_decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("%.10f\n", tempo_decorrido);

    for (int i = 0; i <= numberOfNodes; i++) {
        free(graph[i]);
    }
    free(graph);

    // Liberar memória alocada dinamicamente para 'parent'
    free(parent);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    getGraph(argv[1]);
    return 0;
}
