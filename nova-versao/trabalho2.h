#include <stdbool.h>
#include <stdio.h>

typedef struct Pages {
    int frame;
    int page_number;
    struct Pages *next_page;
} page_t;

// Estrutura do processo
typedef struct Process {
    int id;
    int size_in_bytes;
    // Bits de pagína
    int p;
    // Bits de deslocamento
    int d;
    struct Process *next_process;
    struct Pages *table_pages;
    int *address;
} process_t;

// MEMÓRIA FÍSICA
typedef struct Memory {
    // Número de quadros
    int frames_number;
    // Bits de frames
    int f;
    // Bits de deslocamento
    int d;
    // Tamanho tototal de mémoria
    int size_KB;
    // Tamano maximo Process
    int process_max_size;
    // Array de Bytes de endereços
    int *address;
} memory_t;

bool process_bool(int id, process_t *processos);
bool quadroVazio(int frame, memory_t *memory);
int inserirQuadro(int pagina, process_t *processo, memory_t *memory);
process_t *get_process(int id, process_t *processos);

unsigned count(unsigned int number);
bool multiple(unsigned int number, unsigned int mult);
unsigned int kbInBytes(int kb);
unsigned int BytesInKB(int bytes);

void print_screen();
void input_process_max_size();
void input_memory_size();
void input_page_size();
void reviewMenu(unsigned int physical_memory_size, unsigned int process_max_size, unsigned int page_size);
void viewMemory(memory_t *physical_memory);
void viewTablePage(int number, process_t *processos, memory_t *physical_memory);