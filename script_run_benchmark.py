import shlex
import subprocess
import logging
import matplotlib.pyplot as plt
import csv
import numpy as np
import os

# Inicialização de logging e configuração
logging.basicConfig(level=logging.DEBUG, filename=os.path.join('log', 'run_exp_data.log'), filemode='w', format='%(process)d - [%(asctime)s] : %(levelname)s -> %(message)s')

BINARY_PROGRAM = "./ed"
BINARY_PROGRAM_OPTIMIZED = "./FordFulkerson"
inputs = ["50-48", "100-100","200-196","300-302", "400-398","500-498","600-602","700-702","800-796","900-902"]
TIMES_RUN = 6
execution_data = [] 

# Dicionário para armazenar resultados de vértices e arestas
grafo_info = {}

# Função para contar vértices e arestas
def contar_grafo(arquivo):
    with open(arquivo, 'r') as f:
        # Lê as duas primeiras linhas e calcula o número de vértices
        linha1 = int(f.readline().strip())
        linha2 = int(f.readline().strip())
        num_vertices = linha1 + linha2 + 2
        
        arestas = 0
        
        for linha in f:
            # Cada linha até encontrar '0' representa uma aresta
            vertices_linha = list(map(int, linha.strip().split()))
            # Remove o último elemento que é 0
            if vertices_linha[-1] == 0:
                vertices_linha.pop()
            arestas += len(vertices_linha)  # Contamos os vértices que formam a aresta

        # A quantidade de arestas é ajustada com a soma das duas primeiras linhas
        arestas += linha1 + linha2
        
    return num_vertices, arestas

def run_code():
    print("Script em execução...., aguarde")
    logging.debug(f'Running the programs with the test files {TIMES_RUN} times')
    
    # Loop sobre cada arquivo de entrada na lista 'inputs'
    for caminho_arquivo_teste in inputs:
        logging.debug(f'Processing test file: {caminho_arquivo_teste}')
        file_path = f"./testes/{caminho_arquivo_teste}.txt"
        
        # Contagem de vértices e arestas
        num_vertices, num_arestas = contar_grafo(file_path)
        
        # Inicializa o dicionário com o nome do arquivo de teste
        grafo_info[caminho_arquivo_teste] = {"Vértices": num_vertices, "Arestas": num_arestas}
        
        logging.debug(f'Vértices: {num_vertices}, Arestas: {num_arestas}')
        
        with open(file_path, 'r') as arquivo:
            linhas = arquivo.readlines()
        
        i = 0
        num_linhas = len(linhas)

        while i < num_linhas:
            # Ler o primeiro valor (número de entradas)
            primeira_linha = linhas[i].strip()
            i += 1
            
            # Ler o segundo valor (quantidade de leituras)
            segunda_linha = linhas[i].strip()
            quantidade_leituras = int(segunda_linha)
            i += 1

            # Ler as próximas linhas conforme a quantidade
            entradas = [linhas[j].strip() for j in range(i, i + quantidade_leituras)]
            i += quantidade_leituras
            
            # Preparar as entradas para o programa C
            entradas_str = '\n'.join(entradas)
            entrada_total = f"{primeira_linha}\n{segunda_linha}\n{entradas_str}\n"
            
            logging.debug(f"======================================TESTE: {primeira_linha}, Leituras: {segunda_linha}==========================================")
            for program_name, program in [("Edmond-Karp", BINARY_PROGRAM), ("Ford-Fulkerson", BINARY_PROGRAM_OPTIMIZED)]:
                logging.debug(f"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX<<< {program_name} >>>XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
                
                times = []

                for count_time in range(TIMES_RUN):
                    logging.debug(f"------------------------------------{count_time + 1}° EXECUÇÃO-----------------------------------------")
                    cmd = shlex.split(f"{program} {file_path}")
                    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    stdout, stderr = process.communicate(input=entrada_total.encode())

                    # Verifica se houve erro durante a execução
                    if process.returncode != 0 or stderr:
                        logging.error(f"Error during run {count_time + 1} with test {primeira_linha} ({program_name}): {stderr}")
                    else:
                        try:
                            # Extrair o tempo de execução da saída
                            run_time = float(stdout.strip())
                            logging.debug(f"Execution time for test {primeira_linha} (run {count_time + 1}): {run_time} seconds")
                            logging.debug(f"------------------------------------------------------------------------------------\n")
                            times.append(run_time)
                        except ValueError:
                            logging.error(f"Invalid output for execution time: {stdout.strip()}")

                if times:
                    time_average = sum(times) / len(times)
                    execution_data.append((int(primeira_linha), segunda_linha, time_average, program_name))

def plot_execution_times():
    print(grafo_info)
    # Separar os dados por algoritmo
    inputs_data = sorted(set(entry[0] for entry in execution_data))  # Obter entradas únicas e ordenadas
    times_edmond_karp = []
    times_ford_fulkerson = []
    ve_squared_edmond_karp = []  # Para armazenar os valores de VE^2

    # Criar dicionários para mapear inputs aos tempos
    times_dict_original = {entry[0]: entry[2] for entry in execution_data if entry[3] == "Edmond-Karp"}
    times_dict_optimized = {entry[0]: entry[2] for entry in execution_data if entry[3] == "Ford-Fulkerson"}

    # Preencher listas com tempos e calcular VE^2 para cada entrada
    for input_value in inputs_data:
        times_edmond_karp.append(times_dict_original.get(input_value, np.nan))
        times_ford_fulkerson.append(times_dict_optimized.get(input_value, np.nan))

    plot_comparison_ford_edmond(inputs, times_edmond_karp, times_ford_fulkerson)
    plot_o_Edmond(inputs, times_edmond_karp)
    plot_o_Ford(inputs, times_ford_fulkerson)

def plot_comparison_ford_edmond(inputs_data, times_edmond_karp, times_ford_fulkerson):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_edmond_karp, marker='o', label='Média tempo execução - Edmond-karp')
    plt.plot(inputs_data, times_ford_fulkerson, marker='o', label='Média tempo execução - Ford-Fulkerson')
    plt.xlabel('Funcionários-Tarefas')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Comparação de Tempo de Execução entre Edmond-Karp e Ford-Fulkerson')
    plt.legend()
    plt.grid(True)
    
    plt.savefig(os.path.join('plot', 'execution_time_comparisonEF_plot.png'))
    print("O gráfico foi salvo como 'execution_time_comparisonEF_plot.png'.")

def plot_o_Edmond(inputs_data, times_edmond_karp):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_edmond_karp, marker='o', label='Média tempo execução')
    plt.xlabel('Funcionários-Tarefas')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Tempo de Execução do Edmond-Karp')
    plt.legend()
    plt.grid(True)
    
    plt.savefig(os.path.join('plot', 'execution_time_Edmond.png'))
    print("O gráfico foi salvo como 'execution_time_Edmond.png'.")

def plot_o_Ford(inputs_data, times_ford_fulkerson):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_ford_fulkerson, marker='o', label='Média tempo execução')
    plt.xlabel('Funcionários-Tarefas')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Tempo de Execução do Ford Fulkerson')
    plt.legend()
    plt.grid(True)
    
    plt.savefig(os.path.join('plot', 'execution_time_Ford.png'))
    print("O gráfico foi salvo como 'execution_time_Ford.png'.")

def save_to_csv():
    with open('execution_times.csv', mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['N° Funcionários', 'N° Tarefas', 'Tempo Médio (segundos)', 'Algoritmo'])
        for entry in execution_data:
            writer.writerow(entry)
    print("Os tempos médios foram salvos em 'execution_times.csv'.")

def main():
    logging.debug('Experiment script executed')
    run_code()
    
    # Executa a plotagem sem logar as informações do gráfico
    logging.disable(logging.CRITICAL)
    plot_execution_times()
    save_to_csv()
    logging.disable(logging.NOTSET)

if __name__ == "__main__":
    main()
