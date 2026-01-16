/*
 * TurboLog - High-Performance CLI Log Analyzer
 * Author: Mohamed Anis ATHMANE
 * Description: A C-based tool to parse and filter massive server logs (GBs) in seconds.
 * Uses direct memory addressing (O(1)) and stream processing for minimal RAM usage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 64KB Buffer to handle very long log lines safely
#define MAX_LINE 65536

// HTTP Status codes range from 100 to 599. We size the array to 600 for direct indexing.
#define STATUS_RANGE 600

/**
 * Robustly extracts the HTTP status code from a log line.
 * Strategy: "Anchor Parsing"
 * Instead of searching the whole string, we locate the request quotes.
 * The status code is guaranteed to be the first integer after the closing quote.
 *
 * @param line The log line to parse
 * @return int The HTTP status code (e.g., 200, 404) or 0 if malformed.
 */
int status_finder(char line[]){
    char* first_quote = strchr(line, '"');
    
    // Safety Check: Ensure the log line actually contains a request
    if(first_quote != NULL){
        // Find the closing quote of the HTTP request
        char* second_quote = strchr(first_quote + 1, '"');
        // +1 skips the quote itself. atoi ignores leading spaces automatically.
        if(second_quote != NULL)
            return atoi(second_quote + 1);
    }
    return 0; // Return 0 for malformed/empty lines
}

int main(int argc, char *argv[]){
    // Basic Argument Validation
    if (argc < 2) {
        printf("Usage: %s <file path> [-s status_code]\n", argv[0]);
        return 1;
    }

    // Open File Stream
    char *file_path = argv[1];
    FILE *log_file = fopen(file_path, "r");

    if(log_file == NULL){
        printf("The file %s has not been found", file_path);
        return 1;
    }

    // Argument Parsing & Boolean Mask Setup
    // filter_mask[i] = 1 means the user wants to see status code 'i'.
    // This allows O(1) lookup later, avoiding slow list searches.
    int filter_mask[600] = {0};

    // Check if Filter Mode is active (-s flag)
    if(argc >= 3) {
        // Enforce strict flag positioning for this version
        if(strcmp(argv[2], "-s") == 0){
            if(argc == 3){
                printf("-s requires at least one status code after it\n");
                fclose(log_file);
                return 1;
            }

            // Loop through all remaining arguments to capture multiple codes
            for(int i = 3; i < argc; i++){
                int filtered_status = atoi(argv[i]);

                // Validate HTTP range
                if(filtered_status < 100 || filtered_status >= 600){
                    printf("Status code %d does not exist.\nPlease give numbers between 100 and 599 \n", filtered_status);
                    fclose(log_file);
                    return 1;
                }

                // Activate the mask for this specific code
                filter_mask[filtered_status] = 1;
            }
        }
    }

    char buffer[MAX_LINE];
    // "Bucket" Array to count occurrences. status_counts[200] holds the count for 200.
    int status_counts[600] = {0};

    // We read line-by-line to keep memory usage constant, regardless of file size.
    while (fgets(buffer, sizeof(buffer), log_file)) {
        int status = status_finder(buffer);

        // Only count valid HTTP codes to avoid array out-of-bounds
        if(status >= 100 && status < 600)
            status_counts[status]++;
    }

    // 5. Output Results
    for(int i = 100; i< 600; i++){
        // If Filter Mode was active (argc > 3), only show masked codes
        if(argc > 3){
            if(filter_mask[i] == 1)
                printf("Status code %d : %d occurences\n", i, status_counts[i]);
        }
        // Summary Mode: Show all non-zero counts
        else{
            if(status_counts[i] > 0)
                printf("Status code %d : %d occurences\n", i, status_counts[i]);
        }
    }
    fclose(log_file);
    return 0;
}