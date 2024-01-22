#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        cerr << "This program requires at least two processes.\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error opening file.\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string word;
    map<string, int> wordFrequency;

    // Read words from the file
    while (inputFile >> word) {
        // Remove any punctuation or special characters from the word
        for (char &c : word) {
            if (!isalnum(c)) {
                c = '\0';
            }
        }

        // Skip empty words
        if (!word.empty()) {
            // Increment the frequency of the word in the map
            wordFrequency[word]++;
        }
    }

    inputFile.close();  // Close the file when done

    // Gather word frequencies from all processes
    vector<pair<string, int>> localWordVector(wordFrequency.begin(), wordFrequency.end());
    vector<pair<string, int>> globalWordVector;

    MPI_Allgather(
        localWordVector.data(), localWordVector.size() * sizeof(pair<string, int>),
        MPI_BYTE, globalWordVector.data(), localWordVector.size() * sizeof(pair<string, int>),
        MPI_BYTE, MPI_COMM_WORLD);

    // Combine word frequencies from all processes
    map<string, int> combinedWordFrequency;
    for (const auto &pair : globalWordVector) {
        combinedWordFrequency[pair.first] += pair.second;
    }

    // Convert the map to a vector of pairs for sorting
    vector<pair<string, int>> combinedWordVector(combinedWordFrequency.begin(), combinedWordFrequency.end());

    // Sort the vector based on frequency in descending order
    sort(combinedWordVector.begin(), combinedWordVector.end(), [](const auto &a, const auto &b) {
        return b.second < a.second;
    });

    // Print the sorted list from each process
    cout << "Word Frequency from Process " << rank << ":\n";
    for (const auto &pair : combinedWordVector) {
        cout << pair.first << " - " << pair.second << "\n";
    }

    MPI_Finalize();
    return 0;
}
