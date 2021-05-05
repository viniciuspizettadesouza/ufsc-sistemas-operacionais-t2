#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ArrayList.c"
#include "trabalho2.h"

// Iniciando a lista de process
process_t *process = NULL;
// Iniciando a memórica
memory_t *physical_memory = NULL;
char option;
unsigned int process_number = 0, process_max_size = 0, physical_memory_size = 0, page_size = 0;

// MEMÓRIA LÓGICA
int p; //	numero da pagina: usado como um índice em uma tabela de pagina
int d; //	page-offset (Deslocamento)

// MEMÓRIA FÍSICA
int f;              // Número do frame: endereço básico de cada pagina na memoria fisica
int frames_number;  // Memória fisica onde o frame 0 vai dos quadros [0] aos quadros [X]
int table_pages[1]; // Uma entrada de sinalizador livre (-1) ou alocada (0) para cada frame de 0 a X
int current;
int pagefault = 0;

volatile bool exit_menu;

// DATA STRUCTURE START
process_t *get_process(int id, process_t *process)
{
    if (process == NULL)
    {
        return NULL;
    }

    process_t *tmp = process;
    while (tmp != NULL)
    {
        if (tmp->id == id)
        {
            return tmp;
        }
        tmp = tmp->next_process;
    }
    return NULL;
}

int insert_frame(int page, process_t *process, memory_t *memmory)
{
    int frame = -1;
    int pages = memmory->size_KB / (int)pow(2, memmory->f);
    int shift_kb = (int)pow(2, process->d) / 1024;
    for (int i = 0; i < memmory->size_KB; i += shift_kb)
    {
        if (memmory->address[i] == -1)
        {
            for (int j = 0; j < shift_kb; j++)
            {
                memmory->address[i + j] = process->address[(page * shift_kb) + j];
            }
            return i / pages;
        }
    }
    return frame;
}

bool process_bool(int id, process_t *process)
{
    if (process == NULL)
    {
        return false;
    }

    process_t *tmp = process;
    while (tmp != NULL)
    {
        if (tmp->id == id)
        {
            return true;
        }
        tmp = tmp->next_process;
    }
    return false;
}
// DATA STRUCTURE END

// CALCS START
unsigned count(unsigned int number)
{
    // Função de log na base 2
    // pegue apenas parte inteira
    return (int)log2(number);
}

bool multiple(unsigned int number, unsigned int mult)
{
    return number % mult == 0;
}

unsigned int kb_to_bytes(int kb)
{
    return kb * 1000;
}

unsigned int bytes_to_kb(int bytes)
{
    return bytes / 1000;
}
// CALCS END

// METHODS START
void add_page(page_t *head, page_t *new_page)
{
    page_t *current = head;
    while (current->next_page != NULL)
    {
        current = current->next_page;
    }

    current->next_page = (page_t *)malloc(sizeof(page_t));
    current->next_page->page_number = new_page->page_number;
    current->next_page->frame = new_page->frame;
    current->next_page->next_page = NULL;
}

bool add_process(unsigned int id, unsigned int process_size)
{
    bool process_included = false;
    process_t *temp_process = (process_t *)malloc(sizeof(process_t));
    page_t *temp_pages = NULL;
    temp_pages = NULL;

    temp_process->id = id;
    temp_process->size_in_bytes = process_size;
    temp_process->p = count(bytes_to_kb(process_size) / page_size);
    temp_process->d = count(page_size * 1024);
    temp_process->address = malloc(kb_to_bytes(process_size) * sizeof(int));

    unsigned int t = bytes_to_kb(process_size);

    for (unsigned int z = 0; z < t; z++)
    {
        temp_process->address[z] = rand() % (page_size * 1024);
    }

    unsigned int pages = bytes_to_kb(temp_process->size_in_bytes) / page_size;

    for (unsigned int i = 0; i < pages; i++)
    {
        if (temp_pages == NULL)
        {
            temp_pages = (page_t *)malloc(sizeof(page_t));
            temp_pages->next_page = NULL;
            temp_pages->page_number = i;
            temp_pages->frame = insert_frame(i, temp_process, physical_memory);
        }
        else
        {
            page_t *test_pages = (page_t *)malloc(sizeof(page_t));
            test_pages->next_page = NULL;
            test_pages->page_number = i;
            test_pages->frame = insert_frame(i, temp_process, physical_memory);
            add_page(temp_pages, test_pages);
        }
    }
    temp_process->table_pages = temp_pages;

    // Verifica se existe process já criados
    if (process == NULL)
    {
        process = (process_t *)malloc(sizeof(process_t));
        process = temp_process;
    }
    else
    {
        process_t *current = process;
        while (current->next_process != NULL)
        {
            current = current->next_process;
        }
        current->next_process = temp_process;
        current->next_process->next_process = NULL;
    }
    return process_included = true;
}

void init()
{
    do
    {
        exit_menu = true;

        if (physical_memory_size <= 0)
        {
            input_memory_size();
            scanf("%d", &physical_memory_size);
            if (!multiple(physical_memory_size, 2))
            {
                physical_memory_size = 0;
                exit_menu = false;
            }
        }
        if (process_max_size <= 0)
        {
            input_process_max_size();
            scanf("%d", &process_max_size);
            if (!multiple(process_max_size, 2))
            {
                process_max_size = 0;
                exit_menu = false;
            }
        }
        if (page_size <= 0)
        {
            input_page_size();
            scanf("%d", &page_size);
            if (!multiple(page_size, 2))
            {
                process_max_size = 0;
                exit_menu = false;
            }
        }

        // Metódo que inicia a memoria fisica
        if (page_size > 0 && process_max_size > 0 && physical_memory_size > 0)
        {
            physical_memory = (memory_t *)malloc(sizeof(memory_t));
            physical_memory->size_KB = physical_memory_size;
            physical_memory->frames_number = (physical_memory_size / page_size);
            physical_memory->f = count(physical_memory->frames_number);
            physical_memory->d = count(page_size * 1024);
            physical_memory->address = malloc(physical_memory->size_KB * sizeof(int));
            physical_memory->process_max_size = process_max_size;

            for (unsigned int i = 0; i < physical_memory->size_KB; i++)
            {
                physical_memory->address[i] = -1;
            }
        }

    } while (!exit_menu);
    view_menu(physical_memory_size, process_max_size, page_size);
}

void menu()
{
    print_screen();
    do
    {
        exit_menu = false;
        int process_size = 0, process_id = 0;
        scanf("%c", &option);
        switch (option)
        {
        case '1':
            printf("\nVisualizar memoria:\n");
            view_memory(physical_memory);
            exit_menu = true;
            menu();
            break;

        case '2':;
            exit_menu = true;
            do
            {
                if (process_id == 0)
                {
                    printf("Digite o id do processo\n");
                    scanf("%d", &process_id);
                }
                if (process_size == 0)
                {
                    printf("Digite o tamanho do processo %d em bytes\n", process_id);
                    scanf("%d", &process_size);
                }
                if (bytes_to_kb(process_size) > process_max_size)
                {
                    printf("O processo %d, nao pode ser maior que o tamanho limite de %d Bytes\n", process_id, kb_to_bytes(process_max_size));
                    process_size = 0;
                    exit_menu = false;
                }
                if (process_bool(process_id, process))
                {
                    exit_menu = true;
                    printf("Processo %d, ja existe\n", process_id);
                    menu();
                }
                if (process_id > 0 && process_size > 0)
                {
                    if (add_process(process_id, process_size))
                    {
                        exit_menu = true;
                        printf("Processo = %d Criado com sucesso\n\n", process_id);
                        menu();
                    }
                }
            } while (!exit_menu);
            break;

        case '3':;
            process_number = 0, process_size = 0, process_max_size = 0, physical_memory_size = 0, page_size = 0;
            init();
            menu();
            break;

        case '4':
            printf("Informe o numero id do processo\n");
            scanf("%d", &process_id);
            if (process_bool(process_id, process))
            {
                view_table_page(process_id, process, physical_memory);
            }
            else
            {
                exit_menu = true;
                printf("Processo %d, nao existe\n", process_id);
            }
            menu();
            break;
        default:
            break;
        }

    } while (option != '5');
}
// METHODS END

// INTERFACE START
void print_screen()
{
    printf("\n\n Menu:\n\n");
    printf(" 1. Visualizar memoria\n 2. Criar processo\n 3. Finalizar processo\n 4. Visualizar tabela de paginas\n 5. Sair \n");
}

void input_memory_size()
{
    printf("Digite o tamanho da memoria fisica em KB\n");
}

void input_process_max_size()
{
    printf("Digite o tamanho maximo de processo em KB\n");
}

void input_page_size()
{
    printf("Digite o tamanho da pagina KB\n");
}

void view_menu(unsigned int physical_memory_size, unsigned int process_max_size, unsigned int page_size)
{
    printf("Tamanho da memoria fisica = %d Bits\n", physical_memory_size);
    printf("Tamanho maximo de um processo = %d\n", process_max_size);
    printf("Tamanho da pagina = %d\n", page_size);
}

void printBynary(int number, int shift)
{
    int c, k;
    for (c = shift; c >= 0; c--)
    {
        k = number >> c;

        if (k & 1)
            printf("1");
        else
            printf("0");
    }
}

void view_memory(memory_t *physical_memory)
{
    int page_size = physical_memory->size_KB / (int)pow(2, physical_memory->f);
    int total_bits = physical_memory->f + count(physical_memory->process_max_size / page_size);

    printf("Dado   -  Endereco\n");
    printf("                  \n");
    for (int i = physical_memory->size_KB - 1; i > 0; i--)
    {
        if (physical_memory->address[i] == -1)
        {
            printf("vazio  -   ");
        }
        else
        {
            printf(" %d  -   ", physical_memory->address[i]);
        }
        printBynary(i, total_bits - 1);
        printf("\n");
    }
}

void view_table_page(int number, process_t *process, memory_t *physical_memory)
{
    process_t *tmp = get_process(number, process);
    page_t *temp_pages = tmp->table_pages;
    printf("Tamanho do processo: %d e de %d bytes\n", number, tmp->size_in_bytes);
    int shift_memo = physical_memory->f - 1;

    printf("Pagina - Quadro \n");

    while (temp_pages != NULL)
    {
        printf("   ");
        int var_1 = temp_pages->page_number, var_2 = tmp->p - 1, var_3 = temp_pages->frame, var_4 = shift_memo;

        printBynary(var_1, var_2); printf("  -  ");
        printBynary(var_3, var_4); printf("     \n");
        temp_pages = temp_pages->next_page;
        
    }
}
// INTERFACE END

int main(int argc, char *argv[])
{
    printf("GRUPO - BRUNO ALEXANDRE E VINICIUS PIZETTA\n\n");
    printf("Trabalho sobre Gerenciamento de Memoria usando Paginacao\n");
    init();
    menu();

    return 0;
}