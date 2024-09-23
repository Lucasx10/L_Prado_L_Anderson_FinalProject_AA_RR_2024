#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

int numberOfNodes, numberOfEmployees, numberOfTasks;
int *parent;

// Função para verificar se há um caminho usando BFS
bool hasPath(int **graph, int source, int sink) {
    bool *visited = (bool *)malloc((numberOfNodes + 1) * sizeof(bool));
    int *queue = (int *)malloc((numberOfNodes + 1) * sizeof(int));
    int front = 0, rear = 0;

    // Inicializa o array de pais
    for (int i = 0; i <= numberOfNodes; i++) {
        parent[i] = -1;
        visited[i] = false;
    }

    // Inicia a BFS
    queue[rear++] = source;
    visited[source] = true;

    while (front < rear) {
        int u = queue[front++];
        for (int v = 1; v <= numberOfNodes; v++) {
            if (!visited[v] && graph[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = true;
                if (v == sink) return true; // Se alcançamos o sumidouro
            }
        }
    }
    free(visited);
    free(queue);
    return false; // Se não há caminho
}

// Função para gerar o arquivo dot com as atribuições
void generateAssignmentDotFile(int **residualGraph, int **originalGraph, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao abrir o arquivo %s para escrita!\n", filename);
        return;
    }

    fprintf(file, "digraph Assignment {\n");
    fprintf(file, "  node [shape=circle];\n");

    // Iterar sobre os nós de tarefas e funcionários
    for (int taskNode = 2; taskNode <= numberOfTasks + 1; taskNode++) {
        for (int workerNode = numberOfTasks + 3; workerNode <= numberOfNodes; workerNode++) {
            // Se a aresta foi usada e está agora com fluxo zero
            if (originalGraph[taskNode][workerNode] == 1 && residualGraph[taskNode][workerNode] == 0) {
                fprintf(file, "  Tarefa%d -> Funcionario%d;\n", taskNode - 1, workerNode - numberOfTasks - 2);
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);
}

// Gera a saída final com as atribuições de tarefas
void generateOutput(int **residualGraph, int **originalGraph) {
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        for (int j = numberOfTasks + 3; j <= numberOfNodes; j++) {
            // Se a aresta foi usada e está agora com fluxo zero
            if (originalGraph[i][j] == 1 && residualGraph[i][j] == 0) {
                printf("TaskID %d : Worker ID %d\n", i - 1, j - numberOfTasks - 2);
            }
        }
    }
}

void edmondsKarp(int **graph, int source, int sink) {
    int **residualGraph = (int **)malloc((numberOfNodes + 1) * sizeof(int *));
    for (int i = 0; i <= numberOfNodes; i++) {
        residualGraph[i] = (int *)malloc((numberOfNodes + 1) * sizeof(int));
    }

    // Inicializa o grafo residual com o original
    for (int i = 1; i <= numberOfNodes; i++) {
        for (int j = 1; j <= numberOfNodes; j++) {
            residualGraph[i][j] = graph[i][j];
        }
    }

    // Enquanto houver caminho no grafo residual
    while (hasPath(residualGraph, source, sink)) {
        int pathFlow = INT_MAX;
        // Encontra o fluxo mínimo no caminho encontrado
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = pathFlow < residualGraph[u][v] ? pathFlow : residualGraph[u][v];
        }

        // Atualiza as capacidades no grafo residual
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }
    }

    //Para visualizar as atribuições das tarefas via terminal basta descomentar a linha abaixo
    // generateOutput(residualGraph, graph);
    //Para gerar o arquivo DotFile basta descomentar a linha abaixo 
    // generateAssignmentDotFile(residualGraph, graph, "EdmondKarpGraph.dot");

    // Libera a memória alocada dinamicamente para o grafo residual
    for (int i = 0; i <= numberOfNodes; i++) {
        free(residualGraph[i]);
    }
    free(residualGraph);
}

// Função para ler o grafo a partir do arquivo
void getGraph(const char *filename) {
    clock_t inicio, fim;
    double tempo_decorrido;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Lê o número de funcionários e tarefas
    if (fscanf(file, "%d", &numberOfEmployees) != 1) {
        fprintf(stderr, "Erro ao ler o número de funcionários.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", &numberOfTasks) != 1) {
        fprintf(stderr, "Erro ao ler o número de tarefas.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    numberOfNodes = numberOfEmployees + numberOfTasks + 3;

    if (numberOfNodes > 2500) {
        fprintf(stderr, "Numero de nos (%d) excede o limite maximo (%d).\n", numberOfNodes, 1500);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Aloca dinamicamente a matriz `graph`
    int **graph = (int **)malloc((numberOfNodes + 1) * sizeof(int *));
    for (int i = 0; i <= numberOfNodes; i++) {
        graph[i] = (int *)malloc((numberOfNodes + 1) * sizeof(int));
    }

    // Inicializa o grafo com zeros
    for (int i = 0; i <= numberOfNodes; i++) {
        for (int j = 0; j <= numberOfNodes; j++) {
            graph[i][j] = 0;
        }
    }

    // Conexões da fonte para as tarefas
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        graph[1][i] = 1;
    }

    // Conexões dos funcionários para o sumidouro
    for (int i = numberOfTasks + 3; i <= numberOfNodes; i++) {
        graph[i][numberOfTasks + 2] = 1;
    }

    // Leitura das conexões tarefas-funcionários do arquivo
    for (int i = 0; i < numberOfTasks; i++) {
        int workerID;
        while (fscanf(file, "%d", &workerID) == 1 && workerID != 0) {
            if (workerID > 0 && workerID <= numberOfEmployees) {
                int columnNumber = workerID + numberOfTasks + 2;
                if (columnNumber <= numberOfNodes) {  // Verifica se o nó está dentro do limite
                    graph[i + 2][columnNumber] = 1;
                }
            }
        }
    }

    // Aloca dinamicamente o array `parent`
    parent = (int *)malloc((numberOfNodes + 1) * sizeof(int));

    inicio = clock();
    edmondsKarp(graph, 1, numberOfTasks + 2);
    fim = clock();

    tempo_decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // Exibe o tempo decorrido
    printf("%.10f\n", tempo_decorrido);

    // Libera a memória alocada dinamicamente para o grafo e o array `parent`
    for (int i = 0; i <= numberOfNodes; i++) {
        free(graph[i]);
    }
    free(graph);
    free(parent);

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    getGraph(argv[1]);
    return 0;
}
