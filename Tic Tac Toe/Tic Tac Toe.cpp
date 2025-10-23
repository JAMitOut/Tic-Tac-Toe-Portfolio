#include <iostream>
#include <array>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;
using Board = array<char, 9>;

static const int win_lines[8][3]{
	{0,1,2}, {3,4,5}, {6,7,8}, //rows
	{0,3,6}, {1,4,7}, {2,5,8}, //colums
	{0,4,8}, {2,4,6}		   //diagonals
};

void clearBoard(Board& b) {
	b.fill(' ');
}

//Checks which player goes first/next
char currentPlayer(int turn) {
	return (turn % 2 == 0) ? 'X' : 'O';
}

//Checks for a Winner
char winner(const Board& b) {
	for (auto& line : win_lines) {
		char a = b[line[0]];

		if (a != ' ' && a == b[line[1]] && a == b[line[2]])
			return a; //X or O
	}
	return ' '; //No Winner
}

//Checks if the Board is full, if yes it's a Tie
bool boardFull(const Board& b) {
	return all_of(b.begin(), b.end(), [](char c) { return c != ' '; });
}

void printBoard(const Board& b) {
	auto cellText = [&](int i) -> string {
		if (b[i] == ' ') return to_string(i + 1); //Empty, Show index
		return string(1, b[i]);					  //Occupied, Show Symbol
	};

	auto row = [&](int r) {
		cout << " " << cellText(3 * r + 0) << " | " << cellText(3 * r + 1) << " | " << cellText(3 * r + 2) << "\n";
	};

	cout << "\n";
	row(0); cout << "-----------\n";
	row(1); cout << "-----------\n";
	row(2); cout << "\n";
	cout << "Enter: 1-9 or a-i\n\n";
}

int parseMove(const string& raw) {
	string s;
	for (unsigned char c : raw) {
		if (!isspace(c)) s.push_back(static_cast<char>(c));
	}
	if (s.empty()) return -1;

	if (s.size() == 1) {
		char c = static_cast<char>(tolower(static_cast<unsigned char>(s[0])));
		if (c >= 'a' && c <= 'i') return c - 'a'; // a=0 … i=8
		if (isdigit(static_cast<unsigned char>(s[0]))) {
			int n = s[0] - '0';
			if (1 <= n && n <= 9) return n - 1;
		}
	}

	try {
		size_t pos = 0;
		int n = stoi(s, &pos);
		if (pos == s.size() && 1 <= n && n <= 9) return n - 1;
	}
	catch (...) {
	}

	return -1;
}


//Checks for Bad Inputs
int promptMove(const Board& b, char playerMark, const string& label) {
	while (true) {
		cout << label << " (" << playerMark << "), choose a cell (1-9 or a-i): ";

		string line;
		if (!getline(cin, line)) {
			cout << "\nInput stream closed. Exiting.\n";
			exit(0);
		}

		int idx = parseMove(line);
		if (idx == -1) {
			cout << "  Invalid input. Please enter 1-9 or a-i.\n";
			continue;
		}
		if (b[idx] != ' ') {
			cout << "  That space is already taken. Choose another.\n";
			continue;
		}
		return idx;
	}
}

//Portfolio 2 Stuff

bool isAllowedMark(char c) {
	if (isalpha(static_cast<unsigned char>(c))) return true;	//A-Z, a-z
	switch (c) { case '?': case '!': case '*': case '~': case '$': case '%': case '#': return true; }
	return false;
}

char promptMark(const string& playerLabel) {
	bool running = true;
	while (running) {
		cout << playerLabel << ", choose your mark (One Char: A-Z, a-z, ?, !, *, ~, $, %, #): ";
		string s;

		if (!getline(cin, s)) { cout << "\nInput Stream Closed. Exiting\n"; exit(0); }
		if (s.size() != 1) { cout << "Please enter exactly one Character.\n"; continue; }

		char c = s[0];
		if (!isAllowedMark(c)) { cout << "\tThat Character is not allowed.\n"; continue; }
		if (isspace(static_cast<unsigned char>(c))) { cout << "\tSpace is not allowed.\n"; continue; }
		return c;
	}
}

string promptArch(const string& playerLabel) {
	bool running = true;
	while (running) {
		cout << playerLabel << ", choose your Archetype (Alchemist / Paladin): ";
		string s;
		if (!getline(cin, s)) {
			cout << "\nInput stream closed. Exiting.\n";
			exit(0);
		}

		string lower;
		for (char c : s)lower += static_cast<char>(tolower(static_cast<unsigned char>(c)));

		if (lower == "paladin" || lower == "alchemist") { return lower; }

		cout << "\tInvalid Archetype. Please enter either Paladin or Alchemist.\n";
	}
}

int countPlaced(const Board& b) {
	return static_cast<int>(count_if(b.begin(), b.end(), [](char c) {return c != ' '; }));
}

int promptAnyOccupiedIdx(const Board& b, const string& msg) {
	bool running = true;
	while (running) {
		cout << msg;
		string s;
		if (!getline(cin, s)) { cout << "Input Closed. Exiting.\n"; exit(0); }

		int idx = parseMove(s);
		if (idx == -1) { cout << "Invalid Cell.\n"; continue; }
		if (b[idx] == ' ') { cout << "\tThat Cell is Empty.\n"; continue; }
		return idx;
	}
}

int promptAnyIdx(const string& msg) {
	bool running = true;
	while (running) {
		cout << msg;
		string s;
		if (!getline(cin, s)) { cout << "Input Closed. Exiting.\n"; exit(0); }

		int idx = parseMove(s);
		if (idx == -1) { cout << "Invalid Cell.\n"; continue; }
		return idx;
	}
}

vector<int> adjacentCells(int idx) {
	int r = idx / 3, c = idx % 3;
	vector<int> out;
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;
			int rr = r + dr, cc = c + dc;
			if (0 <= rr && rr < 3 && 0 <= cc && cc < 3) out.push_back(rr * 3 + cc);
		}
	}
	return out;
}

bool isAdjacent(int from, int to) {
	auto adj = adjacentCells(from);
	return find(adj.begin(), adj.end(), to) != adj.end();
}

//Reqular Tic Tac Toe
void playRegular() {
	Board board;
	clearBoard(board);

	int turn = 0;
	bool gameOver = false;

	while (!gameOver) {
		printBoard(board);
		char player = currentPlayer(turn);

		if (winner(board) != ' ' || boardFull(board)) break;

		string label = (player == 'X') ? "Player 1" : "Player 2";
		int moveIndex = promptMove(board, player, label);
		board[moveIndex] = player;

		char w = winner(board);
		if (w == 'X' || w == 'O') {
			printBoard(board);
			cout << w << " won\n" << endl;
			gameOver = true;
		}
		else if (boardFull(board)) {
			printBoard(board);
			cout << "Tie\n";
			gameOver = true;
		}
		else {
			++turn;
		}
	}
}

//Battle Tic Tac Toe
void playBattle() {
	Board board;
	clearBoard(board);

	if (cin.peek() == '\n') cin.ignore(numeric_limits<streamsize>::max(), '\n');

	char p1 = promptMark("Player 1");
	char p2 = promptMark("Player 2");

	cout << "\nPlayer 1 chose '" << p1 << "'.\n";
	cout << "Player 2 chose '" << p2 << "'.\n";

	string a1 = promptArch("Player 1");
	string a2 = promptArch("Player 2");

	cout << "\nPlayer 1 chose '" << a1 << "'.\n";
	cout << "Player 2 chose '" << a2 << "'.\n";

	int turn = 0;
	bool gameOver = false;

	while (!gameOver) {
		printBoard(board);
		if (winner(board) != ' ' || boardFull(board)) break;

		//These Check for a Players Turn
		int p = turn % 2;
		char mark = (p == 0 ? p1 : p2);
		string arch = (p == 0 ? a1 : a2);
		string label = string("Player ") + (p == 0 ? "1" : "2");

		bool tookAction = false;
		while (!tookAction) {
			cout << label << " (" << mark << ") - Choose Action:\n";

			//Moves able to be taken
			cout << " 1) Regular Move\n";
			if (arch == "alchemist") cout << " 2) Alchemist: swap two marks\n";
			if (arch == "paladin") cout << " 2) Paladin: shift a mark to an adjacent empty cell\n";
			cout << "Select: ";

			string s;
			if (!getline(cin, s)) { cout << "\nInput closed. Exiting.\n"; exit(0); }

			if (s == "1") {	//Regular Move
				int idx = promptMove(board, mark, label);
				board[idx] = mark;
				tookAction = true;
			}
			else if(s == "2" && arch == "alchemist") {	//Alchemist Check/Mov
				if (countPlaced(board) < 2) {
					cout << "You can't swap yet, you need at least two marks on the board to swap.\n";
					continue;
				}

				int a = promptAnyOccupiedIdx(board, " Choose first occupied cell to swap: ");
				int b = promptAnyOccupiedIdx(board, " Choose second occupied cell to swap: ");

				//Alchemist Error Checks
				if (a == b) { cout << "You chose the same cell twice.\n"; continue; }
				if (board[a] == board[b]) { cout << "\tThose Match-Swaps would be Pointless.\n"; continue; }

				swap(board[a], board[b]);
				tookAction = true;
			}
			else if (s == "2" && arch == "paladin") {	//Paladin Check/Move
				if (countPlaced(board) < 1) {
					cout << "\tYou can't shift yet, you need at least one mark on the board to shift.\n";
					continue;
				}
				int from = promptAnyOccupiedIdx(board, " Choose an occupied cell to shift: ");
				int to = promptAnyIdx("\tChoose a cell to shift to: ");

				//Paladin Error Checks
				if (from == to) { cout << " Destination must be different.\n"; continue; }
				if (!isAdjacent(from, to)) { cout << " Destination is not adjacent.\n"; continue; }
				if (board[to] != ' ') { cout << "Destination must be empty.\n"; continue; }

				board[to] = board[from];
				board[from] = ' ';
				tookAction = true;
			}
			else {
				cout << "\tInvalid Choice.\n";
			}
		}

		char w = winner(board);
		if (w != ' ') {
			printBoard(board);
			cout << w << " won\n" << endl;
			gameOver = true;
		}
		else if (boardFull(board)) {
			printBoard(board);
			cout << "Tie\n";
			gameOver = true;
		}
		else {
			++turn;
		}
	}
}


int main() {
	int choice;
	bool running = true;

	while (running) {
		cout << "-Tic Tac Toe-\n"
			<< "Player 1/X, Player 2/O\n"
			<< "Get three in a row\n";

		cout << "\n1 for Regular Tic Tac Toe\n"
			<< "2 for Battle Tic Tac Toe\n"
			<< "3 to Quit\n";
		cin >> choice;

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (choice) {
			case 1:
				cout << "\nRegular Tic Tac Toe Chosen:\n";
				playRegular();
				break;
			case 2:
				cout << "\nBattle Tic Tac Toe Chosen:\n";
				playBattle();
				break;
			case 3:
				cout << "Quitting...\n";
				running = false;
				break;
			default:
				cout << "Invalid Choice. Please Try Again\n";
				break;
		}
	}

	return 0;
}