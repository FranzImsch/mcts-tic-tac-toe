#include <iostream>
#include <string>
#include <vector>

int field[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // testing

using namespace std;

void printfield()
{
    // clear terminal on Windows or a proper OS
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    // cout << " ___________\n";
    for (int i = 0; i < 3; i++) // iterate through rows
    {
        cout << "   |   |   \n ";
        for (int j = 0; j < 3; j++) // iterate through columns
        {
            cout << field[i * 3 + j]; // print field content

            if (j != 2) // print pipe between contents
                cout << " | ";
        }
        if (i != 2) // print lines between rows
            cout << "\n___|___|___";
            else
            cout << "\n   |   |   ";

        cout << endl;
    }
    cout << endl;
    cout << endl;
}

int main()
{

    return 0;
}