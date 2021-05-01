#include "Calculos.h"

#include <math.h>
#include <stdio.h>

unsigned contadorDeBits(unsigned int numero) {
    // Função de log na base 2
    // pegue apenas parte inteira
    return (int)log2(numero);
}

bool multiploPorDois(unsigned int numero) {
    return numero % 2 == 0;
}

unsigned int kbInBytes(int kb) {
    return kb * 1000;
}

unsigned int BytesInKB(int bytes) {
    return bytes / 1000;
}