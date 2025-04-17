#include <iostream>
#include <string>
#include <vector>

int field[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // testing

using namespace std;

class board
{
    int board_state[9]; // 0 = leer, 1 = X, -1=O
    int turn;           // Anzahl Züge bis jetzt
    bool next_player;   // 0 = O, 1 = X

public:
    board(int turn_nr, int currentstate[9], bool last_played) // Konstruktor, erzeugt ein neues Brett mit dem Status der in
    {                                                         // currentstate übergeben wird, dem aktuellen Zug und welcher Spieler
        for (int jj = 0; jj < 9; jj++)                        // als letztes gezogen hat
        {
            this->board_state[jj] = currentstate[jj];
        }
        this->turn = turn_nr;
        this->next_player = !last_played;
    }
    board()                            // Überladener Konstruktor, erzeugt ein leeres Feld
    {                                  // wenn keine Parameter übergeben werden.
        for (int jj = 0; jj < 9; jj++) //
        {                              //
            this->board_state[jj] = 0; //
        } //
        this->turn = 0;        //
        this->next_player = 1; // Standard: X fängt an
    }
    int get_cell_state(int cell)        // Gibt den Status einer bestimmten Zelle zurück,
    {                                   // mit 0=leeres Feld,1 = X in diesem Feld,
        return this->board_state[cell]; //-1 = O in diesem Feld
    }
    int get_turn() // Gibt den aktuellen Zug zurück (Standard getter-funktion)
    {
        return this->turn;
    }
    int get_next_player() // Gibt zurück, welcher Spieler als nächstes Spielen darf
    {
        return this->next_player;
    }
    int set_cell_state(int cell) // Setzt den Status einer Zelle in Abhängigkeit davon, wer
    {                            // als nächster Spieler eingetragen ist. Gibt -1 zurück wenn das Feld belegt ist
        if (cell > 8)            // und -2 wenn das Feld nicht existiert (Zahl zu groß/zu klein)
        {                        //
            return -2;           //
        }
        if (board_state[cell] != 0) // Überprüft ob das gewählte Feld leer ist
        {                           //
            return -1;              //
        }
        if (next_player)               // Wenn der nächste Spieler X ist wird der Status von der gewählten Zelle
        {                              // um 1 erhöht, das entspricht dann einem platzierten X in dieser Zelle
            this->board_state[cell]++; //
        }
        else                           //
        {                              // Andernfalls wird der Status um 1 reduziert, das entspricht
            this->board_state[cell]--; // einem platzierten O in der Zelle
        }
        this->turn++; // Der Zug wird automatisch erhöht
        return 0;     // Kein Fehler-code
    }
};

struct board_node
{
    board *this_board = new board();
    int games_played = 0;
    int games_won_by_player = 0;
    vector<board *> next_boards;
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