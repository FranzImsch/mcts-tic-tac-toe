/*
Projektarbeit 1 – MCTS
Gruppe:
Jasmin Ballein
Felix Bär
Kolja Wallentin
Franz Imschweiler
Wir stimmen der Veröffentlichung unseres Source Code in anonymisierter Form zu.

Copyright (C) 2025 Ballein, Bär, Wallentin, Imschweiler
SPDX-License-Identifier: MIT
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

int rndm(int upper_bound) // Erzeugt eine Zufallszahl zwischen 0 und der gegebenen Obergrenze
{
    return rand() % upper_bound; // Zufallszahl Modulo Obergrenze, resultiert in Zahl in [0 ; upper_bound]
}

enum class Player // Ersatzsymbole für X, O und kein Spieler, um Code besser lesbar zu machen.
{                 // Syntax: Player::NONE oder ähnliches (muss noch als Datentyp gecastet werden!)
    NONE = 0,
    X = 1,
    O = 2
};

class board // Klasse um ein beliebiges Spielbrett zu erzeugen
{
    /*All das ist privat, kann also von außerhalb der Klasse nicht direkt gelesen/geschrieben werden!*/
    vector<int> board_state; // 0 = leer, 1 = X, 2=O
    int turn;                // Anzahl Züge bis jetzt
    bool next_player;        // 0 = O, 1 = X (Player in diesem Spiel)

    /*All das ist öffentlich*/
public:
    board(int turn_nr, vector<int> currentstate, bool last_played) // Konstruktor, erzeugt ein neues Brett mit dem Status der in
    {                                                              // currentstate übergeben wird, dem aktuellen Zug und welcher Spieler
        for (int jj = 0; jj < 9; jj++)                             // als letztes gezogen hat
        {                                                          // Für jedes Feld in board_state wird der entsprechende Zustand
            this->board_state.push_back(currentstate[jj]);         // aus currentstate übernommen.
        }
        this->turn = turn_nr;             // Zugnummer wird übernommen
        this->next_player = !last_played; // Letzter Spieler wird invertiert, also abgewechselt.
    }
    board()                                                 // Überladener Konstruktor, erzeugt ein leeres Feld
    {                                                       // wenn keine Parameter übergeben werden.
        for (int jj = 0; jj < 9; jj++)                      //
        {                                                   //
            this->board_state.push_back(int(Player::NONE)); // Jedes Feld mit leer initialisieren
        }
        this->turn = 0;                      // Zugnummer mit 0 initialisieren
        this->next_player = bool(Player::X); // Standard: X fangt an (der Cast funktioniert,
    } // weil Player::X nur 1 entspricht und damit als "wahr" gecastet wird.)
    int get_cell_state(int cell)        // Gibt den Status einer bestimmten Zelle zurück,
    {                                   // mit 0=leeres Feld,1 = X in diesem Feld,
        return this->board_state[cell]; // 2 = O in diesem Feld
    }
    int get_turn() // Gibt den aktuellen Zug zurück (Standard getter-funktion)
    {
        return this->turn;
    }
    int get_next_player()          // Gibt zurück, welcher Spieler als nachstes Spielen darf
    {                              // Im Prinzip auch eine ganz normale getter-funktion, nur dass der Return-Wert nicht direkt dem gespeicherten Wert
        if (this->next_player)     // entspricht.
            return int(Player::X); // Hier wird X zurückgegeben, wenn der nächste Spieler X ist

        else
            return int(Player::O); // Ansonsten O
    }

    vector<int> get_possible_moves() // Gibt die Menge an möglichen Zügen im aktuellen Brett zurück
    {
        vector<int> possible_moves; // Der Vektor, der die Felder die noch bespielt werden können, enthält

        for (int i = 0; i < 9; i++)                        // Für jedes Feld in board_state wird überprüft, ob es leer ist.
            if (this->board_state[i] == int(Player::NONE)) // Falls ja, wird dieses Feld in den vector possible_moves
                possible_moves.push_back(i);               // eingefügt, ansonsten eben nicht

        return possible_moves; // Falls alle Felder belegt sind, gibt diese Funktion einen leeren Vektor zurück.
    }

    board make_move(int cell) // Erzeugt ein neues Brett, in dem der aktuelle Zug gemacht wurde.
    {
        if (cell > 8 || cell < 0 || board_state[cell] != int(Player::NONE)) // Fehler-erkennung falls das Feld nicht frei
            cout << "error making move" << endl;                            // oder außerhalb des Spielfelds ist.
                                                                            // Außer einer Fehlermeldung wird nichts ausgegeben, da trotzdem
                                                                            // ein return-Wert erwartet wird. Falls es jemals zu diesem Fehler
                                                                            // kommt, stürzt das Programm einfach ab,
                                                                            // weil wir kein Error-handling außerhalb dieser Funktion haben.
        vector<int> new_board_state = this->board_state;                    // Den aktuellen Brett-Zustand übernehmen

        int next_move;

        if (this->next_player)
            next_move = int(Player::X);
        else
            next_move = int(Player::O);    // Next player wird in den entsprechenden int-Wert umgewandelt
        new_board_state[cell] = next_move; // Den neuen Zug im Brett speichern

        board new_board(turn + 1, new_board_state, next_player); // Neues Board mit neuem Zustand und anderem Spieler erzeugen

        return new_board; // Und auch zurückgeben
    }

    bool is_draw() // Überprüfen ob das Brett unentschieden ist
    {
        if (check_win(int(Player::X)) || check_win(int(Player::O))) // Wenn es einen Gewinner gibt, dann kein Unentschieden
            return false;

        if (this->turn >= 9) // Wenn es keinen Gewinner gibt, wir aber schon im 9. Zug sind, dann ist es unentschieden
            return true;

        return false; // sonst noch nicht Untentschieden
    }

    bool check_win(int player) // Überprüfen, ob der gegebene Spieler gewonnen hat.
    {
        if (this->turn < 5) // So lange noch nicht 5 Züge gespielt wurden, kann niemand gewonnen haben
        {
            return false;
        }
        for (int i = 0; i < 9; i += 3) // Reihen testen
            if (this->board_state[i] == player &&
                this->board_state[i + 1] == player &&
                this->board_state[i + 2] == player)
                return true;

        for (int i = 0; i < 3; i += 1) // Spalten testen
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

        return false; // andernfalls ist der player kein Gewinner
    }

    bool ist_todgeweiht() // Brett ist todgeweiht, wenn
    {
        if (check_win(int(Player::X)) || check_win(int(Player::O)) || is_draw()) // Gewinner ∃ oder unentschieden ist
            return true;

        return false; // sonst nicht
    }

    int get_winner() // Findet heraus, welcher Spieler gewonnnen hat (falls irgendeiner gewonnen hat)
    {
        if (check_win(int(Player::X)))
            return int(Player::X);
        else if (check_win(int(Player::O)))
            return int(Player::O);
        else
            return int(Player::NONE); // Ansonsten wird zurückgegeben dass niemand gewonnen hat
    }
};

struct board_node // Struct für die Knoten des Monte-Carlo Baumes
{
    board this_board;              // Das Brett in diesem Knoten
    board_node *parent = nullptr;  // Elternknoten; nullptr für Anker
    vector<board_node *> children; // Menge an Kinderknoten (maximal 8)

    int parent_move = 0; // Wie wurde der Knoten erzeugt?

    int games_played = 0; // Spiele die ab hier gespielt wurden
    double game_wins = 0; // Anzahl Gewinne des Spielers aus Sicht des Elternknotens

    vector<int> untried_moves; // Züge, die noch nicht ausprobiert wurden (ab dem jetzigen Brett)

    board_node(board current_board, board_node *parent_node, int move) // Konstruktor für einen Knoten
    {
        this->this_board = current_board;
        this->parent = parent_node;
        this->parent_move = move;
        this->games_played = 0;                                // bei Erstellung 0
        this->game_wins = 0.;                                  // auch 0
        this->untried_moves = this_board.get_possible_moves(); // alle Züge wurden noch nicht probiert
    }

    ~board_node() // Destruktor gegen Memory Leaks von den Kindern, die unten erstellt werden ^^'
    {
        for (int i = 0; i < children.size(); i++)
            delete children[i]; // Wenn das nicht gemacht wird, würden noch unzählige Kinderknoten herumschwirren, ohne
    } // einen Pointer darauf zu haben. Das würde sehr schnell den Heap füllen.
};

/*++++++ Ab hier beginnt der eigentliche MCTS-Algorithmus (bei dem man mitmuss) +++++*/
/*-----------------------------------------------------------------------------------*/
const double C = sqrt(2);

board_node *select_best_child(board_node *node) // Funktion um den besten Kind-Knoten herauszufinden
{
    double best_score = -INFINITY;

    board_node *best_child = nullptr;

    for (int i = 0; i < node->children.size(); i++) // Jeden Kind-Knoten vom übergebenen Knoten durchgehen
    {
        board_node *child = node->children[i];
        double ucb;
        if (child->games_played == 0) // Falls dieser Kind-Knoten noch nicht angespielt wurde, kriegt er den höchsten Score der möglich ist.
        {                             // Dadurch werden noch "unbekannte" Optionen bevorzugt
            ucb = INFINITY;
        }
        else // Ansonsten wird die ucb-Formel angewandt, um den Score zu berechnen
        {
            ucb = (child->game_wins / double(child->games_played)) + C * sqrt(log(double(node->games_played)) / double(child->games_played)); // UCB des Kindes ausrechnen nach Formel aus VL
        }
        if (ucb > best_score) // Wenn der berechnete ucb besser als der bisherige best_score ist,
        {                     // wird das aktuelle Kind als bester Knoten ausgewählt und der ucb-score übernommen
            best_score = ucb;
            best_child = child;
        }
    }
    return best_child; // Am Ende wird das beste Kind zurückgegeben
}

board_node *expand(board_node *node) // Den aktuellen Knoten erweitern
{
    if (node->untried_moves.size() == 0) // wenn es keine möglichen Züge mehr gibt,
        return nullptr;                  // dann kann der Knoten auch nicht erweitert werden

    int next_move = node->untried_moves[0];                 // Nächster Move wird der erste aus den unversuchten Zügen
    node->untried_moves.erase(node->untried_moves.begin()); // Natürlich den gewählten Zug dann auch löschen, jetzt ist
                                                            // er nicht mehr unversucht

    board new_board_state = node->this_board.make_move(next_move); // erstelle neues Board mit dem ausgewählten Zug

    board_node *new_child = new board_node(new_board_state, node, next_move); // Generiere einen Pointer zu dem neu erstellten Kind

    node->children.push_back(new_child); // Diesen Kind-Knoten der Liste an Kind-Knoten von diesem Knoten hinzufügen

    return new_child; // Gib Zeiger auf den neuen Knoten zurück
}

double simulate(board current_board) // call by name damit nichts aus Versehen zerstört wird
{
    board simulate_board = current_board; // Brett trotzdem nochmal kopieren
    int sim_player;
    if (current_board.get_next_player() == 1) // Den betrachteten Spieler invertieren
        sim_player = int(Player::O);

    else
        sim_player = int(Player::X);

    while (!simulate_board.ist_todgeweiht()) // Simuliere Spiel durch mit zufalligen Moves solange das Brett noch nicht zu Ende ist
    {
        vector<int> pos_moves = simulate_board.get_possible_moves();

        int move = pos_moves[rndm(pos_moves.size())];    // Wahle zufalligen Move aus den möglichen Zügen
        simulate_board = simulate_board.make_move(move); // Spiele damit weiter bis zum Ende
    }

    int winner = simulate_board.get_winner(); // Überprüfen wer gewonnen hat

    if (winner == sim_player) // Falls der aktuelle Player gewinnt
        return 1.0;           // gib 1 zurück
    else if (winner == int(Player::NONE))
        return 0.5; // Unentschieden besser als verlieren, aber trotzdem blöd
    else
        return 0.; // verloren, dieses Spiel kriegt keinen guten Score
}

void backprop(board_node *node, double sim_result) // Die Back-propagation des Monte-Carlo-Tree-Search
{
    board_node *temp_node = node; // aktuellen Knoten übernehmen
    double result = sim_result;   // Start mit dem Blatt-Ergebnis

    while (temp_node != nullptr) // bis zum Anker
    {
        temp_node->games_played++;      // Spiel-anzahl erhöhen
        temp_node->game_wins += result; // den gerade gültigen Wert addieren
        result = 1.0 - result;          // für den Parent "invertieren", ein Gewinn wird also zu 0, eine Niederlage zu 1
                                        // (unentschieden bleibt gleich)
        temp_node = temp_node->parent;  // Einen Knoten "nach oben" gehen
    }
}

int mcts_get_move(board game, int n) // n ist Zahl der Iterationen
{
    board_node game_anchor(game, nullptr, -1); // Anker für diesen Monte-Carlo-Baum erzeugen

    for (int i = 0; i < n; i++) // Für die anzahl Iterationen
    {
        board_node *promising = &game_anchor; // Starte beim Anker

        // Selection
        // Spiele den Baum bis unten durch mit den besten Kindern
        while (!promising->this_board.ist_todgeweiht() && // Solange das Spiel nicht um ist
               promising->untried_moves.empty())          // und alle möglichen Kinder existieren
        {

            if (!promising->children.empty())             // Wenn es Kinderknoten gibt
                promising = select_best_child(promising); // Wird der beste der Kinderknoten ausgewählt
            else
                break; // sobald es keine Kinderknoten mehr gibt (am Blatt angekommen)
                       // wird die Schleife verlassen
        }

        // Expansion
        if (!promising->this_board.ist_todgeweiht() && !(promising->untried_moves.empty())) // Wenn dieses Brett nicht zu Ende gespielt ist
        {
            // Expansion: Erzeuge EINEN neuen Kindknoten aus dem ersten unversuchten Zug
            promising = expand(promising);
        }

        // Simulation
        double sim_res = simulate(promising->this_board);

        // Backpropagation
        backprop(promising, sim_res);
    }

    // Jetzt kann der beste Zug gewahlt werden

    int best_move = -1;
    double best_win_rate = -1.;

    for (int i = 0; i < game_anchor.children.size(); i++) // Alle möglichen Kinderknoten vom Anker durchgehen
    {
        board_node *child = game_anchor.children[i];

        if (child->games_played == 0) // Um Division durch 0 zu verhindern, wird vorher dieser Kind-Knoten übersprungen
            continue;

        double win_rate = child->game_wins / double(child->games_played); // Kill-Death-Ratio berechnen

        cout << "Child Move: " << child->parent_move // Informationen auf der Konsole ausgeben
             << ", WinRate: " << win_rate
             << " (" << child->game_wins << "/" << child->games_played << ")" << endl;

        if (win_rate > best_win_rate) // Wenn die aktuelle win-rate besser ist als die bisher beste win-rate
        {
            best_win_rate = win_rate;
            best_move = i; // das ist dann unser bester Move
        }
        else if (win_rate == best_win_rate && child->games_played > game_anchor.children[best_move]->games_played)
        {
            best_win_rate = win_rate;
            best_move = i; // bzw. bei Gleichstand mit anderem den Move wahlen, der mehr Spiele hat
        }
    }

    return game_anchor.children[best_move]->parent_move; // Der Zug der zum besten Kind-Knoten geführt hat wird zurückgegeben
}

void printfield(board *b) // Gibt das Spielfeld in der Konsole aus
{

    for (int i = 0; i < 3; i++) // Durch die Reihen durchgehen
    {
        cout << "   |   |   \n ";
        for (int j = 0; j < 3; j++) // Durch die Spalten durchgehen
        {
            if (b->get_cell_state(i * 3 + j) == int(Player::NONE)) // Leeres Feld
            {
                cout << " "; // Leer ausgeben
            }
            else if (b->get_cell_state(i * 3 + j) == int(Player::O)) // O-Feld
            {
                cout << "O"; // O ausgeben
            }
            else if (b->get_cell_state(i * 3 + j) == int(Player::X)) // X-Feld
                cout << "X";                                         // X ausgeben

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

    // Begrüßungstext
    cout << "Willkommen beim Next Gen. AI  powered Tic-Tac-Toe-Spiel!\n";
    cout << "Hier kannst du auch ohne echte Freunde spielen." << endl;

    // Charakterauswahl
    cout << "Wahle zunachst aus, ob Du X oder O spielen willst.\n";
    cout << "Beachte dabei, dass X das Spiel beginnt.\n";
    cout << "Gib C ein, um den Computer gegen sich selbst spielen zu lassen.\n";
    cout << "Wahle (X/O/C):" << endl;

    int player_start = -1;
    char input;
    bool valid = false;
    while (!valid)
    {
        cin >> input; // Spieler-Input verarbeiten

        if (input == 'X' || input == 'x') // Wenn X eingegeben wurde
        {
            player_start = int(Player::X); // Den start-spieler als X definieren
            valid = true;                  // Spielstart erlauben
        }
        else if (input == 'O' || input == 'o') // Wenn O eingegeben wurde
        {
            player_start = int(Player::O);
            valid = true; // Start-Spieler als O definieren und erlauben
        }
        else if (input == 'C' || input == 'c') // Wenn C eingegeben wurde
        {
            player_start = int(Player::NONE); // KI gegen KI spielen lassen
            valid = true;
        }
        else
            cout << "Ungültige Eingabe. Erneut versuchen (X/O/C): ";
    }

    // erstelle Spielfeld
    board game = board();

    if (player_start != int(Player::NONE)) // Wenn Mensch vs. Maschine ausgewahlt wurde
    {
        while (!game.ist_todgeweiht()) // So lange das spiel noch nicht zu Ende ist
        {
            printfield(&game); // Spielfeld anzeigen

            int player = game.get_next_player();
            int move = -1;
            if (player == player_start) // Wenn der nächste Spieler auf diesem Brett der menschliche Spieler ist
            {
                valid = false;
                vector<int> valid_moves = game.get_possible_moves();

                if(player_start == 1){
                    cout << "X ist am Zug. Gib einen Zug ein (1-9): ";
                }else{
                    cout << "O ist am Zug. Gib einen Zug ein (1-9): ";
                }
                while (!valid) // Eingabe verarbeiten
                {
                    cin >> move;
                    move--; // [1-9] |-> [0-8]
                    for (int i = 0; i < valid_moves.size(); i++)
                        if (valid_moves[i] == move)
                        {
                            valid = true;
                            break;
                        }
                    if (!valid)
                        cout << "Ungültige Eingabe. Erneut probieren: ";
                }
                cout << endl;
            }
            else // Ansonsten ist die KI am Zug
            {
                if(player_start == 1){
                    cout << "AI (O) macht Zug... " << endl;
                }else{
                    cout << "AI (X) macht Zug... " << endl;
                }
                move = mcts_get_move(game, 100000); // 100k Bretter mit mcts simulieren und den besten Zug AB DEM JETZIGEN BRETT auswählen
                                                    // Nach jedem Mensch-Zug wird der Baum also komplett neu berechnet
            }
            game = game.make_move(move); // Den besten Zug auch machen
        }
        // Ab jetzt ist das Brett todgeweiht
        printfield(&game); // Spielfeld ausgeben

        cout << endl;
        if (game.get_winner() == int(Player::X)) // Überprüfen wer gewonnen hat und entsprechende Ausgabe machen
            cout << "X hat gewonnen.";
        else if (game.get_winner() == int(Player::O))
            cout << "O hat gewonnen.";
        else if (game.is_draw())
            cout << "Es ist unentschieden.";

        cout << endl
             << endl;

        return 0; // Programm endet nach einer Partie
    }
    else
    {
        // Spiele n Spiele Maschine vs. Maschine und gib Statistik aus.
        vector<int> winner_stats;

        int n_simulations = 100; // n: Anzahl der Spiele AI <-> AI

        for (int i = 0; i < n_simulations; i++) // Gleiche gameplay-loop wie bei Mensch vs. Maschine, nur dass 2 KIs gegeneinander spielen
        {
            board game = board();

            while (!game.ist_todgeweiht())
            {
                int player = game.get_next_player();
                int move = -1;
                if (player == int(Player::X))
                {
                    cout << "AI (X) macht Zug... " << endl;
                    move = mcts_get_move(game, 100000);
                }
                else // AI
                {
                    cout << "AI (O) macht Zug... " << endl;
                    move = mcts_get_move(game, 100000);
                }
                game = game.make_move(move);
                printfield(&game);
            }

            winner_stats.push_back(game.get_winner()); // Speichere Ergebnis in Liste
        }

        int X_wins = 0, O_wins = 0, Draws = 0;

        for (int i = 0; i < winner_stats.size(); i++)
        {
            if (winner_stats[i] == int(Player::X))
                X_wins++;
            else if (winner_stats[i] == int(Player::O))
                O_wins++;
            else
                Draws++;
        }

        cout << endl
             << endl
             << endl
             << endl;

        cout << "Ergebnisse sehen wie folgt aus." << endl // Ergebnisse auf die Konsole schreiben
             << "Gespielte Spiele: " << winner_stats.size() << endl
             << "Gewinne X: " << X_wins << endl
             << "Gewinne O: " << O_wins << endl
             << "Unentschiedene Spiele: " << Draws << endl
             << endl;
    }
}