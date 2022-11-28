#include <stdbool.h>

#include "record.h"


// checks if any of record values have initial not correct value
bool is_record_empty(Record record)
{
    if (record.a == INITIAL_VALUE || record.b == INITIAL_VALUE || record.product == INITIAL_VALUE)
        return true;
    return false;
}

// returns key for sorting records
float get_key(Record record)
{
    return conditional_probabilty(record);
}

// P(A/B) = P(AB)/P(B)
float conditional_probabilty(Record record)
{
    if (record.b == 0.0f)
        return 0.0f;
    return record.product / record.b;
}

int compare_keys(float key1, float key2)
{
    if (key1 < key2)
        return -1;
    if (key1 == key2)
        return 0;
    return 1;
}
