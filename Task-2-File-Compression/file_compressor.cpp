#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>

using namespace std;

// ---------------------------------------------------------
// Read a binary file into a vector
// ---------------------------------------------------------
vector<unsigned char> readFile(const string& filename)
{
    ifstream file(filename, ios::binary);

    if (!file)
    {
        cout << "Error: Could not open file.\n";
        return {};
    }

    file.seekg(0, ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, ios::beg);

    vector<unsigned char> data(fileSize);

    if (fileSize > 0)
    {
        file.read(reinterpret_cast<char*>(data.data()), fileSize);
    }

    file.close();

    return data;
}

// ---------------------------------------------------------
// Write binary data to a file
// ---------------------------------------------------------
bool writeFile(const string& filename,
               const vector<unsigned char>& data)
{
    ofstream file(filename, ios::binary);

    if (!file)
    {
        cout << "Error: Could not create output file.\n";
        return false;
    }

    if (!data.empty())
    {
        file.write(reinterpret_cast<const char*>(data.data()),
                   data.size());
    }

    file.close();

    return true;
}

// ---------------------------------------------------------
// RLE compression of one chunk
// ---------------------------------------------------------
vector<unsigned char> compressChunk(
    const vector<unsigned char>& input)
{
    vector<unsigned char> output;

    size_t i = 0;

    while (i < input.size())
    {
        unsigned char value = input[i];
        unsigned char count = 1;

        while (i + count < input.size() &&
               input[i + count] == value &&
               count < 255)
        {
            count++;
        }

        // Store: count + value
        output.push_back(count);
        output.push_back(value);

        i += count;
    }

    return output;
}

// ---------------------------------------------------------
// RLE decompression of one chunk
// ---------------------------------------------------------
vector<unsigned char> decompressChunk(
    const vector<unsigned char>& input)
{
    vector<unsigned char> output;

    for (size_t i = 0; i + 1 < input.size(); i += 2)
    {
        unsigned char count = input[i];
        unsigned char value = input[i + 1];

        for (int j = 0; j < count; j++)
        {
            output.push_back(value);
        }
    }

    return output;
}

// ---------------------------------------------------------
// MULTITHREADED COMPRESSION
// ---------------------------------------------------------
bool compressFile(const string& inputFile,
                  const string& outputFile)
{
    vector<unsigned char> data = readFile(inputFile);

    if (data.empty())
    {
        cout << "Error: Input file is empty or could not be read.\n";
        return false;
    }

    const size_t threadCount = 4;

    size_t chunkSize =
        (data.size() + threadCount - 1) / threadCount;

    vector<vector<unsigned char>> chunks;
    vector<vector<unsigned char>> results;
    vector<thread> threads;

    // Split the input file into chunks
    for (size_t start = 0;
         start < data.size();
         start += chunkSize)
    {
        size_t end =
            min(start + chunkSize, data.size());

        chunks.emplace_back(
            data.begin() + start,
            data.begin() + end
        );
    }

    results.resize(chunks.size());

    auto startTime =
        chrono::high_resolution_clock::now();

    // Create multiple threads
    for (size_t i = 0; i < chunks.size(); i++)
    {
        threads.emplace_back(
            [&chunks, &results, i]()
            {
                results[i] =
                    compressChunk(chunks[i]);
            }
        );
    }

    // Wait for every thread
    for (auto& t : threads)
    {
        t.join();
    }

    // Combine compressed chunks
    vector<unsigned char> compressedData;

    for (const auto& result : results)
    {
        compressedData.insert(
            compressedData.end(),
            result.begin(),
            result.end()
        );
    }

    auto endTime =
        chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duration =
        endTime - startTime;

    if (!writeFile(outputFile, compressedData))
    {
        return false;
    }

    cout << "\n========================================\n";
    cout << "       COMPRESSION COMPLETED\n";
    cout << "========================================\n";

    cout << "Original size   : "
         << data.size() << " bytes\n";

    cout << "Compressed size : "
         << compressedData.size() << " bytes\n";

    double percentage =
        100.0 -
        (static_cast<double>(compressedData.size()) /
         data.size() * 100.0);

    cout << "Space saved     : "
         << percentage << "%\n";

    cout << "Compression time : "
         << duration.count() << " ms\n";

    cout << "Threads used     : "
         << threads.size() << "\n";

    cout << "Output file      : "
         << outputFile << "\n";

    return true;
}

// ---------------------------------------------------------
// MULTITHREADED DECOMPRESSION
// ---------------------------------------------------------
bool decompressFile(const string& inputFile,
                    const string& outputFile)
{
    vector<unsigned char> data =
        readFile(inputFile);

    if (data.empty())
    {
        cout << "Error: Compressed file is empty or "
                "could not be read.\n";
        return false;
    }

    // RLE data must contain count/value pairs.
    if (data.size() % 2 != 0)
    {
        cout << "Error: Invalid compressed file format.\n";
        return false;
    }

    const size_t threadCount = 4;

    // Number of count/value pairs
    size_t pairCount = data.size() / 2;

    size_t pairsPerThread =
        (pairCount + threadCount - 1) / threadCount;

    vector<vector<unsigned char>> chunks;
    vector<vector<unsigned char>> results;
    vector<thread> threads;

    // Split compressed data only at pair boundaries
    for (size_t startPair = 0;
         startPair < pairCount;
         startPair += pairsPerThread)
    {
        size_t endPair =
            min(startPair + pairsPerThread,
                pairCount);

        size_t startIndex =
            startPair * 2;

        size_t endIndex =
            endPair * 2;

        chunks.emplace_back(
            data.begin() + startIndex,
            data.begin() + endIndex
        );
    }

    results.resize(chunks.size());

    auto startTime =
        chrono::high_resolution_clock::now();

    // Create multiple decompression threads
    for (size_t i = 0; i < chunks.size(); i++)
    {
        threads.emplace_back(
            [&chunks, &results, i]()
            {
                results[i] =
                    decompressChunk(chunks[i]);
            }
        );
    }

    // Wait for all threads
    for (auto& t : threads)
    {
        t.join();
    }

    // Combine results in original order
    vector<unsigned char> decompressedData;

    for (const auto& result : results)
    {
        decompressedData.insert(
            decompressedData.end(),
            result.begin(),
            result.end()
        );
    }

    auto endTime =
        chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duration =
        endTime - startTime;

    if (!writeFile(outputFile, decompressedData))
    {
        return false;
    }

    cout << "\n========================================\n";
    cout << "      DECOMPRESSION COMPLETED\n";
    cout << "========================================\n";

    cout << "Compressed size   : "
         << data.size() << " bytes\n";

    cout << "Decompressed size : "
         << decompressedData.size() << " bytes\n";

    cout << "Decompression time: "
         << duration.count() << " ms\n";

    cout << "Threads used      : "
         << threads.size() << "\n";

    cout << "Output file       : "
         << outputFile << "\n";

    return true;
}

// ---------------------------------------------------------
// Main menu
// ---------------------------------------------------------
int main()
{
    int choice = 0;

    cout << "========================================\n";
    cout << "   MULTITHREADED FILE COMPRESSION TOOL\n";
    cout << "========================================\n";

    while (true)
    {
        cout << "\n1. Compress File\n";
        cout << "2. Decompress File\n";
        cout << "3. Exit\n";
        cout << "\nEnter your choice: ";

        // Safely read the user's choice
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "Invalid input!\n";
            cout << "Please enter 1, 2, or 3.\n";

            continue;
        }

        // -------------------------------
        // Compression
        // -------------------------------
        if (choice == 1)
        {
            string inputFile;
            string outputFile;

            cout << "\nEnter input filename: ";
            cin >> inputFile;

            cout << "Enter output filename: ";
            cin >> outputFile;

            compressFile(inputFile, outputFile);
        }

        // -------------------------------
        // Decompression
        // -------------------------------
        else if (choice == 2)
        {
            string inputFile;
            string outputFile;

            cout << "\nEnter compressed filename: ";
            cin >> inputFile;

            cout << "Enter output filename: ";
            cin >> outputFile;

            decompressFile(inputFile, outputFile);
        }

        // -------------------------------
        // Exit
        // -------------------------------
        else if (choice == 3)
        {
            cout << "\nExiting program...\n";
            break;
        }

        // -------------------------------
        // Invalid number
        // -------------------------------
        else
        {
            cout << "\nInvalid choice!\n";
            cout << "Please enter 1, 2, or 3.\n";
        }
    }

    return 0;
}3