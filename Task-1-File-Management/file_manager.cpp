#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Function to write data to a file
void writeToFile(const string& filename)
{
    ofstream file(filename);

    if (!file)
    {
        cout << "Error opening file for writing.\n";
        return;
    }

    string data;

    cout << "Enter data to write to the file: ";
    getline(cin, data);

    file << data << endl;

    file.close();

    cout << "Data written successfully!\n";
}

// Function to append data to a file
void appendToFile(const string& filename)
{
    ofstream file(filename, ios::app);

    if (!file)
    {
        cout << "Error opening file for appending.\n";
        return;
    }

    string data;

    cout << "Enter data to append to the file: ";
    getline(cin, data);

    file << data << endl;

    file.close();

    cout << "Data appended successfully!\n";
}

// Function to read data from a file
void readFromFile(const string& filename)
{
    ifstream file(filename);

    if (!file)
    {
        cout << "Error opening file for reading.\n";
        return;
    }

    string line;

    cout << "\n--- File Contents ---\n";

    while (getline(file, line))
    {
        cout << line << endl;
    }

    cout << "---------------------\n";

    file.close();
}

int main()
{
    const string filename = "data.txt";

    int choice;

    cout << "=================================\n";
    cout << "      C++ FILE MANAGEMENT TOOL   \n";
    cout << "=================================\n";

    do
    {
        cout << "\n1. Write to File";
        cout << "\n2. Append to File";
        cout << "\n3. Read File";
        cout << "\n4. Exit";
        cout << "\n\nEnter your choice: ";

        cin >> choice;
        cin.ignore();

        switch (choice)
        {
            case 1:
                writeToFile(filename);
                break;

            case 2:
                appendToFile(filename);
                break;

            case 3:
                readFromFile(filename);
                break;

            case 4:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 4);

    return 0;
}