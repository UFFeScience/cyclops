import matplotlib.pyplot as plt
import pandas as pd
from tabulate import tabulate

def imprime_tabela(instance):
    df = pd.read_csv(f'{path_to_project}/{output_folder}/{selected_option}/{output_file_name}', header='infer')
    newdf = df[(1==1)
               & (df['num_buckets'] == 64)
               & (df['alpha_restricted_candidate_list'] == 0.3)
               & (df.alpha_time == 0.05) 
               & (df.alpha_cost == 0.05) 
               & (df.alpha_security == 0.9) 
               & (df['instance'] == instance)
               & (df['scenario'] == 4)
              ]
    
    # Selecionar apenas as colunas desejadas
    colunas_desejadas = [
        'object_function', 
        'makespan', 
        'cost', 
        'security', 
        'time',
        'num_iterations', 
        'best_solution_iteration', 
        'best_solution_time', 
        # 'lsn_time_1', 
        # 'lsn_noi_1',
        # 'lsn_time_2', 
        # 'lsn_noi_2', 
        # 'lsn_time_3', 
        # 'lsn_noi_3'
    ]
    
    # Limpar valores negativos
    for coluna in colunas_desejadas:
        newdf.loc[newdf[coluna] < 0, coluna] = None
    
    # Agrupar e calcular a média
    media_por_grupo = newdf.groupby([
        'algorithm', 
        'instance', 
        'scenario', 
        'alpha_time', 
        'alpha_security',
        'alpha_restricted_candidate_list', 
        'num_buckets'])[colunas_desejadas].mean()

    # Sobrescrever os valores da primeira coluna com apenas o nome do algoritmo
    # media_por_grupo.index = media_por_grupo.index.set_levels(media_por_grupo.index.levels[0].str.split('_').str[0], level=0)
    # media_por_grupo.index = media_por_grupo.index.set_levels(media_por_grupo.index.levels[0].values, level=0)
    # print(media_por_grupo.index.get_levels())
    # print(media_por_grupo.index.set_levels(media_por_grupo.index.levels[0].str.split('_').str[0], level=0))


    # Se o seu índice atual é um MultiIndex, você pode modificá-lo da seguinte maneira
    media_por_grupo.index = media_por_grupo.index.droplevel(level=[1, 2, 6, 7])

    # print(dir(media_por_grupo.index.levels[0].values))
    # print(media_por_grupo.index.levels[0].values)
    
    # print(media_por_grupo.columns)
    # Após o cálculo da média, remover as colunas 'instance' e 'scenario'
    # media_por_grupo = media_por_grupo.reset_index(drop=True)  # Resetar o índice para poder remover colunas por nome
    # print(media_por_grupo.columns)
    # media_por_grupo.drop(columns=['instance', 'scenario'], inplace=True)
    
    # Imprimir a tabela usando tabulate
    # Apelidos para os cabeçalhos das colunas
    apelidos = {
        "num_iterations": "n_iter", 
        "object_function": "O.F.", 
        "makespan": "mspn",
        "cost": "cost",
        "security": "security",
        "time": "time",
        "best_solution_iteration": "bsi",
        "best_solution_time": "bst",
        "lsn_time_1": "lsnt_1",
        "lsn_noi_1": "lsni_1",
        "lsn_time_2": "lsnt_2",
        "lsn_noi_2": "lsni_2",
        "lsn_time_3": "lsnt_3",
        "lsn_noi_3": "lsni_3",
    }

    # Função para aplicar estilos às células
    def estilo_celula(valor, menor_valor):
        if valor == menor_valor:
            return f"\033[1m{valor}\033[0m"  # Adiciona negrito ao valor
        return valor

    # Calcula o menor valor para cada coluna
    menores_valores = media_por_grupo.min()

    # Aplica o estilo às células das colunas desejadas
    for coluna in colunas_desejadas:
        if coluna in media_por_grupo.columns:
            media_por_grupo[coluna] = media_por_grupo[coluna].apply(
                lambda valor: estilo_celula(valor, menores_valores[coluna])
            )
    
    # Renomear as colunas
    
    media_por_grupo = media_por_grupo.rename(columns=apelidos)

    df_sorted = media_por_grupo.sort_values(by='algorithm', ascending=False)
    df_sorted.index.name = instance + ' - Cenário 4\n64 buckets - RCL = 0.3'
    print(tabulate(df_sorted, headers='keys', tablefmt='psql'))