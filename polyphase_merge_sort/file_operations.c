#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "disk_operations.h"
#include "file_operations.h"
#include "sorting_layer.h"


// generate file with specified number of random records
void generate_random_file(const char* filename, int number_of_records)
{
    FILE* file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Could not open a file: %s!\n", filename);
        return;
    }

    RecordExtractor re;
    record_extractor_init(&re, file, 'w');

    srand(time(NULL));
    Record record;
    for (int i = 0; i < number_of_records; i++)
    {
        record.a = (float)rand() / RAND_MAX;
        record.b = (float)rand() / RAND_MAX;
        record.product = ((float)rand() / RAND_MAX) * min(record.a, record.b);
        save_record(&re, record);
    }

    // flush last block to file
    save_block(file, re.block);
    fclose(file);
}

// print file (one record in a row)
void print_file(const char* filename)
{
    FILE* file;
    file = fopen(filename, "rb");

    printf("%s:\n", filename);
    printf("P(A)\tP(B)\tP(AB)\tP(A/B)\n");
    RecordExtractor re;
    record_extractor_init(&re, file, 'r');
    Record record;
    record = get_next_record(&re);
    while (!is_record_empty(record))
    {
        printf("%.2f\t%.2f\t%.2f\t%.2f\n", record.a, record.b, record.product, get_key(record));
        record = get_next_record(&re);
    }

    fclose(file);
}

// ask user to enter records and create a file out of them
void generate_file_from_user_input(const char* filename)
{
    printf("How many records do you want to enter?\n");
    int number_of_records;
    scanf("%d", &number_of_records);

    FILE* file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Could not open a file: %s!\n", filename);
        return;
    }

    RecordExtractor re;
    record_extractor_init(&re, file, 'w');

    Record record;
    for (int i = 0; i < number_of_records; i++)
    {
        printf("Record%d:\n", i);
        printf("P(A): ");
        scanf("%f", &(record.a));
        printf("P(B): ");
        scanf("%f", &(record.b));
        printf("P(AB): ");
        scanf("%f", &(record.product));
        if (record.a > 1.0f || record.a < 0.0f || record.b > 1.0f || record.b < 0.0f || record.product > 1.0f || record.product < 0.0f)
        {
            printf("INVALID DATA: probabilty must be in range <0, 1>\n");
            i--;
            continue;
        }
        else if (record.product > min(record.a, record.b))
        {
            printf("INVALID DATA: Probabilty of product cannot be greater than probabilty of smaller element of this product!\n");
            i--;
            continue;
        }
        save_record(&re, record);
    }

    // flush last block to file
    save_block(file, re.block);

    fclose(file);
}

// write whole filename1 content to filename2
void write_file_to_file(const char* filename1, const char* filename2)
{
    FILE* file1;
    file1 = fopen(filename1, "rb");
    if (file1 == NULL)
    {
        printf("Could not open a file: %s!\n", filename1);
        return;
    }
    FILE* file2;
    file2 = fopen(filename2, "wb");
    if (file2 == NULL)
    {
        printf("Could not open a file: %s!\n", filename2);
        return;
    }

    RecordExtractor re_file1, re_file2;
    record_extractor_init(&re_file1, file1, 'r');
    record_extractor_init(&re_file2, file2, 'w');
    Record record;
    record = get_next_record(&re_file1);
    while (!is_record_empty(record))
    {
        save_record(&re_file2, record);
        record = get_next_record(&re_file1);
    }

    save_block(file2, re_file2.block);
    fclose(file1);
    fclose(file2);
}
