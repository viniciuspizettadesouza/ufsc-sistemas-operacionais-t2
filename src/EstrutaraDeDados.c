#include "EstrutaraDeDados.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

bool quadroVazio(int quadro, memoria_t *memoria) {
    if (memoria == NULL) {
        return false;
    }
    return memoria->enderecos[quadro] == -1;
}

int inserirQuadro(int pagina, processo_t *processo, memoria_t *memoria) {
    int quadro = -1;
    int paginas = memoria->tamanho_KB / (int)pow(2, memoria->f);
    int deslocamentoKB = (int)pow(2, processo->d) / 1024;
    for (int i = 0; i < memoria->tamanho_KB; i += deslocamentoKB) {
        if (memoria->enderecos[i] == -1) {
            for (int j = 0; j < deslocamentoKB; j++) {
                memoria->enderecos[i + j] = processo->enderecos[(pagina * deslocamentoKB) + j];
            }
            return i / paginas;
        }
    }
    return quadro;
}
