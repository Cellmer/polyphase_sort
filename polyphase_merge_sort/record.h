#pragma once
#define INITIAL_VALUE -1.0f

typedef struct Record
{
    float a, b, product;
} Record;


// checks if any of record values have initial not correct value
bool is_record_empty(Record record);

// returns key for sorting records
float get_key(Record record);

// P(A/B) = P(AB)/P(B)
float conditional_probabilty(Record record);

// used every time keys are compared in sorting (there you can reverse sorting order)
int compare_keys(float key1, float key2);


