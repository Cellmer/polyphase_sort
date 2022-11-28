#pragma once

// generate file with specified number of random records
void generate_random_file(const char* filename, int number_of_records);

// print file (one record in a row)
void print_file(const char* filename);

// ask user to enter records and create a file out of them
void generate_file_from_user_input(const char* filename);

// write whole filename1 content to filename2
void write_file_to_file(const char* filename1, const char* filename2);
