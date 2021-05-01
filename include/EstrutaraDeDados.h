#include <stdbool.h>
#ifndef A_H_INCLUDED
#define A_H_INCLUDED
typedef struct Paginas {
    int quadro;
    int numeroPagina;
    struct Paginas *proximaPagina;

} pagina_t;

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

} processo_t;

// MEMÓRIA FÍSICA
typedef struct Memoria {
    // Número de quadros
    int numero_quadros;
    // Bits de frames
    int f;
    // Bits de deslocamento
    int d;
    // Tamanho tototal de mémoria
    int tamanho_KB;
    // Tamanho da página
    int tm_pagina;
    // Tamano maximo processo
    int tamanho_max_processo;
    // Array de Bytes de endereços
    int *enderecos;
} memoria_t;

bool existeProcesso(int indentificador, processo_t *processos);
bool quadroVazio(int quadro, memoria_t *memoria);
int inserirQuadro(int pagina, processo_t *processo, memoria_t *memoria);
processo_t *pegarProcesso(int identificador, processo_t *processos);
#endif