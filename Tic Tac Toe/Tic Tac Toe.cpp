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
int promptMove(const Board& b, char playerSymbol) {
	while (true) {
		cout << "Player " << (playerSymbol == 'X' ? "1 (X)" : "2 (O)")
			<< ", choose a cell (1-9 or a-i): ";

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

void playRegular() {
	Board board;
	clearBoard(board);

	int turn = 0;
	bool gameOver = false;

	while (!gameOver) {
		printBoard(board);
		char player = currentPlayer(turn);

		if (winner(board) != ' ' || boardFull(board)) break;

		int moveIndex = promptMove(board, player);
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