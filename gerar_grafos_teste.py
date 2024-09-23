import random

# Número de funcionários e tarefas
num_funcionarios = [50,100,200,300,400,500,600,700,800,900]  # Diferentes casos de teste
densidade_min = 0.2  # Densidade mínima (20% de conexões)
densidade_max = 0.9  # Densidade máxima (90% de conexões)

# Nome do arquivo de saída
nome_arquivo = "funcionarios_tarefas.txt"

# Função para gerar tarefas com base na densidade
def gerar_tarefas(num_func, num_tarefas, densidade):
    tarefas = []
    for _ in range(num_tarefas):
        # Determinar quais funcionários estarão conectados a esta tarefa
        funcionarios_conectados = [
            str(funcionario) for funcionario in range(1, num_func + 1)
            if random.random() <= densidade
        ]
        # Adicionar "0" no final da lista de funcionários conectados
        tarefas.append(" ".join(funcionarios_conectados) + " 0")
    return tarefas

# Abrir o arquivo em modo de escrita
with open(nome_arquivo, "w") as file:
    for num_func in num_funcionarios:
        num_tarefas = random.choice([num_func, num_func - 2, num_func - 4, num_func + 2])  # Escolher o número de tarefas
        densidade = random.uniform(densidade_min, densidade_max)  # Escolher uma densidade aleatória

        # Gerar as tarefas com base na densidade
        tarefas = gerar_tarefas(num_func, num_tarefas, densidade)
        
        # Escrever o número de funcionários e tarefas no arquivo
        file.write(f"{num_func}\n{num_tarefas}\n")

        # Escrever as tarefas no arquivo
        for tarefa in tarefas:
            file.write(tarefa + "\n")

print(f"Arquivo '{nome_arquivo}' gerado com sucesso.")
