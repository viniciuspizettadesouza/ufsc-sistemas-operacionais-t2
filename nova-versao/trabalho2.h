#include <stdbool.h>
#include <stdio.h>

typedef struct Pages
{
    int frame;
    int page_number;
    struct Pages *next_page;
} page_t;

typedef struct Process
{
    int id;
    int size_in_bytes;
    int p;
    int d;
    struct Process *next_process;
    struct Pages *table_pages;
    int *address;
} process_t;


typedef struct Memory
{
    int frames_number; 
    int f;
    int d;
    int size_KB;
    int process_max_size;
    int *address;
} memory_t;

bool process_bool(int id, process_t *processos);
int insert_frame(int pagina, process_t *processo, memory_t *memory);
process_t *get_process(int id, process_t *processos);

unsigned count(unsigned int number);
bool multiple(unsigned int number, unsigned int mult);
unsigned int kb_to_bytes(int kb);
unsigned int bytes_to_kb(int bytes);

void print_screen();
void input_process_max_size();
void input_memory_size();
void input_page_size();
void view_menu(unsigned int physical_memory_size, unsigned int process_max_size, unsigned int page_size);
void view_memory(memory_t *physical_memory);
void view_table_page(int number, process_t *processos, memory_t *physical_memory);