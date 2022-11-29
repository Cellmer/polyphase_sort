#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "disk_operations.h"
#include "sorting_layer.h"
#include "file_operations.h"

#define MAX_FILENAME_LENGTH 100


// show menu with available options
void show_menu();
void clear_input_buffer();

// function counting runs in file, used only for the experiment
int count_runs(const char* filename);


int main(int argc, char* argv[])
{
    //FILE* experiment_file = fopen("experiment_file.txt", "w");
    //fprintf(experiment_file, "RECORDS\tRUNS\tTHEORETICAL_PHASES\tREAL_PHASES\tTHEORETICAL_TOTAL_DISK_OPERATIONS\tTOTAL_DISK_OPERATIONS\tb-factor\n");
    //fclose(experiment_file);

    if (argc > 1)
    {
        printf("File to sort:\n");
        print_file(argv[1]);

        sort(argv[1]);
        printf("Sorted file:\n");
        print_file(argv[1]);
    }
    char filename[MAX_FILENAME_LENGTH];
    char option;
    do
    {
        show_menu();
        scanf("%c", &option);
        switch (option)
        {
        case '1':
            printf("Enter filename you want to generate:\n ");
            scanf("%s", filename);
            printf("Enter number of records you want to generate:\n ");
            int number_of_records;
            scanf("%d", &number_of_records);

            generate_random_file(filename, number_of_records);
            printf("File to sort:\n");
            print_file(filename);
           /* fopen("experiment_file.txt", "a");
            fprintf(experiment_file, "%d\t%d\t", number_of_records, count_runs(filename));
            fclose(experiment_file);*/

            clear_save_count();
            clear_load_count();
            sort(filename);
            printf("\nDISK OPERATIONS:\n");
            printf("Blocks saved: %d\n", get_save_count());
            printf("Blocks loaded: %d\n", get_load_count());
            printf("Sorted file:\n");
            print_file(filename);
            break;
        case '2':
            printf("Enter filename you want to generate:\n ");
            scanf("%s", filename);

            generate_file_from_user_input(filename);
            printf("File to sort:\n");
            print_file(filename);

            clear_save_count();
            clear_load_count();
            sort(filename);
            printf("DISK OPERATIONS:\n");
            printf("Blocks saved: %d\n", get_save_count());
            printf("Blocks loaded: %d\n", get_load_count());
            printf("\nSorted file:\n");
            print_file(filename);
            break;
        case 'q':
            break;
        }
    } while (option != 'q');

    return 0;
}

void show_menu()
{
    clear_input_buffer();
    printf("----------------MENU----------------\n");
    printf("Choose one of the following options:\n");
    printf("1) generate random file to sort\n");
    printf("2) type records you want to sort\n");
    printf("q) quit\n");
}

void clear_input_buffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

int count_runs(const char* filename)
{
    FILE* file;
    file = fopen(filename, "rb");

    RecordExtractor re;
    record_extractor_init(&re, file, 'r');
    Record record;
    int count = 0;
    float previous_key = INITIAL_VALUE;
    record = get_next_record(&re);
    if (is_record_empty(record))
        return count;
    else
        count++;
    float key = get_key(record);
    while (!is_record_empty(record))
    {
        if (compare_keys(key, previous_key) < 0)
            count++;
        record = get_next_record(&re);
        previous_key = key;
        key = get_key(record);
    }

    fclose(file);

    return count;
}
