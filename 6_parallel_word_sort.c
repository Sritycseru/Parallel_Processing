#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_WORDS 500

// Structure to store word and its frequency
struct WordFrequency {
    char word[50];
    int frequency;
};

// Function to compare two words for sorting
int compareWords(const void *a, const void *b) {
    return ((struct WordFrequency *)b)->frequency - ((struct WordFrequency *)a)->frequency;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "This program requires at least two processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        FILE *file;
        file = fopen("input.txt", "r"); // Open the file for reading

        if (file == NULL) {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        char word[50];
        int totalWords = 0;

        // Read total number of words
        while (fscanf(file, "%s", word) == 1) {
            printf("Read word : %s\n",word);
            totalWords++;
        }
        printf("Total Words : %d\n",totalWords);
        fclose(file); // Close the file when done

        int wordsPerProcess = totalWords / (size - 1); 

        // Broadcast total words and words per process to all processes
        MPI_Bcast(&totalWords, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&wordsPerProcess, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Master process distributes words to each worker process
        file = fopen("input.txt", "r");
        int offset = 0;

        for (int dest = 1; dest < size; dest++) {
            int wordsToSend = (dest == size - 1) ? (totalWords - offset) : wordsPerProcess;
            MPI_Send(&wordsToSend, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

            for (int i = 0; i < wordsToSend; i++) {
                // Use strtok to extract words
                if (fscanf(file, "%s", word) == 1) {
                        if (*word==' '){
                            continue;
                        }
                        char *token = strtok(word, " ,.!?");
                        while (token != NULL) {
                            MPI_Send(token, 50, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
                            token = strtok(NULL, " ,.!?");
                        }
                    }
                }
            }
        fclose(file); // Close the file when done
    
    }

    else {

        int totalWords, wordsPerProcess;

        // Receive total words and words per process
        MPI_Bcast(&totalWords, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&wordsPerProcess, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int wordsToReceive = (rank == size - 1) ? (totalWords - (rank - 1) * wordsPerProcess) : wordsPerProcess;
        struct WordFrequency wordFreq[MAX_WORDS];
        int wordCount = 0;

        // Receive words from master process
        // Receive words from master process
        for (int i = 0; i < wordsToReceive; i++) {
            char word[50];

            MPI_Recv(word, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Check if the word is already in the array
            int found = 0;
            for (int j = 0; j < wordCount; ++j) {
                if (strcmp(wordFreq[j].word, word) == 0) {
                    wordFreq[j].frequency++;
                    found = 1;
                    break;
                }
            }

            // If the word is not in the array, add it
            if (!found) {
                strcpy(wordFreq[wordCount].word, word);
                wordFreq[wordCount].frequency = 1;
                wordCount++;
            }
        }

        // Sort the array of words based on frequency in descending order
        qsort(wordFreq, wordCount, sizeof(struct WordFrequency), compareWords);

        // Print the sorted list from each worker process
        printf("Word Frequency from Process %d:\n", rank);
        for (int i = 0; i < wordCount; ++i) {
            printf("%s - %d\n", wordFreq[i].word, wordFreq[i].frequency);
        }
    }

    MPI_Finalize();
    return 0;
}
