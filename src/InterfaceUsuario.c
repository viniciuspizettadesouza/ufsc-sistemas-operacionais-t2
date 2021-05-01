#include "InterfaceUsuario.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Calculos.h"

void limpaMenu() {
#if defined _WIN32
    system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined(__APPLE__)
    system("clear");
#endif
}

void imprimirMenu() {
    printf("Menu Principal\n\n");
    printf("\033[1;32ma. \033[0m Visualizar memória.\n");
    printf("\033[1;32mb. \033[0m Criar processo.\n");
    printf("\033[1;32mc. \033[0m Visualizar tabela de páginas:.\n");
    printf("\033[1;32md. \033[0m Reinicar programa:.\n");
    printf("\033[1;32me. \033[0m Sair.\n");
}

void pegarTamanhoMemFisica() {
    printf("Informe o tamanho da memória física em \033[1;33mKB\033[0m!!!\n");
}

void pegarTamanhoMaxProcesso() {
    printf("Informe o tamanho maxímo de processo em \033[1;33mKB\033[0m!!!\n");
}

void pegarTamanhoPagina() {
    printf("Informe o tamanho da pagína \033[1;33mKB\033[0m!!!\n");
}

void imprimeResumoMenu1(unsigned int tam_memorica_fisica, unsigned int tam_max_processo, unsigned int tam_pagina) {
    printf("Tamanho da memória fisíca = \033[1;32m%d Bits \033[0m\n", tam_memorica_fisica);
    printf("Tamanho Maxímo de um processo = \033[1;32m%d KB\033[0m\n", tam_max_processo);
    printf("Tamanho da pagína = \033[1;32m%d KB\033[0m\n\n", tam_pagina);
}

void imprimirBinario(int numero, int deslocamento) {
    int c, k;
    for (c = deslocamento; c >= 0; c--) {
        k = numero >> c;

        if (k & 1)
            printf("1");
        else
            printf("0");
    }
}

void imprimirBinarioTabela(int pagina2) {
    int a[10], n, i;
    n = pagina2;
    for (i = 0; n > 0; i++) {
        a[i] = n % 2;
        n = n / 2;
    }
    for (i = i - 1; i >= 0; i--) {
        printf("%d", a[i]);
    }
}

void retornarPorcentagem(memoria_t *memoria_fisica) {
    int livre = 0;
    float per;
    for (int i = memoria_fisica->tamanho_KB - 1; i > 0; i--) {
        if (memoria_fisica->enderecos[i] == -1) {
            livre++;
        }
    }

    per = ((float)livre / memoria_fisica->tamanho_KB) * 100;
    printf("\nPorcetagm de mememoria livre = \033[1;32m%.2f%% \033[0m\n", per);
}

void visualizarMemoria(memoria_t *memoria_fisica) {
    int tamanho_pagina = memoria_fisica->tamanho_KB / (int)pow(2, memoria_fisica->f);
    int total_bits = memoria_fisica->f + contadorDeBits(memoria_fisica->tamanho_max_processo / tamanho_pagina);

    printf("\n+-------+----------+\n");
    printf("| Dado  | Endereço |\n");
    printf("+-------+----------+\n");
    for (int i = memoria_fisica->tamanho_KB - 1; i > 0; i--) {
        if (memoria_fisica->enderecos[i] == -1) {
            printf("| VAZIO |");
        } else {
            printf("|  %d  |", memoria_fisica->enderecos[i]);
        }
        printf("   ");
        imprimirBinario(i, total_bits - 1);
        printf("  |\n");
        printf("+-------+----------+\n");
    }
    retornarPorcentagem(memoria_fisica);
}

void visualizarTabalePaginas(int numero, processo_t *processos, memoria_t *memoria_fisica) {
    processo_t *tmp = pegarProcesso(numero, processos);
    pagina_t *tmp_paginas = tmp->tabela_paginas;
    printf("Tamanho do proceso:\033[1;32m %d\033[0m é de \033[1;34m %d bytes\033[0m!!!\n", numero, tmp->tamanho_bytes);
    int deslocamento_memoria = memoria_fisica->f - 1;

    printf("\n+--------+--------+\n");
    printf("| Página | Quadro |\n");
    printf("+--------+--------+\n");
    while (tmp_paginas != NULL) {
        printf("|    ");
        int vai_1 = tmp_paginas->numeroPagina;
        int vai_2 = tmp->p - 1;
        int vai_3 = tmp_paginas->quadro;
        int vai_4 = deslocamento_memoria;
        imprimirBinario(vai_1, vai_2);
        printf("  |  ");
        imprimirBinario(vai_3, vai_4);
        printf("   |\n");
        printf("+--------+--------+\n");
        tmp_paginas = tmp_paginas->proximaPagina;
    }
}