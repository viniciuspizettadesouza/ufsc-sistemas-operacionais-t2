#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ArrayList.h"
#include "Calculos.h"
#include "EstrutaraDeDados.h"
#include "InterfaceUsuario.h"

pagina_t *primeira, *ultima, *paginas_logicas = NULL;

// Iniciando a lista de processos
processo_t *processos = NULL;
// Iniciando a memórica
memoria_t *memoria_fisica = NULL;
char opcao_menu;
unsigned int num_processo = 0, tam_max_processo = 0, tam_memorica_fisica = 0, tam_pagina = 0, paginasUsadas = 0;

// Número de paginas
unsigned int num_paginas;

// MEMÓRIA LÓGICA
int p;  //	número da página: usado como um índice em uma tabela de página
int d;  //	page-offset (Deslocamento)

// MEMÓRIA FÍSICA
int f;                  //	Número do quadro: endereço básico de cada página na memória física
int numero_quadros;     // Memória física onde o quadro 0 vai dos quadros [0] aos quadros [X]
int tabela_paginas[1];  // Uma entrada de sinalizador livre (-1) ou alocada (0) para cada quadro de 0 a X
int start, current;
int offset, pagefault = 0;
int freeFrame = -1;

volatile bool sair_menu;
pid_t pid;

void AdicionarPagina(int quadro) {
    pagina_t *atual = (pagina_t *)malloc(sizeof(pagina_t));
    atual->quadro = quadro;
    atual->proximaPagina = NULL;

    if (paginasUsadas == 0) {
        primeira = atual;
        ultima = primeira;
    } else {
        ultima->proximaPagina = atual;
        ultima = atual;
    }

    if (paginasUsadas < numero_quadros)
        paginasUsadas++;
}

void adicionarPagina(pagina_t *head, pagina_t *nova_Paggina) {
    pagina_t *current = head;
    while (current->proximaPagina != NULL) {
        current = current->proximaPagina;
    }

    /* now we can add a new variable */
    current->proximaPagina = (pagina_t *)malloc(sizeof(pagina_t));
    current->proximaPagina->numeroPagina = nova_Paggina->numeroPagina;
    current->proximaPagina->quadro = nova_Paggina->quadro;
    current->proximaPagina->proximaPagina = NULL;
}

bool adicionarProcesso(unsigned int indentificador, unsigned int tamanho_processo) {
    bool processo_adicionado = false;
    processo_t *tmp_processo = (processo_t *)malloc(sizeof(processo_t));
    pagina_t *tmp_paginas = NULL;
    tmp_paginas = NULL;

    tmp_processo->identificador = indentificador;
    tmp_processo->tamanho_bytes = tamanho_processo;
    tmp_processo->p = contadorDeBits(BytesInKB(tamanho_processo) / tam_pagina);
    tmp_processo->d = contadorDeBits(tam_pagina * 1024);
    tmp_processo->enderecos = malloc(kbInBytes(tamanho_processo) * sizeof(int));

    unsigned int t = BytesInKB(tamanho_processo);

    for (unsigned int z = 0; z < t; z++) {
        tmp_processo->enderecos[z] = rand() % (tam_pagina * 1024);
    }

    unsigned int paginas = BytesInKB(tmp_processo->tamanho_bytes) / tam_pagina;

    for (unsigned int i = 0; i < paginas; i++) {
        if (tmp_paginas == NULL) {
            tmp_paginas = (pagina_t *)malloc(sizeof(pagina_t));
            tmp_paginas->proximaPagina = NULL;
            tmp_paginas->numeroPagina = i;
            tmp_paginas->quadro = inserirQuadro(i, tmp_processo, memoria_fisica);
        } else {
            pagina_t *teste_paginas = (pagina_t *)malloc(sizeof(pagina_t));
            teste_paginas->proximaPagina = NULL;
            teste_paginas->numeroPagina = i;
            teste_paginas->quadro = inserirQuadro(i, tmp_processo, memoria_fisica);
            adicionarPagina(tmp_paginas, teste_paginas);
        }
    }
    tmp_processo->tabela_paginas = tmp_paginas;

    // Verifica se existe processos já criados
    if (processos == NULL) {
        processos = (processo_t *)malloc(sizeof(processo_t));
        processos = tmp_processo;
    } else {
        processo_t *current = processos;
        while (current->proximoProcessso != NULL) {
            current = current->proximoProcessso;
        }
        current->proximoProcessso = tmp_processo;
        current->proximoProcessso->proximoProcessso = NULL;
    }
    return processo_adicionado = true;
}

void iniciarPrograma() {
    do {
        sair_menu = true;

        if (tam_memorica_fisica <= 0) {
            pegarTamanhoMemFisica();
            scanf("%d", &tam_memorica_fisica);
            if (!multiploPorDois(tam_memorica_fisica)) {
                tam_memorica_fisica = 0;
                sair_menu = false;
            }
        }
        if (tam_max_processo <= 0) {
            pegarTamanhoMaxProcesso();
            scanf("%d", &tam_max_processo);
            if (!multiploPorDois(tam_max_processo)) {
                tam_max_processo = 0;
                sair_menu = false;
            }
        }
        if (tam_pagina <= 0) {
            pegarTamanhoPagina();
            scanf("%d", &tam_pagina);
            if (!multiploPorDois(tam_pagina)) {
                tam_max_processo = 0;
                sair_menu = false;
            }
        }

        // Metódo que inicia a memória física
        if (tam_pagina > 0 && tam_max_processo > 0 && tam_memorica_fisica > 0) {
            memoria_fisica = (memoria_t *)malloc(sizeof(memoria_t));
            memoria_fisica->tamanho_KB = tam_memorica_fisica;
            memoria_fisica->numero_quadros = (tam_memorica_fisica / tam_pagina);
            memoria_fisica->f = contadorDeBits(memoria_fisica->numero_quadros);
            memoria_fisica->d = contadorDeBits(tam_pagina * 1024);
            memoria_fisica->enderecos = malloc(memoria_fisica->tamanho_KB * sizeof(int));
            memoria_fisica->tamanho_max_processo = tam_max_processo;

            for (unsigned int i = 0; i < memoria_fisica->tamanho_KB; i++) {
                memoria_fisica->enderecos[i] = -1;
            }
        }

        limpaMenu();

    } while (!sair_menu);
    imprimeResumoMenu1(tam_memorica_fisica, tam_max_processo, tam_pagina);
}

void menuPrograma() {
    imprimirMenu();
    do {
        sair_menu = false;
        int tam_processo = 0, temp_indentificador = 0;
        scanf("%c", &opcao_menu);
        switch (toupper(opcao_menu)) {
            case 'A':
                limpaMenu();
                printf("Visualizar memória:\n");
                visualizarMemoria(memoria_fisica);
                sair_menu = true;
                menuPrograma();
                break;
            case 'C':;
                printf("Informe o \033[1;34mnúmero\033[0m indentificador do processo!!!\n");
                scanf("%d", &temp_indentificador);
                if (existeProcesso(temp_indentificador, processos)) {
                    visualizarTabalePaginas(temp_indentificador, processos, memoria_fisica);
                } else {
                    sair_menu = true;
                    limpaMenu();
                    printf("[ERROR]:Processo \033[1;32m%d\033[0m, \033[1;31mnão existe\033[0m!!!\n", temp_indentificador);
                }
                menuPrograma();
                break;
            case 'B':;
                sair_menu = true;
                do {
                    if (temp_indentificador == 0) {
                        // limpaMenu();
                        printf("Informe o \033[1;34mnúmero\033[0m indentificador do processo!!!\n");
                        scanf("%d", &temp_indentificador);
                    }

                    if (tam_processo == 0) {
                        printf("Informe o tamanho do processo: \033[1;32m%d\033[0m em \033[1;34mbytes\033[0m!!!\n", temp_indentificador);
                        scanf("%d", &tam_processo);
                    }

                    if (BytesInKB(tam_processo) > tam_max_processo) {
                        limpaMenu();
                        printf("[ERROR]:Processo \033[1;32m%d\033[0m, excede \033[1;31mtamanho limite: %d Bytes\033[0m!!!\n", temp_indentificador, kbInBytes(tam_max_processo));
                        tam_processo = 0;
                        sair_menu = false;
                    }
                    if (existeProcesso(temp_indentificador, processos)) {
                        sair_menu = true;
                        limpaMenu();
                        printf("[ERROR]:Processo \033[1;32m%d\033[0m, \033[1;31mjá existe\033[0m!!!\n", temp_indentificador);
                        menuPrograma();
                    }

                    if (temp_indentificador > 0 && tam_processo > 0) {
                        if (adicionarProcesso(temp_indentificador, tam_processo)) {
                            sair_menu = true;
                            //limpaMenu();
                            printf("Processo = \033[1;32m%d \033[0m Criado com sucesso!!\n\n", temp_indentificador);
                            menuPrograma();
                        }
                    }
                } while (!sair_menu);
                break;
            case 'D':
                limpaMenu();
                num_processo = 0, tam_processo = 0, tam_max_processo = 0, tam_memorica_fisica = 0, tam_pagina = 0;
                iniciarPrograma();
                break;

            default:
                break;
        }

    } while (opcao_menu != 'e');
}

int main(int argc, char *argv[]) {
    printf("Gerenciamento de Memória com Paginação!!!\n");
    iniciarPrograma();
    menuPrograma();

    return 0;
}