#include "EstrutaraDeDados.h"

void imprimirMenu();
void limpaMenu();
void pegarTamanhoMaxProcesso();
void pegarTamanhoMemFisica();
void pegarTamanhoPagina();
void imprimeResumoMenu1(unsigned int tam_memorica_fisica, unsigned int tam_max_processo, unsigned int tam_pagina);
void visualizarMemoria(memoria_t *memoria_fisica);
void visualizarTabalePaginas(int numero, processo_t *processos, memoria_t *memoria_fisica);