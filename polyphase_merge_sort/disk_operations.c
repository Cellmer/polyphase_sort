#include <stdio.h>

#include "disk_operations.h"
#include "record.h"

int save_count = 0;
int load_count = 0;

int get_save_count()
{
    return save_count;
}

int get_load_count()
{
    return load_count;
}

void clear_save_count()
{
    save_count = 0;
}

void clear_load_count()
{
    load_count = 0;
}


// returns block of data from current position in file
void get_block(FILE* file, Block* block)
{
    load_count++;
    clear_block(block);
    fread(block, sizeof(Block), 1, file);
}

// saves block after current position in file
void save_block(FILE* file, Block block)
{
    save_count++;
    fwrite(&block, sizeof(Block), 1, file);
}

// set records vaules to default
void clear_block(Block* block)
{
    for (int i = 0; i < RECORDS_IN_BLOCK; i++)
    {
        (*block).records[i].a = INITIAL_VALUE;
        (*block).records[i].b = INITIAL_VALUE;
        (*block).records[i].product = INITIAL_VALUE;
    }
}

// initiates record extractor and assignes file to it
void record_extractor_init(RecordExtractor* re, FILE* file, char mode)
{
    re->file = file;
    re->record_index = 0;
    if (mode == 'r')
        get_block(file, &(re->block));
    else
        clear_block(&(re->block));
}

// get next record using block operations
Record get_next_record(RecordExtractor* re)
{
    // all of the records of the current block were used
    if (re->record_index == RECORDS_IN_BLOCK)
    {
        re->record_index = 0;
        get_block(re->file, &(re->block));
    }

    return re->block.records[re->record_index++];
}

// save record using block operations
void save_record(RecordExtractor* re, Record record)
{
    // all of the records of the current block were filled
    if (re->record_index == RECORDS_IN_BLOCK)
    {
        re->record_index = 0;
        save_block(re->file, re->block);
        clear_block(&(re->block));
    }

    re->block.records[re->record_index] = record;
    re->record_index++;
}
