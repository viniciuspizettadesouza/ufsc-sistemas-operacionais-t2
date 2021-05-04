#include <stdbool.h>
#include <stdio.h>
#ifndef A_H_INCLUDED
#define A_H_INCLUDED

typedef struct Paginas {
    int quadro;
    int page_number;
    struct Paginas *next_page;

} page_t;

// Estrutura do processo
typedef struct Processo {
    int identificador;
    int tamanho_bytes;
    // Bits de pagína
    int p;
    // Bits de deslocamento
    int d;
    struct Processo *proximoProcessso;
    struct Paginas *tabela_paginas;
    int *enderecos;

} process_t;

// MEMÓRIA FÍSICA
typedef struct Memory {
    // Número de quadros
    int numero_quadros;
    // Bits de frames
    int f;
    // Bits de deslocamento
    int d;
    // Tamanho tototal de mémoria
    int size_KB;
    // Tamano maximo processo
    int process_max_size;
    // Array de Bytes de endereços
    int *enderecos;
} memory_t;

bool existeProcesso(int indentificador, process_t *processos);
bool quadroVazio(int quadro, memory_t *memoria);
int inserirQuadro(int pagina, process_t *processo, memory_t *memoria);
process_t *pegarProcesso(int identificador, process_t *processos);
#endif

unsigned count(unsigned int numero);
bool multiple(unsigned int number, unsigned int mult);
unsigned int kbInBytes(int kb);
unsigned int BytesInKB(int bytes);

void print_screen();
void input_process_max_size();
void input_memory_size();
void input_page_size();
void reviewMenu(unsigned int tam_memorica_fisica, unsigned int tam_max_processo, unsigned int tam_pagina);
void viewMemory(memory_t *physical_memory);
void visualizarTabalePaginas(int numero, process_t *processos, memory_t *physical_memory);