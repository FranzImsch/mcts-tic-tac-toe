#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#ifdef _WIN32
#include <random>

int rndm(int upper_bound) // https://stackoverflow.com/questions/288739/generate-random-numbers-uniformly-over-an-entire-range
{
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_int_distribution<int> distr(0, upper_bound);

    return distr(generator);
}

#else
#define rndm(upper_bound) arc4random_uniform(upper_bound) // macOS
#endif

enum class Player
{
    NONE = 0,
    X = 1,
    O = 2
};

class board
{
    vector<int> board_state; // 0 = leer, 1 = X, -1=O
    int turn;                // Anzahl Züge bis jetzt
    bool next_player;        // 0 = O, 1 = X (Player in diesem Spiel)

public:
    board(int turn_nr, vector<int> currentstate, bool last_played) // Konstruktor, erzeugt ein neues Brett mit dem Status der in
    {                                                              // currentstate übergeben wird, dem aktuellen Zug und welcher Spieler
        for (int jj = 0; jj < 9; jj++)                             // als letztes gezogen hat
        {
            this->board_state.push_back(currentstate[jj]);
        }
        this->turn = turn_nr;
        this->next_player = !last_played;
    }
    board()                                 // Überladener Konstruktor, erzeugt ein leeres Feld
    {                                       // wenn keine Parameter übergeben werden.
        for (int jj = 0; jj < 9; jj++)      //
        {                                   //
            this->board_state.push_back(0); //
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
        if (this->next_player) // X
            return int(Player::X);
        else if (!(this->next_player))
            return int(Player::O);
        else
            return int(Player::NONE);
    }

    vector<int> get_possible_moves()
    {
        vector<int> possible_moves;

        // if (is_terminal())         // Spiel vorbei
        // return possible_moves; // Übergebe leere Liste

        for (int i = 0; i < 9; i++)
            if (this->board_state[i] == int(Player::NONE)) // Moves dort möglich, wo niemand steht
                possible_moves.push_back(i);

        return possible_moves;
    }

    board make_move(int cell)
    {
        if (cell > 8 || cell < 0 || board_state[cell] != int(Player::NONE))
            cout << "error making move" << endl;

        vector<int> new_board_state = this->board_state;

        int next_move = this->next_player ? int(Player::X) : int(Player::O); // next_player wird in int umgewandelt
        new_board_state[cell] = next_move;                                   // Array updaten mit neuem Move

        board new_board(turn + 1, new_board_state, next_player); // Neues Board mit neuem Zustand und anderem Spieler

        return new_board;
    }

    bool is_draw()
    {
        if (check_win(int(Player::X)) || check_win(int(Player::O))) // wenn Gewinner, dann kein Unentschieden
            return false;

        if (this->turn >= 9) // wenn kein Gewinner, aber 9. Zug
            return true;

        return false; // sonst (noch) nicht untentschieden
    }

    bool check_win(int player)
    {
        if (this->turn < 5)
        {
            return false;
        }
        for (int i = 0; i < 9; i += 3) // Reihen
            if (this->board_state[i] == player &&
                this->board_state[i + 1] == player &&
                this->board_state[i + 2] == player)
                return true;

        for (int i = 0; i < 3; i += 1) // Spalten
            if (this->board_state[i] == player &&
                this->board_state[i + 3] == player &&
                this->board_state[i + 6] == player)
                return true;

        if (this->board_state[0] == player &&
            this->board_state[4] == player &&
            this->board_state[8] == player) // Diagonalen
            return true;
        if (this->board_state[2] == player &&
            this->board_state[4] == player &&
            this->board_state[6] == player)
            return true;

        return false; // andernfalls ist player kein Gewinner
    }

    bool is_terminal() // spiel ist todgeweiht, wenn
    {
        if (check_win(int(Player::X)) || check_win(int(Player::O)) || is_draw()) // Gewinner ∃ oder unentschieden ist
            return true;

        return false; // sonst nicht
    }

    int get_winner()
    {
        // Gehe davon aus, dass nie Player X und O gleichzeitig gewinnen können
        if (check_win(int(Player::X)))
            return int(Player::X);
        else if (check_win(int(Player::O)))
            return int(Player::O);
        else
            return int(Player::NONE);
    }
};

struct board_node
{
    board this_board;
    board_node *parent = nullptr;  // Elternknoten; nullptr für Anker
    vector<board_node *> children; // Kinderknoten

    int parent_move = 0; // Wie wurde der Knoten erzeugt?

    int games_played = 0; //
    double game_wins = 0; // Gewinne des Spielers aus Sicht des Elternknotens

    vector<int> untried_moves; // noch nicht simulierte Moves

    board_node(board current_board, board_node *parent_node, int move = -1)
    {
        this->this_board = current_board;
        this->parent = parent_node;
        this->parent_move = move;
        this->games_played = 0;                                // bei Erstellung 0
        this->game_wins = 0.;                                  // auch 0
        this->untried_moves = this_board.get_possible_moves(); // alle offen
    }

    ~board_node() // Destruktor gegen Memory Leaks von den Kindern, die unten erstellt werden ^^'
    {
        for (int i = 0; i < children.size(); i++)
            delete children[i];
    }
};

const double C = sqrt(2);

board_node *select_best_child(board_node *node)
{
    double best_score = -INFINITY; // hoffentlich klein genug…

    board_node *best_child = nullptr;

    for (int i = 0; i < node->children.size(); i++)
    {
        board_node *child = node->children[i];
        double ucb;
        if (child->games_played == 0) // Div/0 vorbeugen
        {
            ucb = INFINITY;
        }
        else
        {
            ucb = (child->game_wins / double(child->games_played)) + C * sqrt(log(double(node->games_played)) / double(child->games_played)); // UCB des Kindes ausrechnen nach Formel aus VL
        }
        if (ucb > best_score) // wähle das beste Kind aus
        {
            best_score = ucb;
            best_child = child;
        }
    }
    return best_child;
}

board_node *expand(board_node *node)
{
    if (node->untried_moves.size() == 0) // wenn es keine möglichen Versuche mehr gibt,
        return nullptr;                  // dann ists auch egal

    int next_move = node->untried_moves[0];                 // nächster Move wird der erste auf der Liste
    node->untried_moves.erase(node->untried_moves.begin()); // lösche das erste Element

    board new_board_state = node->this_board.make_move(next_move); // erstelle neues Board mit dem untried move

    board_node *new_child = new board_node(new_board_state, node, next_move); // Generiere einen Pointer zu dem neu erstellten Kind

    node->children.push_back(new_child); // Kind hinzufügen

    return new_child; // Gib Zeiger auf den neuen Knoten zurück
}

double simulate(board current_board) // call by name damit nichts aus Versehen zerstört wird
{
    board simulate_board = current_board; // für Simulation
    int sim_player = (current_board.get_next_player() == 1 ? int(Player::O) : int(Player::X));

    while (!simulate_board.is_terminal()) // Simuliere Spiel durch mit zufälligen Moves
    {
        vector<int> pos_moves = simulate_board.get_possible_moves();

        int move = pos_moves[rndm(pos_moves.size())]; // Wähle zufälligen Move
        simulate_board = simulate_board.make_move(move); // Spiele damit weiter bis zum Ende
    }

    // int player = current_board.get_next_player();
    int winner = simulate_board.get_winner();

    if (winner == sim_player) // Aktueller Player gewinnt
        return 1.0;
    else if (winner == int(Player::NONE))
        return 0.5; // Unentschieden besser als verlieren
    else
        return 0.; // verloren
}

void backprop(board_node *node, double sim_result)
{
    board_node *temp_node = node;
    double result = sim_result; // Start mit dem Blatt-Ergebnis

    while (temp_node != nullptr) // bis zum ANker
    {
        temp_node->games_played++;
        temp_node->game_wins += result; // den gerade gültigen Wert addieren
        result = 1.0 - result;          // für den Parent "invertieren"
        temp_node = temp_node->parent;
    }
}

int mcts_get_move(board game, int n) // n ist Zahl der Iterationen
{
    board_node game_anchor(game, nullptr, -1);

    for (int i = 0; i < n; i++)
    {
        board_node *promising = &game_anchor; // Starte beim Anker

        // Selection
        // Spiele den Baum bis unten durch mit den besten Kindern
        while (!promising->this_board.is_terminal() && // Solange das Spiel nicht um ist
               promising->untried_moves.empty())       // und Kinder existieren
        {
            // Wenn keine untried moves da sind, wähle das beste Kind
            if (!promising->children.empty())
                promising = select_best_child(promising);
            else
                break;
        }

        // Expansion
        if (!promising->this_board.is_terminal() && !(promising->untried_moves.empty()))
        {
            // Expansion: Erzeuge EINEN neuen Kindknoten aus einem untried move
            promising = expand(promising);
        }

        // Simulation
        double sim_res = simulate(promising->this_board);

        // Backpropagation
        backprop(promising, sim_res);
    }

    // Jetzt kann der beste Zug gewählt werden

    int best_move = -1;
    double best_win_rate = -1.;
    // double highest_games_played = -1.;

    for (int i = 0; i < game_anchor.children.size(); i++) // Alle möglichen Optionen durchgehen
    {
        board_node *child = game_anchor.children[i];

        if (child->games_played == 0) // falls div / 0 überspringen
            continue;

        double win_rate = child->game_wins / double(child->games_played);

        cout << "Child Move: " << child->parent_move
             << ", WinRate: " << win_rate
             << " (" << child->game_wins << "/" << child->games_played << ")" << endl;

        if (win_rate > best_win_rate)
        {
            best_win_rate = win_rate;
            best_move = i; // das ist dann unser bester Move
        }
        else if (win_rate == best_win_rate && child->games_played > game_anchor.children[best_move]->games_played)
        {
            best_win_rate = win_rate;
            best_move = i; // bzw. bei Gleichstand mit anderem den Move wählen, der mehr Spiele hat
        }
    }

    return game_anchor.children[best_move]->parent_move;
}

void printfield(board *b)
{
    // clear terminal on Windows or a proper OS
    // #ifdef _WIN32
    //     system("cls");
    // #else
    //     system("clear");
    // #endif

    for (int i = 0; i < 3; i++) // iterate through rows
    {
        cout << "   |   |   \n ";
        for (int j = 0; j < 3; j++) // iterate through columns
        {
            if (b->get_cell_state(i * 3 + j) == int(Player::NONE))
            {
                cout << " ";
            }
            else if (b->get_cell_state(i * 3 + j) == int(Player::O))
            {
                cout << "O";
            }
            else if (b->get_cell_state(i * 3 + j) == int(Player::X))
                cout << "X";
            else
                cout << "?";

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

    board game = board();

    while (!game.is_terminal())
    {
        printfield(&game);

        int player = game.get_next_player();
        int move = -1;
        if (player == int(Player::X)) // Mensch
        {
            bool valid = false;
            vector<int> valid_moves = game.get_possible_moves();

            cout << "X ist am Zug. Gib einen Zug ein (1-9): ";
            while (!valid)
            {
                cin >> move;
                move--; // [1-9] |-> [0-8]
                for (int i = 0; i < valid_moves.size(); i++)
                    if (valid_moves[i] == move)
                        valid = true;
                cout << "Ungültige Eingabe. Erneut probieren: ";
            }
            cout << endl;
        }
        else
        {
            cout << "AI (O) macht Zug... " << endl;
            move = mcts_get_move(game, 100000);
        }
        game = game.make_move(move);
    }

    printfield(&game);

    cout << endl;
    if (game.get_winner() == int(Player::X))
        cout << "X hat gewonnen.";
    else if (game.get_winner() == int(Player::O))
        cout << "O hat gewonnen.";
    else if (game.is_draw())
        cout << "Es ist unentschieden.";

    cout << endl
         << endl;

    return 0;
}