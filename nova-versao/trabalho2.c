#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ArrayList.c"
#include "trabalho2.h"

page_t *first, *last;

// Iniciando a lista de process
process_t *process = NULL;
// Iniciando a memórica
memory_t *physical_memory = NULL;
char option;
unsigned int process_number = 0, process_max_size = 0, physical_memory_size = 0, page_size = 0, used_pages = 0;

// MEMÓRIA LÓGICA
int p;  //	número da página: usado como um índice em uma tabela de página
int d;  //	page-offset (Deslocamento)

// MEMÓRIA FÍSICA
int f;                  // Número do frame: endereço básico de cada página na memória física
int frames_number;      // Memória física onde o frame 0 vai dos quadros [0] aos quadros [X]
int table_pages[1];     // Uma entrada de sinalizador livre (-1) ou alocada (0) para cada frame de 0 a X
int start, current;
int offset, pagefault = 0;
int freeFrame = -1;

volatile bool exit_menu;

void add_page(page_t *head, page_t *new_page) {
    page_t *current = head;
    while (current->next_page != NULL) {
        current = current->next_page;
    }

    current->next_page = (page_t *)malloc(sizeof(page_t));
    current->next_page->page_number = new_page->page_number;
    current->next_page->frame = new_page->frame;
    current->next_page->next_page = NULL;
}

bool add_process(unsigned int indentificador, unsigned int tamanho_processo) {
    bool processo_adicionado = false;
    process_t *tmp_processo = (process_t *)malloc(sizeof(process_t));
    page_t *tmp_paginas = NULL;
    tmp_paginas = NULL;

    tmp_processo->id = indentificador;
    tmp_processo->size_in_bytes = tamanho_processo;
    tmp_processo->p = count(BytesInKB(tamanho_processo) / page_size);
    tmp_processo->d = count(page_size * 1024);
    tmp_processo->address = malloc(kbInBytes(tamanho_processo) * sizeof(int));

    unsigned int t = BytesInKB(tamanho_processo);

    for (unsigned int z = 0; z < t; z++) {
        tmp_processo->address[z] = rand() % (page_size * 1024);
    }

    unsigned int paginas = BytesInKB(tmp_processo->size_in_bytes) / page_size;

    for (unsigned int i = 0; i < paginas; i++) {
        if (tmp_paginas == NULL) {
            tmp_paginas = (page_t *)malloc(sizeof(page_t));
            tmp_paginas->next_page = NULL;
            tmp_paginas->page_number = i;
            tmp_paginas->frame = inserirQuadro(i, tmp_processo, physical_memory);
        } else {
            page_t *teste_paginas = (page_t *)malloc(sizeof(page_t));
            teste_paginas->next_page = NULL;
            teste_paginas->page_number = i;
            teste_paginas->frame = inserirQuadro(i, tmp_processo, physical_memory);
            add_page(tmp_paginas, teste_paginas);
        }
    }
    tmp_processo->table_pages = tmp_paginas;

    // Verifica se existe process já criados
    if (process == NULL) {
        process = (process_t *)malloc(sizeof(process_t));
        process = tmp_processo;
    } else {
        process_t *current = process;
        while (current->next_process != NULL) {
            current = current->next_process;
        }
        current->next_process = tmp_processo;
        current->next_process->next_process = NULL;
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
            physical_memory->frames_number = (physical_memory_size / page_size);
            physical_memory->f = count(physical_memory->frames_number);
            physical_memory->d = count(page_size * 1024);
            physical_memory->address = malloc(physical_memory->size_KB * sizeof(int));
            physical_memory->process_max_size = process_max_size;

            for (unsigned int i = 0; i < physical_memory->size_KB; i++) {
                physical_memory->address[i] = -1;
            }
        }

    } while (!exit_menu);
    reviewMenu(physical_memory_size, process_max_size, page_size);
}

void menu() {
    print_screen();
    do {
        exit_menu = false;
        int process_size = 0, process_id = 0;
        scanf("%c", &option);
        switch (option) {
            
            case '1':
                printf("\nVisualizar memória:\n");
                viewMemory(physical_memory);
                exit_menu = true;
                menu();
                break;
            
            case '2':;
                exit_menu = true;
                do {
                    if (process_id == 0) {
                        printf("Digite o id do processo\n");
                        scanf("%d", &process_id);
                    }

                    if (process_size == 0) {
                        printf("Digite o tamanho do processo: %d em bytes\n", process_id);
                        scanf("%d", &process_size);
                    }

                    if (BytesInKB(process_size) > process_max_size) {
                        printf("Processo %d, excede tamanho limite: %d Bytes\n", process_id, kbInBytes(process_max_size));
                        process_size = 0;
                        exit_menu = false;
                    }
                    if (process_bool(process_id, process)) {
                        exit_menu = true;
                        printf("Processo %d, já existe\n", process_id);
                        menu();
                    }

                    if (process_id > 0 && process_size > 0) {
                        if (add_process(process_id, process_size)) {
                            exit_menu = true;
                            printf("Processo = %d Criado com sucesso\n\n", process_id);
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
                scanf("%d", &process_id);
                if (process_bool(process_id, process)) {
                    viewTablePage(process_id, process, physical_memory);
                } else {
                    exit_menu = true;
                    printf("Processo %d, não existe\n", process_id);
                }
                menu();
                break;
            default:
                break;
        }

    } while (option != '5');
}

// DATA STRUCTURE
bool process_bool(int id, process_t *process) {
    if (process == NULL) {
        return false;
    }

    process_t *tmp = process;
    while (tmp != NULL) {
        if (tmp->id == id) {
            return true;
        }
        tmp = tmp->next_process;
    }
    return false;
}

process_t *get_process(int id, process_t *process) {
    if (process == NULL) {
        return NULL;
    }

    process_t *tmp = process;
    while (tmp != NULL) {
        if (tmp->id == id) {
            return tmp;
        }
        tmp = tmp->next_process;
    }
    return NULL;
}

bool quadroVazio(int frame, memory_t *memoria) {
    if (memoria == NULL) {
        return false;
    }
    return memoria->address[frame] == -1;
}

int inserirQuadro(int pagina, process_t *processo, memory_t *memoria) {
    int frame = -1;
    int paginas = memoria->size_KB / (int)pow(2, memoria->f);
    int deslocamentoKB = (int)pow(2, processo->d) / 1024;
    for (int i = 0; i < memoria->size_KB; i += deslocamentoKB) {
        if (memoria->address[i] == -1) {
            for (int j = 0; j < deslocamentoKB; j++) {
                memoria->address[i + j] = processo->address[(pagina * deslocamentoKB) + j];
            }
            return i / paginas;
        }
    }
    return frame;
}


// CALCS
unsigned count(unsigned int number) {
    // Função de log na base 2
    // pegue apenas parte inteira
    return (int)log2(number);
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
    printf("\n\n Menu:\n\n");
    printf(" 1. Visualizar memória.\n 2. Criar processo.\n 3. Finalizar processo:.\n 4. Visualizar tabela de páginas:.\n 5. Sair. \n");
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


void printBynary(int number, int deslocamento) {
    int c, k;
    for (c = deslocamento; c >= 0; c--) {
        k = number >> c;

        if (k & 1)
            printf("1");
        else
            printf("0");
    }
}

void viewMemory(memory_t *physical_memory) {
    int page_size = physical_memory->size_KB / (int)pow(2, physical_memory->f);
    int total_bits = physical_memory->f + count(physical_memory->process_max_size / page_size);

    printf("Dado   -  Endereço\n");
    printf("                  \n");
    for (int i = physical_memory->size_KB - 1; i > 0; i--) {
        if (physical_memory->address[i] == -1) {
            printf("vazio  -   ");
        } else {
            printf(" %d  -   ", physical_memory->address[i]);
        }
        printBynary(i, total_bits - 1);
        printf("\n");        
    }
}

void viewTablePage(int number, process_t *process, memory_t *physical_memory) {
    process_t *tmp = get_process(number, process);
    page_t *tmp_paginas = tmp->table_pages;
    printf("Tamanho do proceso: %d é de %d bytes\n", number, tmp->size_in_bytes);
    int deslocamento_memoria = physical_memory->f - 1;

    printf("Página - Quadro \n");

    while (tmp_paginas != NULL) {
        int vai_1 = tmp_paginas->page_number;
        int vai_2 = tmp->p - 1;
        int vai_3 = tmp_paginas->frame;
        int vai_4 = deslocamento_memoria;
        printBynary(vai_1, vai_2);
        printBynary(vai_3, vai_4);
        printf("\n");
        tmp_paginas = tmp_paginas->next_page;
    }
}

int main(int argc, char *argv[]) {
    printf("Gerenciamento de Memória com Paginação\n");
    init_program();
    menu();

    return 0;
}