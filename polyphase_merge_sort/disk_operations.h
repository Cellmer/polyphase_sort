#pragma once
#include <stdbool.h>

#include "record.h"

#define RECORDS_IN_BLOCK 32


////////// structures //////////////////
typedef struct Block
{
    Record records[RECORDS_IN_BLOCK];
} Block;

// structure that user can use to get and save records
typedef struct RecordExtractor
{
    FILE* file;
    Block block;
    int record_index;
} RecordExtractor;


////////// functions //////////////////
// returns block of data from current position in file
void get_block(FILE* file, Block* block);

// saves block after current position in file
void save_block(FILE* file, Block block);

// set records vaules to default
void clear_block(Block* block);

// initiates record extractor and assignes file to it
void record_extractor_init(RecordExtractor* re, FILE* file, char mode);

// get next record using block operations
Record get_next_record(RecordExtractor* re);

// save record using block operations
void save_record(RecordExtractor* re, Record record);

int get_save_count();
int get_load_count();
void clear_save_count();
void clear_load_count();
