#include <iostream>
#include <string>
#include <vector>

int field[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // testing

using namespace std;

class board
{
    int board_state[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0 = leer, 1 = X, -1=O
    int turn;                                         // Anzahl Züge bis jetzt
    bool next_player;                                 // 0 = O, 1 = X
    board(int turn_nr, int currentstate[9], bool last_played)
    {
        for (int jj = 0; jj < 9; jj++)
        {
            this->board_state[jj] = currentstate[jj];
        }
        this->turn = turn_nr;
        this->next_player = !last_played;
    }

public:
    int get_cell_state(int cell)
    {
        return this->board_state[cell];
    }
    int get_turn()
    {
        return this->turn;
    }
    int get_next_player()
    {
        return this->next_player;
    }
    int set_cell_state(int cell)
    {
        if (board_state[cell] != 0)
        {
            return -1;
        }
        if (next_player)
        {
            this->board_state[cell]++;
        }
        else
        {
            this->board_state[cell]--;
        }
        return 0;
    }
};

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
            if (field[i * 3 + j] == 0)
            {
                cout << " ";
            }
            else if (field[i * 3 + j] == 1)
            {
                cout << "O";
            }
            else
                cout << "X";

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
    printfield();
    return 0;
}