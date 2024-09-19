#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_NODES 100

int numberOfNodes, numberOfTasks;
int parent[MAX_NODES];

// Função para gerar a saída, mostrando quais tarefas foram atribuídas a quais trabalhadores
void generateOutput(int residualGraph[MAX_NODES][MAX_NODES], int originalGraph[MAX_NODES][MAX_NODES]) {
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        for (int j = numberOfTasks + 3; j < numberOfNodes + 3; j++) {
            // Se a aresta foi usada e está agora com fluxo zero
            if (originalGraph[i][j] == 1 && residualGraph[i][j] == 0) {
                printf("TaskID %d : Worker ID %d\n", i - 1, j - numberOfTasks - 2);
            }
        }
    }
}

void generateAssignmentDotFile(int residualGraph[MAX_NODES][MAX_NODES], int originalGraph[MAX_NODES][MAX_NODES], const char* filename) {
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


// Função para verificar se existe um caminho aumentado usando BFS (Busca em Largura)
bool hasPath(int graph[MAX_NODES][MAX_NODES], int source, int sink) {
    bool visited[MAX_NODES] = {false};  // Array para marcar nós visitados
    int queue[MAX_NODES], front = 0, rear = 0;
    
    // Inicializa o array de pais
    for (int i = 0; i <= numberOfNodes; i++) {
        parent[i] = -1;
    }
    
    // Adiciona o nó fonte na fila
    queue[rear++] = source;
    visited[source] = true;
    parent[source] = -1;

    // BFS para encontrar o caminho aumentado
    while (front < rear) {
        int u = queue[front++];  // Remove o nó da frente da fila
        for (int v = 1; v <= numberOfNodes; v++) {
            // Se o nó v não foi visitado e há capacidade na aresta u-v
            if (!visited[v] && graph[u][v] > 0) {
                queue[rear++] = v;  // Adiciona o nó v na fila
                parent[v] = u;  // Define o pai de v como u
                visited[v] = true;
                if (v == sink) return true;  // Se chegou ao nó destino, retorna true
            }
        }
    }

    return false;  // Se não encontrou caminho, retorna false
}

// Função principal do algoritmo Ford-Fulkerson
void fordFulkerson(int graph[MAX_NODES][MAX_NODES], int source, int sink) {
    int u, v;
    int residualGraph[MAX_NODES][MAX_NODES];
    
    // Inicializa o grafo residual com o grafo original
    for (int i = 1; i <= numberOfNodes; i++) {
        for (int j = 1; j <= numberOfNodes; j++) {
            residualGraph[i][j] = graph[i][j];
        }
    }

    // Enquanto houver caminho aumentado
    while (hasPath(residualGraph, source, sink)) {
        int pathFlow = INT_MAX;
        // Encontra o fluxo mínimo no caminho encontrado
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            pathFlow = pathFlow < residualGraph[u][v] ? pathFlow : residualGraph[u][v];
        }

        // Atualiza o grafo residual
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }
    }
    
    // Gera a saída exibindo o resultado
    generateOutput(residualGraph, graph);
    generateAssignmentDotFile(residualGraph, graph, "FordFulkersonGraph.dot");
}

// Função para ler o grafo a partir da entrada padrão
void getGraph() {
    int graph[MAX_NODES][MAX_NODES] = {0};  // Matriz de adjacência inicializada com zeros
    
    int numberOfWorkers;
    printf("Digite o numero de Funcionarios: ");
    scanf("%d", &numberOfWorkers);
    
    printf("Digite o numero de Tarefas: ");
    scanf("%d", &numberOfTasks);

    numberOfNodes = numberOfWorkers + numberOfTasks + 3;  // Calcula o número total de nós

    // Inicializa o grafo com zero
    for (int i = 0; i < numberOfNodes + 3; i++) {
        for (int j = 0; j < numberOfNodes + 3; j++) {
            graph[i][j] = 0;
        }
    }

    // Conecta o nó fonte a todos os nós de tarefas
    for (int i = 2; i <= numberOfTasks + 1; i++) {
        graph[1][i] = 1;
    }

    // Conecta todos os nós de trabalhadores ao nó sumidouro
    for (int i = numberOfTasks + 3; i < numberOfNodes + 3; i++) {
        graph[i][numberOfTasks + 2] = 1;
    }

    // Conecta nós de tarefas e trabalhadores com base na entrada do usuário
    for (int i = 0; i < numberOfTasks; i++) {
        printf("Funcionarios que concluem a Tarefa %d (separados por espaço, termine com 0): ", i + 1);
        int workerID;
        while (scanf("%d", &workerID) && workerID != 0) {
            if (workerID > 0 && workerID <= numberOfWorkers) {
                int columnNumber = workerID + numberOfTasks + 2;
                graph[i + 2][columnNumber] = 1;
            }
        }
        // Limpa o buffer de entrada
        while (getchar() != '\n');
    }   

    // Executa o algoritmo Ford-Fulkerson
    fordFulkerson(graph, 1, numberOfTasks + 2);

}


// Função principal que inicia o programa
int main() {
    getGraph();
    return 0;
}
