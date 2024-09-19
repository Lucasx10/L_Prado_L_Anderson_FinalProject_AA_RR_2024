#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_NODES 200

int numberOfNodes, numberOfEmployees, numberOfTasks;
int parent[MAX_NODES];

bool hasPath(int graph[MAX_NODES][MAX_NODES], int source, int sink) {
    bool visited[MAX_NODES] = {false};
    int queue[MAX_NODES], front = 0, rear = 0;

    for (int i = 0; i <= numberOfNodes; i++) {
        parent[i] = -1;
    }

    queue[rear++] = source;
    visited[source] = true;

    while (front < rear) {
        int u = queue[front++];
        for (int v = 1; v <= numberOfNodes; v++) {
            if (!visited[v] && graph[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = true;
                if (v == sink) return true;
            }
        }
    }
    return false;
}

void generateOutput(int residualGraph[MAX_NODES][MAX_NODES], int originalGraph[MAX_NODES][MAX_NODES]) {
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        for (int j = numberOfTasks + 3; j <= numberOfNodes; j++) {
            // Se a aresta foi usada e está agora com fluxo zero
            if (originalGraph[i][j] == 1 && residualGraph[i][j] == 0) {
                printf("TaskID %d : Worker ID %d\n", i - 1, j - numberOfTasks - 2);
            }
        }
    }
}

void edmondsKarp(int graph[MAX_NODES][MAX_NODES], int source, int sink) {
    int residualGraph[MAX_NODES][MAX_NODES];

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

    generateOutput(residualGraph, graph);
}

void getGraph() {
    int graph[MAX_NODES][MAX_NODES] = {0};

    printf("Digite o numero de Funcionarios: ");
    scanf("%d", &numberOfEmployees);

    printf("Digite o numero de Tarefas: ");
    scanf("%d", &numberOfTasks);

    numberOfNodes = numberOfEmployees + numberOfTasks + 3;

    for (int i = 0; i <= numberOfNodes; i++) {
        for (int j = 0; j <= numberOfNodes; j++) {
            graph[i][j] = 0;
        }
    }

    for (int i = 2; i <= numberOfTasks + 1; i++) {
        graph[1][i] = 1;
    }

    for (int i = numberOfTasks + 3; i <= numberOfNodes; i++) {
        graph[i][numberOfTasks + 2] = 1;
    }

    for (int i = 0; i < numberOfTasks; i++) {
        printf("Funcionarios que concluem a Tarefa %d (separados por espaço, termine com 0): ", i + 1);
        int workerID;
        while (scanf("%d", &workerID) && workerID != 0) {
            if (workerID > 0 && workerID <= numberOfEmployees) {
                int columnNumber = workerID + numberOfTasks + 2;
                graph[i + 2][columnNumber] = 1;
            }
        }
        while (getchar() != '\n');
    }   

    edmondsKarp(graph, 1, numberOfTasks + 2);
}

int main() {
    getGraph();
    return 0;
}
