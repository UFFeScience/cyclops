# Índice

1. [Introdução](#introdução)
2. [Arquitetura](#arquitetura)
3. [Escalonamento #1](#escalonamento-1)
    1. [Ativações por Máquina Virtual](#escalonamento-1)
    2. [Distribuições dos Arquivos por Dispositivos de Armazenamento](#distribuições-dos-arquivos-por-dispositivos-de-armazenamento-1)
    3. [Ordem de Execução](#ordem-de-execução)
4. [Escalonamento #2](#escalonamento-2)
    1. [Ativações por Máquina Virtual](#escalonamento-2)
    2. [Distribuições dos Arquivos por Dispositivos de Armazenamento](#distribuições-dos-arquivos-por-dispositivos-de-armazenamento-2)
    3. [Ordem de Execução](#ordem-de-execução-2)
5. [Escalonamento #3](#escalonamento-3)
    1. [Ativações por Máquina Virtual](#escalonamento-3)
    2. [Distribuições dos Arquivos por Dispositivos de Armazenamento](#distribuições-dos-arquivos-por-dispositivos-de-armazenamento-3)
    3. [Ordem de Execução](#ordem-de-execução-3)

## Introdução

Fizemos o escalonamento das ativações utilizando a metaheurística GRASP sobre o *workflow* [**wf-montage-50d**](https://github.com/ovvesley/akoflow/blob/d1b1b18866a613bd840e77e6d9128b0c9daba453/pkg/client/resource/montage-wf/wf-montage-050d/wf-montage-050d.yaml). Abaixo temos as definições das máquinas virtuais e *buckets* utilizados.

Algumas observações para o restante deste documento: 

1. Usamos para identificar as ativações os seus números, portanto, onde estiver `ID0000001`, isso significará a ativação `mprojectid0000001` no *workflow* inicial definido pelo Wesley.
2. A definição de arquitetura abaixo apresenta apenas um único *bucket*, porém, para os escalonamentos foram utilizados de 8 a 32.

## Arquitetura

| **Tipo** | **Disco (GB)** | **CSI** | **Rede (Gbps)** | **Custo**      | **Criptografia** |
|----------|----------------|---------|-----------------|----------------|------------------|
| MV1      |             12 |   1,00  |               4 | U$ 0,02 / hora | Ligado           |
| MV2      |             12 |   0,75  |               9 | U$ 0,09 / hora | Ligado           |
| MV3      |             12 |   0,50  |              10 | U$ 0,16 / hora | Desligado        |
| MV4      |             12 |   0,25  |              10 | U$ 0,33 / hora | Desligado        |
| Bucket   |          51200 |    -    |              25 | U$ 0,23 / GB   | -                |

## Escalonamento #1

Neste escalonamento, estamos considerando a execução numa nuvem com as 4 máquinas virtuais definidas em [Arquitetura](#arquitetura) com 8 *buckets*.

### Ativações por Máquina Virtual (#1)

Nesta subseção, definimos quais ativações serão executadas em quais máquinas virtuais. A seta (-->) indica a ordem em que as ativações precisam ser executadas em cada máquina virtual.

**MV1**: \
ID0000013 --> ID0000017 --> ID0000018 --> ID0000019 --> ID0000021 --> ID0000023 --> ID0000024 --> ID0000045 --> ID0000050 --> ID0000053 --> ID0000054 \
**MV2**: \
ID0000002 --> ID0000006 --> ID0000007 --> ID0000008 --> ID0000009 --> ID0000016 --> ID0000020 --> ID0000030 --> ID0000031 --> ID0000033 --> ID0000037 --> ID0000049 --> ID0000056 --> ID0000057 \
**MV3**: \
ID0000014 --> ID0000015 --> ID0000022 --> ID0000025 --> ID0000026 --> ID0000028 --> ID0000029 --> ID0000034 --> ID0000038 --> ID0000039 --> ID0000040 --> ID0000046 --> ID0000051 --> ID0000055 --> ID0000058 \
**MV4**: \
ID0000001 --> ID0000003 --> ID0000004 --> ID0000005 --> ID0000010 --> ID0000011 --> ID0000012 --> ID0000027 --> ID0000032 --> ID0000035 --> ID0000036 --> ID0000041 --> ID0000042 --> ID0000043 --> ID0000044 --> ID0000047 --> ID0000048 --> ID0000052

### Distribuições dos Arquivos por Dispositivos de Armazenamento (#1)

Nesta subseção, a tabela abaixo apresenta onde cada um dos arquivos estáticos, presentes desde o início da execução, e cada um dos arquivos dinâmicos, gerados durante a execução, deverão estar no final da execução do *workflow*.

|  **Dispersão dos Dados** | |
|:-:|:-|
|**MV1**|
|  |region.hdr
|  |region-oversized.hdr
|  |poss2ukstu_blue_001_001.fits
|  |poss2ukstu_blue_001_002.fits
|  |poss2ukstu_blue_002_001.fits
|  |poss2ukstu_blue_002_002.fits
|  |1-stat.tbl
|  |1-images.tbl
|  |1-projected.tbl
|  |1-corrected.tbl
|  |poss2ukstu_red_001_001.fits
|  |poss2ukstu_red_001_002.fits
|  |poss2ukstu_red_002_001.fits
|  |poss2ukstu_red_002_002.fits
|  |2-stat.tbl
|  |2-images.tbl
|  |2-projected.tbl
|  |2-corrected.tbl
|  |poss2ukstu_ir_001_001.fits
|  |poss2ukstu_ir_001_002.fits
|  |poss2ukstu_ir_002_001.fits
|  |poss2ukstu_ir_002_002.fits
|  |3-stat.tbl
|  |3-images.tbl
|  |3-projected.tbl
|  |3-corrected.tbl
|  |pposs2ukstu_blue_001_001.fits
|  |pposs2ukstu_blue_001_001_area.fits
|  |pposs2ukstu_red_001_001.fits
|  |pposs2ukstu_red_001_001_area.fits
|  |cposs2ukstu_red_001_002.fits
|  |cposs2ukstu_red_001_002_area.fits
|  |cposs2ukstu_red_002_002.fits
|  |cposs2ukstu_red_002_002_area.fits
|  |pposs2ukstu_ir_001_001.fits
|  |pposs2ukstu_ir_001_001_area.fits
|  |pposs2ukstu_ir_002_001.fits
|  |pposs2ukstu_ir_002_001_area.fits
|  |pposs2ukstu_ir_002_002.fits
|  |pposs2ukstu_ir_002_002_area.fits
|  |3-fits.tbl
|  |cposs2ukstu_ir_001_002.fits
|  |cposs2ukstu_ir_001_002_area.fits
|**MV2**|                                    |
|  |pposs2ukstu_blue_002_002.fits
|  |pposs2ukstu_blue_002_002_area.fits
|  |2-fit.000001.000002.txt
|  |2-diff.000001.000002.fits
|  |2-diff.000001.000002_area.fits
|  |2-fit.000001.000003.txt
|  |2-diff.000001.000003.fits
|  |2-diff.000001.000003_area.fits
|  |2-corrections.tbl
|  |3-diff.000001.000003_area.fits
|  |3-diff.000001.000004.fits
|  |3-fit.000002.000003.txt
|  |3-diff.000002.000003.fits
|  |3-diff.000002.000003_area.fits
|  |3-fit.000002.000004.txt
|  |3-diff.000002.000004.fits
|  |3-diff.000002.000004_area.fits
|  |3-diff.000003.000004_area.fits
|  |3-corrections.tbl
|  |3-updated-corrected.tbl
|  |3-mosaic.png
|**MV3**|                                    |
|  |pposs2ukstu_blue_001_002.fits
|  |pposs2ukstu_blue_001_002_area.fits
|  |pposs2ukstu_red_002_001.fits
|  |pposs2ukstu_red_002_001_area.fits
|  |2-fit.000002.000004.txt
|  |2-diff.000002.000004.fits
|  |2-diff.000002.000004_area.fits
|  |2-mosaic.png
|  |pposs2ukstu_ir_001_002.fits
|  |pposs2ukstu_ir_001_002_area.fits
|  |3-fit.000001.000003.txt
|  |3-diff.000001.000003.fits
|  |3-fit.000001.000004.txt
|  |3-diff.000001.000004_area.fits
|  |3-fit.000003.000004.txt
|  |3-diff.000003.000004.fits
|  |cposs2ukstu_ir_001_001.fits
|  |cposs2ukstu_ir_001_001_area.fits
|  |cposs2ukstu_ir_002_001.fits
|  |cposs2ukstu_ir_002_001_area.fits
|  |cposs2ukstu_ir_002_002.fits
|  |cposs2ukstu_ir_002_002_area.fits
|**MV4**|
|  |1-fit.000001.000002.txt
|  |1-diff.000001.000002.fits
|  |1-diff.000001.000002_area.fits
|  |2-fit.000002.000003.txt
|  |2-diff.000002.000003.fits
|  |2-diff.000002.000003_area.fits
|  |2-updated-corrected.tbl
|  |3-fit.000001.000002.txt
|  |3-diff.000001.000002.fits
|  |3-diff.000001.000002_area.fits
|***Bucket1***|
|  |1-diff.000001.000004.fits
|  |1-diff.000002.000003.fits
|  |cposs2ukstu_blue_002_001_area.fits
|  |pposs2ukstu_red_001_002.fits
|  |pposs2ukstu_red_002_002.fits
|  |2-mosaic_area.fits
|***Bucket2***|  
|  |1-fit.000001.000003.txt
|  |1-corrections.tbl
|  |1-mosaic.fits
|  |1-mosaic_area.fits
|  |2-fit.000001.000004.txt
|  |2-mosaic.fits
|***Bucket3***|
|  |pposs2ukstu_blue_002_001.fits
|  |pposs2ukstu_blue_002_001_area.fits
|  |1-fits.tbl
|  |cposs2ukstu_blue_001_001.fits
|  |pposs2ukstu_red_002_002_area.fits
|  |cposs2ukstu_red_002_001_area.fits
|  |3-mosaic_area.fits
|***Bucket4***|  
|  |1-fit.000003.000004.txt
|  |cposs2ukstu_blue_001_001_area.fits
|  |cposs2ukstu_blue_001_002.fits
|  |2-diff.000001.000004_area.fits
|  |cposs2ukstu_red_001_001_area.fits
|***Bucket5***|  
|  |1-fit.000001.000004.txt
|  |1-diff.000002.000004.fits
|  |cposs2ukstu_blue_002_001.fits
|  |2-diff.000001.000004.fits
|  |2-diff.000003.000004_area.fits
|***Bucket6***|
|  |1-diff.000001.000003.fits
|  |1-diff.000003.000004_area.fits
|  |cposs2ukstu_blue_001_002_area.fits
|  |1-mosaic.png
|  |2-fits.tbl
|***Bucket7***|
|  |1-diff.000001.000003_area.fits
|  |1-fit.000002.000003.txt
|  |1-fit.000002.000004.txt
|  |cposs2ukstu_blue_002_002.fits
|  |cposs2ukstu_blue_002_002_area.fits
|  |2-fit.000003.000004.txt
|  |2-diff.000003.000004.fits
|  |cposs2ukstu_red_001_001.fits
|  |mosaic-color.png
|***Bucket8***|
|  |1-diff.000001.000004_area.fits
|  |1-diff.000002.000003_area.fits
|  |1-diff.000002.000004_area.fits
|  |1-diff.000003.000004.fits
|  |1-updated-corrected.tbl
|  |pposs2ukstu_red_001_002_area.fits
|  |cposs2ukstu_red_002_001.fits
|  |3-mosaic.fits

### Ordem de Execução #1

A ordem para executar o *workflow* é a seguinte:

ID0000021 --> ID0000023 --> ID0000039 --> ID0000042 --> ID0000002 --> ID0000040 --> ID0000003 --> ID0000041 --> ID0000022 --> ID0000001 --> ID0000004 --> ID0000020 --> ID0000045 --> ID0000005 --> ID0000047 --> ID0000025 --> ID0000043 --> ID0000046 --> ID0000048 --> ID0000028 --> ID0000044 --> ID0000026 --> ID0000024 --> ID0000007 --> ID0000029 --> ID0000010 --> ID0000008 --> ID0000009 --> ID0000006 --> ID0000027 --> ID0000049 --> ID0000030 --> ID0000011 --> ID0000050 --> ID0000012 --> ID0000031 --> ID0000013 --> ID0000052 --> ID0000014 --> ID0000033 --> ID0000032 --> ID0000015 --> ID0000016 --> ID0000035 --> ID0000034 --> ID0000054 --> ID0000053 --> ID0000051 --> ID0000055 --> ID0000036 --> ID0000017 --> ID0000037 --> ID0000056 --> ID0000018 --> ID0000019 --> ID0000057 --> ID0000038 --> ID0000058

## Escalonamento #2

Neste escalonamento, estamos considerando a execução numa nuvem com as 4 máquinas virtuais definidas em [Arquitetura](#arquitetura) com 16 *buckets*.

### Ativações por Máquina Virtual (#2)

**MV1**: \
ID0000001 --> ID0000006 --> ID0000024 --> ID0000025 --> ID0000027 --> ID0000028 --> ID0000029 --> ID0000032 --> ID0000038 --> ID0000043 --> ID0000044 --> ID0000045 --> ID0000048 --> ID0000049 --> ID0000050 --> ID0000051 --> ID0000052 --> ID0000053 --> ID0000054 \
**MV2**: \
ID0000003 --> ID0000009 --> ID0000010 --> ID0000019 --> ID0000022 --> ID0000026 --> ID0000033 --> ID0000035 --> ID0000046 --> ID0000047 --> ID0000055 --> ID0000056 \
**MV3**: \
ID0000002 --> ID0000005 --> ID0000011 --> ID0000012 --> ID0000013 --> ID0000015 --> ID0000017 --> ID0000018 --> ID0000023 --> ID0000039 --> ID0000057 \
**MV4**: \
ID0000004 --> ID0000007 --> ID0000008 --> ID0000014 --> ID0000016 --> ID0000020 --> ID0000021 --> ID0000030 --> ID0000031 --> ID0000034 --> ID0000036 --> ID0000037 --> ID0000040 --> ID0000041 --> ID0000042 --> ID0000058

### Distribuições dos Arquivos por Dispositivos de Armazenamento (#2)

Nesta subseção, a tabela abaixo apresenta onde cada um dos arquivos estáticos, presentes desde o início da execução, e cada um dos arquivos dinâmicos, gerados durante a execução, deverão estar no final da execução do *workflow*.

|  **Dispersão dos Dados** | |
|:-:|:-|
|**MV1**|
| |region.hdr
| |region-oversized.hdr
| |poss2ukstu_blue_001_001.fits
| |poss2ukstu_blue_001_002.fits
| |poss2ukstu_blue_002_001.fits
| |poss2ukstu_blue_002_002.fits
| |1-stat.tbl
| |1-images.tbl
| |1-projected.tbl
| |1-corrected.tbl
| |poss2ukstu_red_001_001.fits
| |poss2ukstu_red_001_002.fits
| |poss2ukstu_red_002_001.fits
| |poss2ukstu_red_002_002.fits
| |2-stat.tbl
| |2-images.tbl
| |2-projected.tbl
| |2-corrected.tbl
| |poss2ukstu_ir_001_001.fits
| |poss2ukstu_ir_001_002.fits
| |poss2ukstu_ir_002_001.fits
| |poss2ukstu_ir_002_002.fits
| |3-stat.tbl
| |3-images.tbl
| |3-projected.tbl
| |3-corrected.tbl
| |pposs2ukstu_blue_001_002.fits
| |pposs2ukstu_blue_001_002_area.fits
| |pposs2ukstu_red_001_001.fits
| |pposs2ukstu_red_001_001_area.fits
| |pposs2ukstu_red_001_002.fits
| |pposs2ukstu_red_001_002_area.fits
| |pposs2ukstu_red_002_001_area.fits
| |2-fits.tbl
| |pposs2ukstu_ir_001_002.fits
| |pposs2ukstu_ir_001_002_area.fits
| |pposs2ukstu_ir_002_001.fits
| |pposs2ukstu_ir_002_001_area.fits
| |3-fits.tbl
|**MV2**|
| |1-fit.000002.000004.txt
| |1-diff.000002.000004.fits
| |1-diff.000002.000004_area.fits
| |2-diff.000002.000004_area.fits
| |cposs2ukstu_red_001_002.fits
| |cposs2ukstu_red_001_002_area.fits
| |cposs2ukstu_red_002_002.fits
| |cposs2ukstu_red_002_002_area.fits
| |3-fit.000001.000003.txt
| |3-diff.000001.000003.fits
| |3-diff.000001.000003_area.fits
| |3-fit.000002.000003.txt
| |3-diff.000002.000003.fits
| |3-diff.000002.000003_area.fits
| |3-fit.000002.000004.txt
| |3-diff.000002.000004.fits
| |3-fit.000003.000004.txt
| |3-diff.000003.000004.fits
| |3-diff.000003.000004_area.fits
|**MV3**|
| |pposs2ukstu_blue_001_001.fits
| |pposs2ukstu_blue_001_001_area.fits
| |1-fit.000002.000003.txt
| |1-diff.000002.000003.fits
| |1-updated-corrected.tbl
| |2-corrections.tbl
| |2-mosaic.png
| |3-diff.000002.000004_area.fits
|**MV4**|
| |pposs2ukstu_blue_002_002.fits
| |pposs2ukstu_blue_002_002_area.fits
| |1-diff.000002.000003_area.fits
| |cposs2ukstu_blue_001_002.fits
| |cposs2ukstu_blue_001_002_area.fits
| |pposs2ukstu_red_002_001.fits
| |2-fit.000001.000004.txt
| |2-diff.000001.000004.fits
| |2-diff.000001.000004_area.fits
| |2-fit.000002.000004.txt
| |2-diff.000002.000004.fits
| |2-updated-corrected.tbl
| |pposs2ukstu_ir_002_002.fits
| |pposs2ukstu_ir_002_002_area.fits
|***Bucket1***|
| |pposs2ukstu_red_002_002.fits
| |2-fit.000001.000002.txt
| |cposs2ukstu_ir_002_002_area.fits
|***Bucket2***|
| |pposs2ukstu_blue_002_001_area.fits
| |2-diff.000001.000002_area.fits
| |pposs2ukstu_ir_001_001.fits
| |3-mosaic_area.fits
|***Bucket3***|
| |1-diff.000001.000004_area.fits
| |2-fit.000002.000003.txt
| |pposs2ukstu_ir_001_001_area.fits
| |cposs2ukstu_ir_001_002_area.fits
|***Bucket4***|
| |pposs2ukstu_blue_002_001.fits
|***Bucket5***|
| |1-diff.000001.000002.fits
| |cposs2ukstu_blue_001_001_area.fits
| |2-diff.000002.000003.fits
| |cposs2ukstu_ir_002_001.fits
|***Bucket6***|
| |1-diff.000001.000003.fits
| |1-fit.000003.000004.txt
| |1-corrections.tbl
| |1-mosaic.fits
| |2-diff.000001.000003.fits
| |cposs2ukstu_red_001_001_area.fits
| |cposs2ukstu_red_002_001.fits
| |3-fit.000001.000002.txt
| |3-corrections.tbl
| |3-updated-corrected.tbl
|***Bucket7***|
| |1-fit.000001.000002.txt
| |cposs2ukstu_blue_002_002.fits
| |1-mosaic.png
| |2-fit.000001.000003.txt
| |cposs2ukstu_red_002_001_area.fits
| |cposs2ukstu_ir_002_001_area.fits
| |mosaic-color.png
|***Bucket8***|
| |1-fit.000001.000003.txt
| |2-mosaic_area.fits
| |3-fit.000001.000004.txt
| |cposs2ukstu_ir_001_002.fits
| |cposs2ukstu_ir_002_002.fits
|***Bucket9***|
| |1-diff.000003.000004_area.fits
| |2-diff.000003.000004.fits
| |2-diff.000003.000004_area.fits
|***Bucket10***|
| |cposs2ukstu_blue_001_001.fits
| |cposs2ukstu_blue_002_002_area.fits
| |cposs2ukstu_ir_001_001_area.fits
|***Bucket11***|
| |1-fit.000001.000004.txt
|***Bucket12***|
| |1-diff.000003.000004.fits
| |pposs2ukstu_red_002_002_area.fits
| |2-diff.000001.000003_area.fits
| |3-diff.000001.000002.fits
| |3-diff.000001.000004_area.fits
|***Bucket13***|
| |1-fits.tbl
| |cposs2ukstu_blue_002_001.fits
| |2-mosaic.fits
| |3-diff.000001.000002_area.fits
| |3-diff.000001.000004.fits
| |cposs2ukstu_ir_001_001.fits
|***Bucket14***|
| |1-diff.000001.000002_area.fits
| |cposs2ukstu_blue_002_001_area.fits
| |2-diff.000001.000002.fits
| |3-mosaic.png
|***Bucket15***|
| |1-diff.000001.000004.fits
| |1-mosaic_area.fits
| |2-fit.000003.000004.txt
| |cposs2ukstu_red_001_001.fits
| |3-mosaic.fits
|***Bucket16***|
| |1-diff.000001.000003_area.fits
| |2-diff.000002.000003_area.fits

### Ordem de Execução (#2)

A ordem para executar o *workflow* é a seguinte:

ID0000023 --> ID0000039 --> ID0000041 --> ID0000040 --> ID0000001 --> ID0000022 --> ID0000020 --> ID0000021 --> ID0000003 --> ID0000042 --> ID0000002 --> ID0000004 --> ID0000024 --> ID0000027 --> ID0000025 --> ID0000048 --> ID0000026 --> ID0000047 --> ID0000029 --> ID0000046 --> ID0000005 --> ID0000028 --> ID0000043 --> ID0000045 --> ID0000010 --> ID0000009 --> ID0000006 --> ID0000044 --> ID0000007 --> ID0000008 --> ID0000011 --> ID0000049 --> ID0000030 --> ID0000050 --> ID0000012 --> ID0000031 --> ID0000033 --> ID0000051 --> ID0000052 --> ID0000034 --> ID0000015 --> ID0000054 --> ID0000035 --> ID0000053 --> ID0000013 --> ID0000016 --> ID0000014 --> ID0000032 --> ID0000055 --> ID0000017 --> ID0000036 --> ID0000037 --> ID0000056 --> ID0000018 --> ID0000038 --> ID0000057 --> ID0000058 --> ID0000019

## Escalonamento #3

Neste escalonamento, estamos considerando a execução numa nuvem com as 4 máquinas virtuais definidas em [Arquitetura](#arquitetura) com 32 *buckets*.

### Ativações por Máquina Virtual (#3)

**MV1**: \
ID0000007 --> ID0000013 --> ID0000015 --> ID0000021 --> ID0000023 --> ID0000026 --> ID0000037 --> ID0000038 --> ID0000053 --> ID0000054 \
**MV2**: \
ID0000005 --> ID0000006 --> ID0000010 --> ID0000011 --> ID0000014 --> ID0000020 --> ID0000024 --> ID0000027 --> ID0000029 --> ID0000035 --> ID0000040 --> ID0000046 --> ID0000047 --> ID0000048 --> ID0000052 --> ID0000056 --> ID0000057 \
**MV3**: \
ID0000016 --> ID0000019 --> ID0000022 --> ID0000028 --> ID0000030 --> ID0000031 --> ID0000032 --> ID0000039 --> ID0000042 --> ID0000043 --> ID0000044 --> ID0000045 --> ID0000051 --> ID0000055 \
**MV4**: \
ID0000001 --> ID0000002 --> ID0000003 --> ID0000004 --> ID0000008 --> ID0000009 --> ID0000012 --> ID0000017 --> ID0000018 --> ID0000025 --> ID0000033 --> ID0000034 --> ID0000036 --> ID0000041 --> ID0000049 --> ID0000050 --> ID0000058

### Distribuições dos Arquivos por Dispositivos de Armazenamento (#3)

|  **Dispersão dos Dados** | |
|:-:|:-|
|**MV1**| |
| |region.hdr |
| |region-oversized.hdr |
| |poss2ukstu_blue_001_001.fits |
| |poss2ukstu_blue_001_002.fits |
| |poss2ukstu_blue_002_001.fits |
| |poss2ukstu_blue_002_002.fits |
| |1-stat.tbl |
| |1-images.tbl |
| |1-projected.tbl |
| |1-corrected.tbl |
| |poss2ukstu_red_001_001.fits |
| |poss2ukstu_red_001_002.fits |
| |poss2ukstu_red_002_001.fits |
| |poss2ukstu_red_002_002.fits |
| |2-stat.tbl |
| |2-images.tbl |
| |2-projected.tbl |
| |2-corrected.tbl |
| |poss2ukstu_ir_001_001.fits |
| |poss2ukstu_ir_001_002.fits |
| |poss2ukstu_ir_002_001.fits |
| |poss2ukstu_ir_002_002.fits |
| |3-stat.tbl |
| |3-images.tbl |
| |3-projected.tbl |
| |3-corrected.tbl |
| |pposs2ukstu_blue_001_001.fits |
| |pposs2ukstu_blue_001_001_area.fits |
| |2-diff.000002.000004_area.fits |
| |2-mosaic.png |
| |pposs2ukstu_ir_001_001.fits |
| |pposs2ukstu_ir_001_002.fits |
| |pposs2ukstu_ir_001_002_area.fits |
| |pposs2ukstu_ir_002_002.fits |
| |3-fits.tbl |
|**MV2**| |
| |pposs2ukstu_blue_001_002.fits |
| |pposs2ukstu_blue_001_002_area.fits |
| |1-fit.000001.000003.txt |
| |1-diff.000001.000003.fits |
| |1-diff.000001.000003_area.fits |
| |2-diff.000002.000003_area.fits |
| |cposs2ukstu_red_001_001.fits |
| |cposs2ukstu_red_001_001_area.fits |
| |cposs2ukstu_red_001_002.fits |
| |cposs2ukstu_red_001_002_area.fits |
| |cposs2ukstu_red_002_001.fits |
| |cposs2ukstu_red_002_001_area.fits |
| |cposs2ukstu_red_002_002.fits |
| |cposs2ukstu_red_002_002_area.fits |
| |pposs2ukstu_ir_002_001.fits |
| |pposs2ukstu_ir_002_001_area.fits |
| |pposs2ukstu_ir_002_002_area.fits |
| |3-fit.000001.000002.txt |
| |3-diff.000001.000002_area.fits |
| |3-fit.000001.000003.txt |
| |3-diff.000001.000003.fits |
| |3-diff.000001.000003_area.fits |
| |3-diff.000002.000003.fits |
| |3-corrections.tbl |
| |3-mosaic.fits |
| |3-mosaic_area.fits |
|**MV3**| |
| |pposs2ukstu_red_001_001.fits |
| |pposs2ukstu_red_001_001_area.fits |
| |2-fit.000002.000003.txt |
| |2-diff.000002.000003.fits |
| |2-fit.000002.000004.txt |
| |2-diff.000002.000004.fits |
| |2-corrections.tbl |
| |pposs2ukstu_ir_001_001_area.fits |
| |3-updated-corrected.tbl |
|**MV4**| |
| |pposs2ukstu_blue_002_001.fits |
| |pposs2ukstu_blue_002_001_area.fits |
| |pposs2ukstu_red_002_001.fits |
| |pposs2ukstu_red_002_001_area.fits |
| |2-updated-corrected.tbl |
| |3-diff.000001.000002.fits |
| |3-fit.000001.000004.txt |
| |3-diff.000001.000004.fits |
| |3-diff.000001.000004_area.fits |
| |3-fit.000002.000003.txt |
| |3-diff.000002.000003_area.fits |
| |3-fit.000002.000004.txt |
| |3-diff.000002.000004.fits |
| |3-diff.000002.000004_area.fits |
| |3-fit.000003.000004.txt |
| |3-diff.000003.000004.fits |
| |3-diff.000003.000004_area.fits |
| |cposs2ukstu_ir_001_001.fits |
| |cposs2ukstu_ir_001_001_area.fits |
| |cposs2ukstu_ir_001_002.fits |
| |cposs2ukstu_ir_001_002_area.fits |
| |cposs2ukstu_ir_002_001.fits |
| |cposs2ukstu_ir_002_001_area.fits |
| |cposs2ukstu_ir_002_002.fits |
| |cposs2ukstu_ir_002_002_area.fits |
| |3-mosaic.png |
|***Bucket1***| |
| |cposs2ukstu_blue_001_002.fits |
| |2-diff.000001.000004_area.fits |
|***Bucket2***| |
| |1-fit.000001.000002.txt |
| |1-fit.000003.000004.txt |
|***Bucket3***| |
| |pposs2ukstu_red_002_002_area.fits |
|***Bucket4***| |
| |1-mosaic_area.fits |
|***Bucket5***| |
| |pposs2ukstu_blue_002_002.fits |
| |2-mosaic.fits |
|***Bucket6***| |
| |1-fits.tbl |
| |1-mosaic.fits |
|***Bucket7***| |
| |1-diff.000001.000002.fits |
| |2-fits.tbl |
|***Bucket8***| |
| |pposs2ukstu_red_002_002.fits |
|***Bucket9***| |
| |cposs2ukstu_blue_002_002_area.fits |
| |2-diff.000003.000004.fits |
|***Bucket10***| |
| |pposs2ukstu_red_001_002_area.fits |
|***Bucket11***| |
| |1-fit.000002.000003.txt |
| |1-diff.000002.000003.fits |
|***Bucket12***| |
| |pposs2ukstu_blue_002_002_area.fits |
| |2-diff.000001.000003_area.fits |
|***Bucket13***| |
| |1-diff.000002.000003_area.fits |
| |1-diff.000003.000004.fits |
| |2-diff.000001.000004.fits |
|***Bucket14***| |
| |1-diff.000001.000004_area.fits |
| |pposs2ukstu_red_001_002.fits |
|***Bucket15***| |
| |cposs2ukstu_blue_001_002_area.fits |
|***Bucket16***| |
| |cposs2ukstu_blue_002_001.fits |
|***Bucket17***| |
| |1-diff.000003.000004_area.fits |
| |cposs2ukstu_blue_001_001_area.fits |
| |2-fit.000003.000004.txt |
|***Bucket18***| |
| |2-fit.000001.000002.txt |
| |2-diff.000001.000002_area.fits |
| |mosaic-color.png |
|***Bucket19***| |
| |cposs2ukstu_blue_001_001.fits |
| |2-mosaic_area.fits |
|***Bucket20***| |
| |1-diff.000001.000002_area.fits |
| |1-fit.000002.000004.txt |
| |2-diff.000001.000003.fits |
|***Bucket21***| |
| |1-diff.000002.000004.fits |
| |2-diff.000003.000004_area.fits |
|***Bucket22***| |
| |cposs2ukstu_blue_002_002.fits |
| |1-mosaic.png |
| |2-fit.000001.000003.txt |
| |2-fit.000001.000004.txt |
|***Bucket23***| |
| |cposs2ukstu_blue_002_001_area.fits |
|***Bucket24***|
| |1-diff.000001.000004.fits |
|***Bucket25***| |
| |1-diff.000002.000004_area.fits |
|***Bucket26***| |
| |1-fit.000001.000004.txt |
|***Bucket27***| |
| |2-diff.000001.000002.fits |
|***Bucket28***| |
| |1-corrections.tbl |
|***Bucket29***| |
| |1-updated-corrected.tbl |
|***Bucket30***| |
| | \<vazio\> |
|***Bucket31***| |
| | \<vazio\> |
|***Bucket32***| |
| | \<vazio\> |

### Ordem de Execução (#3)

A ordem para executar o *workflow* é a seguinte:

ID0000021 --> ID0000023 --> ID0000022 --> ID0000041 --> ID0000042 --> ID0000039 --> ID0000004 --> ID0000040 --> ID0000003 --> ID0000001 --> ID0000020 --> ID0000002 --> ID0000043 --> ID0000046 --> ID0000027 --> ID0000047 --> ID0000008 --> ID0000044 --> ID0000048 --> ID0000006 --> ID0000026 --> ID0000045 --> ID0000024 --> ID0000028 --> ID0000005 --> ID0000029 --> ID0000010 --> ID0000025 --> ID0000007 --> ID0000009 --> ID0000049 --> ID0000011 --> ID0000030 --> ID0000050 --> ID0000012 --> ID0000031 --> ID0000032 --> ID0000051 --> ID0000054 --> ID0000033 --> ID0000034 --> ID0000053 --> ID0000016 --> ID0000052 --> ID0000015 --> ID0000035 --> ID0000013 --> ID0000014 --> ID0000036 --> ID0000055 --> ID0000017 --> ID0000056 --> ID0000037 --> ID0000018 --> ID0000057 --> ID0000038 --> ID0000019 --> ID0000058