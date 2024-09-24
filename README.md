# L_Prado_L_Anderson_FinalProject_AA_RR_2024

## Network Flow: Alocação de Tarefas usando Grafo Bipartido

## Descrição

Neste projeto, você receberá dois conjuntos: um conjunto de funcionários e um conjunto de tarefas. Para cada tarefa, é fornecida uma lista de funcionários que podem concluir essa tarefa. O objetivo é alocar o trabalho de forma que todas as tarefas sejam concluídas com o máximo de eficiência possível.

### Modelagem do Problema

O problema é modelado como um grafo bipartido onde:
- Um conjunto de nós representa as tarefas.
- O outro conjunto de nós representa os funcionários.
- As arestas entre os nós de tarefas e funcionários indicam a possibilidade de um funcionário concluir uma tarefa.

### Algoritmos Implementados

1. **Ford-Fulkerson**
2. **Edmond-Karp**

### Uso

1. **Compilação**:
   ```sh
   gcc FordFulkerson.c -o FordFulkerson
   ```
   ```sh
   gcc EdmondKarp.c -o EdmondKarp
   ```
2. **Rodar os casos de teste**:
- Após a criação dos arquivos binários basta rodar o comando abaixo para executar os testes
   ```sh
    python3 script_run_benchmark.py
    ```

3. **Execução individual(opcional)**:
   ```sh
    ./FordFulkerson ./testes/<arquivo de teste>.txt
    ./EdmondKarp ./testes/<arquivo de teste>.txt
    ```
4. **Visualização do grafo com GraphVIZ**:
    - Instale o pacote da biblioteca se não tiver instalado no ambiente
    ```sh
    sudo apt-get install graphviz
    ```
    - A fim de não prejudicar nos testes as linhas dos outputs e da geração do arquivo .dot foram comentadas, para obter a visualização final do grafo basta descomentar as linhas 110 e 112 do algoritmo EdmondKarp e as linhas 97 e 99 do algoritmo FordFulkerson
    - Compile os código .c novamente e execute o algoritmo desejado

   - Gere o arquivo de imagem para visualização do grafo

    ```sh
    dot -Tpng <nome-arquivo>.dot -o <nome-arquivo>.png
    ```

## Referências
- https://github.com/nirajdewani/Task-Allocator-using-Maximum-Bipartite-Matching

### Resposta ao questionamento feito em sala
- Por que não utilizar um algoritmo randômico em vez dos algoritmos apresentados neste trabalho?
Algoritmos randômicos não são adequados para a resolução deste problema, pois, na modelagem, cada tarefa recebe uma lista de funcionários aptos a realizá-la. Sendo assim, a solução deve respeitar essa restrição. Uma abordagem randômica seria mais apropriada apenas se essa restrição não existisse, como no caso de se precisar atribuir x tarefas a x funcionários sem qualquer limitação quanto à execução de uma tarefa específica por um funcionário. Por esse motivo, os algoritmos Ford-Fulkerson e Edmonds-Karp são mais adequados para este problema.