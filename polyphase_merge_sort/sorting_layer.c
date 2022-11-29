#define _CRT_SECURE_NO_WARNINGS
#include <math.h>

#include "sorting_layer.h"
#include "file_operations.h"
#include "disk_operations.h"


// sort file records
void sort(const char* filename)
{
    const char tape1_name[] = "tape1.bin";
    const char tape2_name[] = "tape2.bin";
    const char tape3_name[] = "tape3.bin";

    // clear tapes
    FILE* tape1;
    tape1 = fopen(tape1_name, "wb");
    fclose(tape1);
    FILE* tape2;
    tape2 = fopen(tape2_name, "wb");
    fclose(tape2);
    FILE* tape3;
    tape3 = fopen(tape3_name, "wb");
    fclose(tape3);

    Runs runs = distribute(filename, tape1_name, tape2_name);

   /* printf("RUNS: %d, %d, %d index: %d\n", runs.larger_tape_runs, runs.smaller_tape_runs, runs.dummy, runs.larger_tape_index);
    print_file(tape1_name);
    print_file(tape2_name);*/

    char* sorted_tape;
    int phase_count = 0;
    if (runs.larger_tape_index == 0)
        sorted_tape = merge(tape1_name, tape2_name, NULL, 0, tape3_name, runs, &phase_count);
    else
        sorted_tape = merge(tape2_name, tape1_name, NULL, 0, tape3_name, runs, &phase_count);

    printf("\nPHASES: %d\n", phase_count);
    //FILE* experiment_file = fopen("experiment_file.txt", "a");
    //fprintf(experiment_file, "%d\t%d\t%d\t%d\t%d\n", 0, phase_count, 0, get_save_count() + get_load_count(), RECORDS_IN_BLOCK);
    //fclose(experiment_file);

    write_file_to_file(sorted_tape, filename);
}

// initial sorting phase
Runs distribute(const char* filename, const char* tape1_name, const char* tape2_name)
{
    FILE* file = fopen(filename, "rb");
    FILE* tape1 = fopen(tape1_name, "wb");
    FILE* tape2 = fopen(tape2_name, "wb");
    RecordExtractor re_file, re_tape[2];
    record_extractor_init(&re_file, file, 'r');
    record_extractor_init(&(re_tape[0]), tape1, 'w');
    record_extractor_init(&(re_tape[1]), tape2, 'w');
    float previous_key[2] = { INITIAL_VALUE, INITIAL_VALUE };
    int tape_runs[2] = { 0, 0 };
    Record record = get_next_record(&re_file);
    // add first run to tape1
    while (!is_record_empty(record))
    {
        float key = get_key(record);
        if (compare_keys(key, previous_key[0]) >= 0)
        {
            // add another record to the previous run
            save_record(&(re_tape[0]), record);
            previous_key[0] = key;
        }
        else
        {
            tape_runs[0]++;
            break;
        }

        record = get_next_record(&re_file);
    }

    // file is sorted
    if (is_record_empty(record) && previous_key[0] != INITIAL_VALUE)
    {
        save_block(tape1, re_tape[0].block);
        Runs ret = { .larger_tape_runs = 1, .smaller_tape_runs = 0, .dummy = 0, .larger_tape_index = 0 };
        fclose(file);
        fclose(tape1);
        fclose(tape2);
        return ret;
    }
    else if (is_record_empty(record)) // file is empty
    {
        Runs ret = { .larger_tape_runs = 0, .smaller_tape_runs = 0, .dummy = 0, .larger_tape_index = 0 };
        fclose(file);
        fclose(tape1);
        fclose(tape2);
        return ret;
    }

    // add first run to tape2
    while (!is_record_empty(record))
    {
        float key = get_key(record);
        if (compare_keys(key, previous_key[1]) >= 0)
        {
            // add another record to the previous run
            save_record(&(re_tape[1]), record);
            previous_key[1] = key;
        }
        else
        {
            tape_runs[1]++;
            break;
        }

        record = get_next_record(&re_file);
    }

    // both tapes have only 1 run
    if (is_record_empty(record) && previous_key[1] != INITIAL_VALUE)
    {
        save_block(tape1, re_tape[0].block);
        save_block(tape2, re_tape[1].block);
        Runs ret = { .larger_tape_runs = 1, .smaller_tape_runs = 1, .dummy = 0, .larger_tape_index = 0 };
        fclose(file);
        fclose(tape1);
        fclose(tape2);
        return ret;
    }

    int next_runs_goal = 2;
    int current_tape = 0;
    bool first_in_run = true;
    while (!(is_record_empty(record)))
    {
        float key = get_key(record);
        if (compare_keys(key, previous_key[current_tape]) >= 0)
        {
            save_record(&(re_tape[current_tape]), record);
            previous_key[current_tape] = key;
        }
        else
        {
            if (first_in_run)
            {
                save_record(&(re_tape[current_tape]), record);
                previous_key[current_tape] = key;
                tape_runs[current_tape]++;
            }
            else
            {
                if (tape_runs[current_tape] == next_runs_goal)
                {
                    // change current tape
                    current_tape = 1 - current_tape;
                    next_runs_goal = tape_runs[0] + tape_runs[1];
                    first_in_run = true;
                    continue;
                }
                else
                {
                    save_record(&(re_tape[current_tape]), record);
                    previous_key[current_tape] = key;
                    tape_runs[current_tape]++;
                }
            }
        }

        if (first_in_run)
            first_in_run = false;
        record = get_next_record(&re_file);
    }

    save_block(tape1, re_tape[0].block);
    save_block(tape2, re_tape[1].block);

    Runs ret = { .larger_tape_runs = tape_runs[current_tape] , .smaller_tape_runs = tape_runs[1 - current_tape], .dummy = next_runs_goal - tape_runs[current_tape], .larger_tape_index = current_tape };
    fclose(file);
    fclose(tape1);
    fclose(tape2);
    return ret;
}


// main sorting recursive function, returns name of the tape with sorted file
char* merge(const char* tape1_name, const char* tape2_name, fpos_t pos, int record_index, const char* tape3_name, Runs runs, int* phase)
{
    //printf("\nPHASE %d:\n", *phase);
    //printf("\nMERGE\nRUNS: %d, %d, %d\n", runs.larger_tape_runs, runs.smaller_tape_runs, runs.dummy);
    //print_file(tape1_name);
    //print_file(tape2_name);
    //print_file(tape3_name);
    if (runs.smaller_tape_runs == 0)
        return tape1_name;
    (*phase)++;
    FILE* tape1 = fopen(tape1_name, "rb");
    FILE* tape2 = fopen(tape2_name, "rb");
    FILE* tape3 = fopen(tape3_name, "wb");

    // set tape2 file pointer position to the one from the last call if it is not a first call in recursive tree
    if (pos != NULL)
        fsetpos(tape2, &pos);

    RecordExtractor re_tape1, re_tape2, re_tape3;
    record_extractor_init(&re_tape1, tape1, 'r');
    record_extractor_init(&re_tape2, tape2, 'r');
    record_extractor_init(&re_tape3, tape3, 'w');
    // set record index inside of the block linked with file extractor to the position from the last call
    re_tape2.record_index = record_index;

    float previous_key[2] = { INITIAL_VALUE, INITIAL_VALUE };
    Record record[2];
    record[1] = get_next_record(&re_tape2);
    float key[2];
    key[1] = get_key(record[1]);
    // dummy runs (only from tape2)
    for (int i = 0; i < runs.dummy; i++)
    {
        previous_key[1] = INITIAL_VALUE;
        while (compare_keys(key[1], previous_key[1]) >= 0)
        {
            save_record(&re_tape3, record[1]);
            previous_key[1] = key[1];
            record[1] = get_next_record(&re_tape2);
            key[1] = get_key(record[1]);
        }
    }

    // normal runs
    record[0] = get_next_record(&re_tape1);
    key[0] = get_key(record[0]);
    for (int i = 0; i < runs.smaller_tape_runs - runs.dummy; i++)
    {
        previous_key[0] = INITIAL_VALUE;
        previous_key[1] = INITIAL_VALUE;
        bool run_finished[2] = { false, false };
        while (!(run_finished[0] && run_finished[1]))
        {
            if (run_finished[0] || (!run_finished[0] && !run_finished[1] && compare_keys(key[1], key[0]) < 0))
            {
                save_record(&re_tape3, record[1]);
                previous_key[1] = key[1];
                record[1] = get_next_record(&re_tape2);
                key[1] = get_key(record[1]);
                if (is_record_empty(record[1]) || compare_keys(key[1], previous_key[1]) < 0)
                    run_finished[1] = true;
            }
            else if (run_finished[1] || (!run_finished[0] && !run_finished[1] && compare_keys(key[0], key[1]) <= 0))
            {
                save_record(&re_tape3, record[0]);
                previous_key[0] = key[0];
                record[0] = get_next_record(&re_tape1);
                key[0] = get_key(record[0]);
                if (is_record_empty(record[0]) || compare_keys(key[0], previous_key[0]) < 0)
                    run_finished[0] = true;
            }
        }
    }

    save_block(tape3, re_tape3.block);

    int next_larger_runs, next_smaller_runs;
    if (runs.larger_tape_runs == 1 && runs.smaller_tape_runs == 1)
    {
        next_larger_runs = 0;
        next_smaller_runs = 0;
    }
    else
    {
        next_larger_runs = runs.smaller_tape_runs;
        next_smaller_runs = runs.larger_tape_runs + runs.dummy - runs.smaller_tape_runs;
    }
    runs.larger_tape_runs = next_larger_runs;
    runs.smaller_tape_runs = next_smaller_runs;
    runs.dummy = 0;

    // values to set stream position in tape2
    int index = re_tape1.record_index - 1;
    // rewind stream position by one block
    fseek(tape1, -(int)sizeof(Block), SEEK_CUR);
    fpos_t position;
    fgetpos(tape1, &position);

    fclose(tape1);
    fclose(tape2);
    fclose(tape3);

    merge(tape3_name, tape1_name, position, index, tape2_name, runs, phase);
}
