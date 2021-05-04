#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ArrayList.c"
#include "trabalho2.h"

pagina_t *first, *last;

// Iniciando a lista de processos
processo_t *processos = NULL;
// Iniciando a memórica
memory_t *physical_memory = NULL;
char option;
unsigned int process_number = 0, process_max_size = 0, physical_memory_size = 0, page_size = 0, used_pages = 0;

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

volatile bool exit_menu;
pid_t pid;

void AdicionarPagina(int quadro) {
    pagina_t *atual = (pagina_t *)malloc(sizeof(pagina_t));
    atual->quadro = quadro;
    atual->proximaPagina = NULL;

    if (used_pages == 0) {
        first = atual;
        last = first;
    } else {
        last->proximaPagina = atual;
        last = atual;
    }

    if (used_pages < numero_quadros)
        used_pages++;
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
    tmp_processo->p = count(BytesInKB(tamanho_processo) / page_size);
    tmp_processo->d = count(page_size * 1024);
    tmp_processo->enderecos = malloc(kbInBytes(tamanho_processo) * sizeof(int));

    unsigned int t = BytesInKB(tamanho_processo);

    for (unsigned int z = 0; z < t; z++) {
        tmp_processo->enderecos[z] = rand() % (page_size * 1024);
    }

    unsigned int paginas = BytesInKB(tmp_processo->tamanho_bytes) / page_size;

    for (unsigned int i = 0; i < paginas; i++) {
        if (tmp_paginas == NULL) {
            tmp_paginas = (pagina_t *)malloc(sizeof(pagina_t));
            tmp_paginas->proximaPagina = NULL;
            tmp_paginas->numeroPagina = i;
            tmp_paginas->quadro = inserirQuadro(i, tmp_processo, physical_memory);
        } else {
            pagina_t *teste_paginas = (pagina_t *)malloc(sizeof(pagina_t));
            teste_paginas->proximaPagina = NULL;
            teste_paginas->numeroPagina = i;
            teste_paginas->quadro = inserirQuadro(i, tmp_processo, physical_memory);
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

void init_program() {
    do {
        exit_menu = true;

        if (physical_memory_size <= 0) {
            input_memory_size();
            scanf("%d", &physical_memory_size);
            if (!multiple(physical_memory_size, 2)) {
                physical_memory_size = 0;
                exit_menu = false;
            }
        }
        if (process_max_size <= 0) {
            input_process_max_size();
            scanf("%d", &process_max_size);
            if (!multiple(process_max_size, 2)) {
                process_max_size = 0;
                exit_menu = false;
            }
        }
        if (page_size <= 0) {
            input_page_size();
            scanf("%d", &page_size);
            if (!multiple(page_size, 2)) {
                process_max_size = 0;
                exit_menu = false;
            }
        }

        // Metódo que inicia a memória física
        if (page_size > 0 && process_max_size > 0 && physical_memory_size > 0) {
            physical_memory = (memory_t *)malloc(sizeof(memory_t));
            physical_memory->size_KB = physical_memory_size;
            physical_memory->numero_quadros = (physical_memory_size / page_size);
            physical_memory->f = count(physical_memory->numero_quadros);
            physical_memory->d = count(page_size * 1024);
            physical_memory->enderecos = malloc(physical_memory->size_KB * sizeof(int));
            physical_memory->process_max_size = process_max_size;

            for (unsigned int i = 0; i < physical_memory->size_KB; i++) {
                physical_memory->enderecos[i] = -1;
            }
        }

    } while (!exit_menu);
    reviewMenu(physical_memory_size, process_max_size, page_size);
}

void menu() {
    print_screen();
    do {
        exit_menu = false;
        int process_size = 0, temp_indentificador = 0;
        scanf("%c", &option);
        switch (option) {
            case '1':
                printf("Visualizar memória:\n");
                viewMemory(physical_memory);
                exit_menu = true;
                menu();
                break;
            case '2':;
                exit_menu = true;
                do {
                    if (temp_indentificador == 0) {
                        printf("Informe o número indentificador do processo\n");
                        scanf("%d", &temp_indentificador);
                    }

                    if (process_size == 0) {
                        printf("Informe o tamanho do processo: %d em bytes\n", temp_indentificador);
                        scanf("%d", &process_size);
                    }

                    if (BytesInKB(process_size) > process_max_size) {
                        printf("Processo %d, excede tamanho limite: %d Bytes\n", temp_indentificador, kbInBytes(process_max_size));
                        process_size = 0;
                        exit_menu = false;
                    }
                    if (existeProcesso(temp_indentificador, processos)) {
                        exit_menu = true;
                        printf("Processo %d, já existe\n", temp_indentificador);
                        menu();
                    }

                    if (temp_indentificador > 0 && process_size > 0) {
                        if (adicionarProcesso(temp_indentificador, process_size)) {
                            exit_menu = true;
                            printf("Processo = %d Criado com sucesso\n\n", temp_indentificador);
                            menu();
                        }
                    }
                } while (!exit_menu);
                break;
            case '3':;
                process_number = 0, process_size = 0, process_max_size = 0, physical_memory_size = 0, page_size = 0;
                init_program();
                break;
            case '4':
                printf("Informe o número indentificador do processo!!!\n");
                scanf("%d", &temp_indentificador);
                if (existeProcesso(temp_indentificador, processos)) {
                    visualizarTabalePaginas(temp_indentificador, processos, physical_memory);
                } else {
                    exit_menu = true;
                    printf("Processo %d, não existe\n", temp_indentificador);
                }
                menu();
                break;
            default:
                break;
        }

    } while (option != '5');
}

// ESTRUTURA DE DADOS


bool existeProcesso(int identificador, processo_t *processos) {
    if (processos == NULL) {
        return false;
    }

    processo_t *tmp = processos;
    while (tmp != NULL) {
        if (tmp->identificador == identificador) {
            return true;
        }
        tmp = tmp->proximoProcessso;
    }
    return false;
}

processo_t *pegarProcesso(int identificador, processo_t *processos) {
    if (processos == NULL) {
        return NULL;
    }

    processo_t *tmp = processos;
    while (tmp != NULL) {
        if (tmp->identificador == identificador) {
            return tmp;
        }
        tmp = tmp->proximoProcessso;
    }
    return NULL;
}

bool quadroVazio(int quadro, memory_t *memoria) {
    if (memoria == NULL) {
        return false;
    }
    return memoria->enderecos[quadro] == -1;
}

int inserirQuadro(int pagina, processo_t *processo, memory_t *memoria) {
    int quadro = -1;
    int paginas = memoria->size_KB / (int)pow(2, memoria->f);
    int deslocamentoKB = (int)pow(2, processo->d) / 1024;
    for (int i = 0; i < memoria->size_KB; i += deslocamentoKB) {
        if (memoria->enderecos[i] == -1) {
            for (int j = 0; j < deslocamentoKB; j++) {
                memoria->enderecos[i + j] = processo->enderecos[(pagina * deslocamentoKB) + j];
            }
            return i / paginas;
        }
    }
    return quadro;
}


// CALCS

unsigned count(unsigned int numero) {
    // Função de log na base 2
    // pegue apenas parte inteira
    return (int)log2(numero);
}

bool multiple(unsigned int number, unsigned int mult) {
    return number % mult == 0;
}

unsigned int kbInBytes(int kb) {
    return kb * 1000;
}

unsigned int BytesInKB(int bytes) {
    return bytes / 1000;
}

// INTERFACE
void print_screen() {
    printf("Menu:\n\n");
    printf("1. Visualizar memória.\n2. Criar processo.\n3. Finalizar processo:.\n4. Visualizar tabela de páginas:.\n5. Sair.\n");
}

void input_memory_size() {
    printf("Informe o tamanho da memória física em KB\n");
}

void input_process_max_size() {
    printf("Informe o tamanho maxímo de processo em KB\n");
}

void input_page_size() {
    printf("Informe o tamanho da pagína KB\n");
}

void reviewMenu(unsigned int physical_memory_size, unsigned int process_max_size, unsigned int page_size) {
    printf("Tamanho da memória fisíca = %d Bits\n", physical_memory_size);
    printf("Tamanho Maxímo de um processo = %d\n", process_max_size);
    printf("Tamanho da pagína = %d\n", page_size);
}

void printBynary(int numero, int deslocamento) {
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

void viewMemory(memory_t *physical_memory) {
    int page_size = physical_memory->size_KB / (int)pow(2, physical_memory->f);
    int total_bits = physical_memory->f + count(physical_memory->process_max_size / page_size);

    printf("Endereço\n");
    for (int i = physical_memory->size_KB - 1; i > 0; i--) {
        if (physical_memory->enderecos[i] == -1) {
            printf("VAZIO");
        } else {
            printf(" %d ", physical_memory->enderecos[i]);
        }
        printf("\n");
        printBynary(i, total_bits - 1);
    }
}

void visualizarTabalePaginas(int numero, processo_t *processos, memory_t *physical_memory) {
    processo_t *tmp = pegarProcesso(numero, processos);
    pagina_t *tmp_paginas = tmp->tabela_paginas;
    printf("Tamanho do proceso: %d é de %d bytes\n", numero, tmp->tamanho_bytes);
    int deslocamento_memoria = physical_memory->f - 1;

    printf("Página - Quadro \n");

    while (tmp_paginas != NULL) {
        int vai_1 = tmp_paginas->numeroPagina;
        int vai_2 = tmp->p - 1;
        int vai_3 = tmp_paginas->quadro;
        int vai_4 = deslocamento_memoria;
        printBynary(vai_1, vai_2);
        printBynary(vai_3, vai_4);
        printf("\n");
        tmp_paginas = tmp_paginas->proximaPagina;
    }
}

int main(int argc, char *argv[]) {
    printf("Gerenciamento de Memória com Paginação\n");
    init_program();
    menu();

    return 0;
}