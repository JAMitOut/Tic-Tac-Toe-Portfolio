#include <iostream>
#include <array>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

class Board {
public:
	using Cells = array<char, 9>;

	Board() { clearBoard(); }

	void clearBoard() { cells.fill(' '); }

	char get(int idx) const { return cells[idx]; }			//Reads a Cell
	void set(int idx, char value) { cells[idx] = value; }	//Writes into a Cell with a Mark

	bool isFull() const {
		return all_of(cells.begin(), cells.end(), [](char c) { return c != ' '; });
	}

	//Battle Mode Only
	int countPlaced() const {
		return static_cast<int>(count_if(cells.begin(), cells.end(), [](char c) {return c != ' '; }));
	}

	char winner() const {
		for (const auto& line : win_lines) {
			char a = cells[line[0]];
			if (a != ' ' && a == cells[line[1]] && a == cells[line[2]]) {
				return a; // X/O or Custom Mark
			}
		}
		return ' '; //No Winner
	}

	void printBoard() const {
		auto cellText = [&](int i) -> string {
			if (cells[i] == ' ') {
				return to_string(i + 1); //Empty, Show index
			}
			return string(1, cells[i]);	 //Occupied, Show Symbol
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


private:
	Cells cells{};

	static constexpr array<array<int, 3>, 8> win_lines{ {
		{{0,1,2}}, {{3,4,5}}, {{6,7,8}}, //rows
		{{0,3,6}}, {{1,4,7}}, {{2,5,8}}, //colums
		{{0,4,8}}, {{2,4,6}}		   //diagonals
	} };

};


// ------------- Utility Input Helpers -------------

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

		if (b.get(idx) != ' ') {
			cout << "  That space is already taken. Choose another.\n";
			continue;
		}

		return idx;
	}
}

bool isAllowedMark(char c) {
	if (isalpha(static_cast<unsigned char>(c))) return true;	//A-Z, a-z
	switch (c) { case '?': case '!': case '*': case '~': case '$': case '%': case '#': return true;
	default:
		return false;
	}
}

char promptMark(const string& playerLabel, char forbidden = '\0') {
	while (true) {
		cout << playerLabel << ", choose your mark (One Char: A-Z, a-z, ?, !, *, ~, $, %, #): ";
		string s;

		if (!getline(cin, s)) {
			cout << "\nInput Stream Closed. Exiting\n";
			exit(0);
		}

		if (s.size() != 1) {
			cout << "Please enter exactly one Character.\n";
			continue;
		}

		char c = s[0];
		if (!isAllowedMark(c)) {
			cout << "\tThat Character is not allowed.\n";
			continue;
		}
		if (isspace(static_cast<unsigned char>(c))) {
			cout << "\tSpace is not allowed.\n";
			continue;
		}
		if (forbidden != '\0' && c == forbidden) {
			cout << "\tThat Mark is already taken by the other player.\n";
			continue;
		}

		return c;
	}
}

string promptArch(const string& playerLabel) {
	while (true) {
		cout << playerLabel << ", choose your Archetype (Alchemist / Paladin): ";
		string s;

		if (!getline(cin, s)) {
			cout << "\nInput stream closed. Exiting.\n";
			exit(0);
		}

		string lower;
		for (char c : s) {
			lower += static_cast<char>(tolower(static_cast<unsigned char>(c)));
		}

		if (lower == "paladin" || lower == "alchemist") { return lower; }

		cout << "\tInvalid Archetype. Please enter either Paladin or Alchemist.\n";
	}
}

int promptAnyOccupiedIdx(const Board& b, const string& msg) {
	while (true) {
		cout << msg;
		string s;
		if (!getline(cin, s)) {
			cout << "Input Closed. Exiting.\n";
			exit(0);
		}

		int idx = parseMove(s);
		if (idx == -1) {
			cout << "Invalid Cell.\n";
			continue;
		}

		if (b.get(idx) == ' ') {
			cout << "\tThat Cell is Empty.\n";
			continue;
		}

		return idx;
	}
}

int promptAnyIdx(const string& msg) {
	while (true) {
		cout << msg;
		string s;
		if (!getline(cin, s)) {
			cout << "Input Closed. Exiting.\n";
			exit(0);
		}

		int idx = parseMove(s);
		if (idx == -1) {
			cout << "Invalid Cell.\n";
			continue;
		}

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


// ------------- Player & Archetypes -------------

enum class Archetype {
	None,
	Alchemist,
	Paladin
};

struct Player {
	string name;
	char mark = 'X';
	Archetype archetype = Archetype::None;
};


// ------------- Base Game Class -------------

class TicTacToeGame {
public:
	virtual ~TicTacToeGame() = default;

	void run() {
		board.clearBoard();
		turn = 0;
		setupPlayers();
		
		bool gameOver = false;
		while (!gameOver) {
			board.printBoard();

			Player& current = players[turn % 2];
			doTurn(current);

			char w = board.winner();
			if (w != ' ') {
				board.printBoard();
				cout << w << " won\n\n";
				gameOver = true;
			}
			else if (board.isFull()) {
				board.printBoard();
				cout << "Tie\n";
				gameOver = true;
			}
			else {
				++turn;
			}
		}
	}

protected:
	Board board;
	Player players[2];
	int turn{ 0 };

	virtual void setupPlayers() = 0;
	virtual void doTurn(Player& player) = 0;
};


// ------------- Regular Tic Tac Toe -------------

class RegularGame : public TicTacToeGame {
protected:
	void setupPlayers() override {
		players[0].name = "Player 1";
		players[0].mark = 'X';
		players[0].archetype = Archetype::None;

		players[1].name = "Player 2";
		players[1].mark = 'O';
		players[1].archetype = Archetype::None;
	}

	void doTurn(Player& player) override {
		int idx = promptMove(board, player.mark, player.name);
		board.set(idx, player.mark);
	}
};


// ------------- Battle Tic Tac Toe -------------

class BattleGame : public TicTacToeGame {
protected:
	void setupPlayers() override {
		cout << "\n -- Battle Tic Tac Toe Setup --\n";

		players[0].name = "Player 1";
		players[1].name = "Player 2";

		players[0].mark = promptMark(players[0].name);
		players[1].mark = promptMark(players[1].name, players[0].mark);

		cout << "\n" << players[0].name << " chose '" << players[0].mark << "'.\n";
		cout << players[1].name << " chose '" << players[1].mark << "'.\n";

		string a1 = promptArch(players[0].name);
		string a2 = promptArch(players[1].name);

		players[0].archetype = (a1 == "alchemist" ? Archetype::Alchemist : Archetype::Paladin);
		players[1].archetype = (a2 == "alchemist" ? Archetype::Alchemist : Archetype::Paladin);

		cout << "\n" << players[0].name << " chose '" << a1 << "'.\n";
		cout << players[1].name << " chose '" << a2 << "'.\n\n";

	}

	void doTurn(Player& player) override {

		bool tookAction = false;
		while (!tookAction) {
			cout << player.name << " (" << player.mark << ") - Choose Action:\n";
			cout << " 1) Regular Move\n";
			if (player.archetype == Archetype::Alchemist) {
				cout << " 2) Alchemist: swap two marks\n";
			}
			if (player.archetype == Archetype::Paladin) {
				cout << "2) Paladin: shift a mark to an adjactent empty cell\n";
			}
			cout << "Select: ";

			string s;
			if (!getline(cin, s)) {
				cout << "\nInput closed. Exiting.\n";
				exit(0);
			}

			//Regular Move
			if (s == "1") {
				int idx = promptMove(board, player.mark, player.name);
				board.set(idx, player.mark);
				tookAction = true;
			}

			//Alchemist Check/Move
			else if (s == "2" && player.archetype == Archetype::Alchemist) {
				if (board.countPlaced() < 2) {
					cout << "You can't swap yet, you need at least two marks on the board to swap.\n";
					continue;
				}

				int a = promptAnyOccupiedIdx(board, " Choose first occupied cell to swap: ");
				int b = promptAnyOccupiedIdx(board, " Choose second occupied cell to swap: ");

				//Alchemist Error Checks
				if (a == b) {
					cout << "You chose the same cell twice.\n";
					continue;
				}
				if (board.get(a) == board.get(b)) {
					cout << "\tThose Match-Swaps would be Pointless.\n";
					continue;
				}

				char tmp = board.get(a);
				board.set(a, board.get(b));
				board.set(b, tmp);
				tookAction = true;
			}

			//Paladin Check/Move
			else if (s == "2" && player.archetype == Archetype::Paladin) {
				if (board.countPlaced() < 1) {
					cout << "\tYou can't shift yet, you need at least one mark on the board to shift.\n";
					continue;
				}

				int from = promptAnyOccupiedIdx(board, " Choose an occupied cell to shift: ");
				int to = promptAnyIdx("\tChoose a cell to shift to: ");

				//Paladin Error Checks
				if (from == to) {
					cout << " Destination must be different.\n";
					continue;
				}
				if (!isAdjacent(from, to)) {
					cout << " Destination is not adjacent.\n";
					continue;
				}
				if (board.get(to) != ' ') {
					cout << "Destination must be empty.\n";
					continue;
				}

				board.set(to, board.get(from));
				board.set(from, ' ');
				tookAction = true;
			}
			else {
				cout << "\tInvalid Choice.\n";
			}

		}
	}
};


// ------------- Main -------------

int main() {
	int choice;
	bool running = true;

	cout << "-Tic Tac Toe-\n"
		<< "Player 1/X, Player 2/O\n"
		<< "Get three in a row\n";

	while (running) {
		cout << "\n1 for Regular Tic Tac Toe\n"
			<< "2 for Battle Tic Tac Toe\n"
			<< "3 to Quit\n"
			<< "Enter Choice: ";

		if (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Please Try Again.\n";
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (choice) {
			case 1: {
				cout << "\nRegular Tic Tac Toe Chosen:\n";
				RegularGame game;
				game.run();
				break;
			}
			case 2: {
				cout << "\nBattle Tic Tac Toe Chosen:\n";
				BattleGame game;
				game.run();
				break;
			}
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