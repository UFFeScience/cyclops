import csv
import os

def listar_pastas(diretorio):
    # Verificar se o diretório existe
    if not os.path.isdir(diretorio):
        print(f'O diretório "{diretorio}" não existe.')
        return []
    # Lista para armazenar os nomes das pastas
    pastas = []
    # Iterar sobre os arquivos e pastas no diretório
    for nome in os.listdir(diretorio):
        caminho = os.path.join(diretorio, nome)
        if os.path.isdir(caminho):
            pastas.append(nome)
    pastas.sort(reverse=True)
    return pastas

def list_alphas_ordered(file_val):
    # Lista para armazenar as variações únicas
    variacoes = set()
    # Lendo o arquivo CSV
    with open(file_val, 'r') as file:
        reader = csv.reader(file)
        # Lendo o cabeçalho para obter os nomes dos campos
        header = next(reader)
        # Obter os índices dos campos desejados
        indice_campo4 = header.index('alpha_time')
        indice_campo5 = header.index('alpha_cost')
        indice_campo6 = header.index('alpha_security')
        indice_campo7 = header.index('alpha_restricted_candidate_list')
        # Iterar pelas linhas do arquivo
        for row in reader:
            # Extrair os valores dos 4 primeiros campos
            campo1, campo2, campo3, campo4, *_ = row[3:7]
            # Adicionar a variação única à lista
            variacoes.add((campo1, campo2, campo3, campo4))
    # Ordenar as variações únicas
    return sorted(variacoes, reverse=True)
