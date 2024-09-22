# L_Prado_L_Anderson_FinalProject_AA_RR_2024

## Network Flow: Alocação de Tarefas usando Grafo Bipartido

## Descrição

Neste projeto, você receberá dois conjuntos: um conjunto de funcionários e um conjunto de tarefas. Para cada tarefa, é fornecida uma lista de funcionários que podem concluir essa tarefa. O objetivo é alocar o trabalho de forma que todas as tarefas sejam concluídas com o máximo de eficiência possível.

### Modelagem do Problema

O problema é modelado como um grafo bipartido onde:
- Um conjunto de nós representa as tarefas.
- O outro conjunto de nós representa os funcionários.
- As arestas entre os nós de tarefas e funcionários indicam a possibilidade de um funcionário concluir uma tarefa.

### Algoritmo Implementado

1. **Ford-Fulkerson**
   - Um algoritmo clássico para encontrar o fluxo máximo em um grafo de fluxo.
   

### Funcionalidades

- **Visualização do Caminho Aumentado**: A interface do usuário permite a visualização do caminho aumentado.
- **Leitura e Impressão**: O sistema lê as entradas diretamente do usuário e exibe os resultados diretamente no console.

### Uso

1. **Compilação**:
   ```sh
   gcc FordFulkerson.c -o FordFulkerson
   ```
   ```sh
   gcc EdmondKarp.c -o EdmondKarp
   ```
2. **Execução**:
   ```sh
    ./FordFulkerson
    ./EdmondKarp
    ```
3. **Instruções**:
    - Digite o número de funcionários.
    - Digite o número de tarefas.
    - Para cada tarefa, insira a lista de funcionários que podem concluir a tarefa (separados por espaço, termine com 0).
4. **Visualização do grafo com GraphVIZ**:
    - Instale o pacote da biblioteca
    ```sh
    sudo apt-get install graphviz
    ```

   - Gere o arquivo de imagem para visualização do grafo

    ```sh
    dot -Tpng <nome-arquivo>.dot -o <nome-arquivo>.png
    ```

## Referências
- https://github.com/nirajdewani/Task-Allocator-using-Maximum-Bipartite-Matching

### To do list
- [ ] Artigo
- [ ] Cálculo função de custo
- [ ] Benchmark
