import shlex
import subprocess
import logging
import matplotlib.pyplot as plt
import csv
import numpy as np
import os

logging.basicConfig(level=logging.DEBUG, filename=os.path.join('log', 'run_exp_data.log'), filemode='w', format='%(process)d - [%(asctime)s] : %(levelname)s -> %(message)s')

BINARY_PROGRAM = "./ed"
BINARY_PROGRAM_OPTIMIZED = "./ford"
caminho_arquivo_teste = 'teste.txt'
TIMES_RUN = 6
execution_data = []  # Lista para armazenar dados de execução (n, tempo)

def run_code():
    logging.debug(f'Running the programs with the test file {TIMES_RUN} times')
    
    with open(caminho_arquivo_teste, 'r') as arquivo:
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
                process = subprocess.Popen([program], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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
                execution_data.append((primeira_linha, segunda_linha, time_average, program_name))  

def plot_execution_times():
    # Separar os dados por algoritmo
    inputs_data = sorted(set(entry[0] for entry in execution_data))  # Obter entradas únicas e ordenadas
    times_edmond_karp = []
    times_ford_fulkerson = []

    # Criar dicionários para mapear inputs aos tempos
    times_dict_original = {entry[0]: entry[2] for entry in execution_data if entry[3] == "Edmond-Karp"}
    times_dict_optimized = {entry[0]: entry[2] for entry in execution_data if entry[3] == "Ford-Fulkerson"}

    # Preencher listas com tempos para cada entrada
    for input_value in inputs_data:
        times_edmond_karp.append(times_dict_original.get(input_value, np.nan))
        times_ford_fulkerson.append(times_dict_optimized.get(input_value, np.nan))

    plot_general_comparison(inputs_data, times_edmond_karp, times_ford_fulkerson)
    plot_o_Edmond(inputs_data, times_edmond_karp)
    plot_o_Ford(inputs_data, times_ford_fulkerson)

def plot_general_comparison(inputs_data, times_edmond_karp, times_ford_fulkerson):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_edmond_karp, marker='o', label='Média tempo execução - Edmond-karp')
    plt.plot(inputs_data, times_ford_fulkerson, marker='o', label='Média tempo execução - Ford-Fulkerson')
    plt.xlabel('N')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Comparação de Tempo de Execução entre Algoritmo Original e Otimizado')
    plt.legend()
    plt.xlim(left=0, right=max([int(v) for v in inputs_data]))
    plt.grid(True)
    
    # Salva o gráfico em um arquivo PNG
    plt.savefig(os.path.join('plot', 'execution_time_comparison_plot.png'))
    print("O gráfico foi salvo como 'execution_time_comparison_plot.png'.")

def plot_o_Edmond(inputs_data, times_edmond_karp):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_edmond_karp, marker='o', label='Média tempo execução')
    plt.xlabel('N')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Tempo de Execução do Edmond-Karp')
    plt.legend()
    plt.xlim(left=0, right=max([int(v) for v in inputs_data]) + 5)
    plt.grid(True)
    
    # Salva o gráfico em um arquivo PNG
    plt.savefig(os.path.join('plot', 'execution_time_Edmond.png'))
    print("O gráfico foi salvo como 'execution_time_Edmond.png'.")

def plot_o_Ford(inputs_data, times_ford_fulkerson):
    plt.figure(figsize=(12, 6))
    plt.plot(inputs_data, times_ford_fulkerson, marker='o', label='Média tempo execução')
    plt.xlabel('N')
    plt.ylabel('Tempo de execução (segundos)')
    plt.title('Tempo de Execução do Ford Fulkerson')
    plt.legend()
    plt.xlim(left=0, right=max([int(v) for v in inputs_data]) + 5)
    plt.grid(True)
    
    # Salva o gráfico em um arquivo PNG
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
