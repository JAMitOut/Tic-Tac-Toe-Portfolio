#include <iostream>
#include <array>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <limits>
#include <random>
#include <fstream>

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

	// -- Stats --
	int maxHP = 0;
	int hp = 0;
	int attack = 0;
	int defense = 0;
};


// -- Campaign Outcome --

enum class CampaignResult {
	Victory, Defeat, Quit
};

enum class roundOutcome {
	HeroWin, EnemyWin, Tie
};


// ------------- Enemy -------------

enum class EnemyAbility {
	//Abilits to be added
	None,
	BrittleBones,	//Skeleton
	ThickSkin,		//Zombie
	Corporeal,		//Ghost
	Opening,		//Boss 1
	CurseWeakness	//Boss 2
};

struct Enemy {
	Player stats;
	EnemyAbility a1 = EnemyAbility::None;
	EnemyAbility a2 = EnemyAbility::None; //a2 for Boss Only!
};

int randomEmptyCell(const Board& board) {
	vector<int> empty;
	for (int i = 0; i < 9; ++i) {
		if (board.get(i) == ' ') {
			empty.push_back(i);
		}
	}

	if (empty.empty()) {
		return -1; //No Move Possible
	}

	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<int> dist(0, static_cast<int>(empty.size()) - 1);
	return empty[dist(gen)];
}


// -- Random Number Gen --

int randomInt(int min, int max) {
	static random_device rd;
	static mt19937 gen(rd());
	uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}


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
				cout << " 2) Paladin: shift a mark to an adjactent empty cell\n";
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


// ------------- Campaign Tic Tac Toe -------------

class CampaignGame {
public:
	void run() {
		cout << "\n -- Campaign Tic Tac Toe Setup --\n";

		bool loaded = loadGame();

		if (loaded) {
			cout << "A previous campaign was found.\n";
			cout << "Hero: " << hero.name
				<< " (HP " << hero.hp << "/" << hero.maxHP
				<< ", ATK " << hero.attack
				<< ", DEF " << hero.defense << ")\n";
			cout << "You are currently at stage " << stage << ".\n";

			cout << "Continue this campaign? (y/n): ";
			string ans;
			if (!getline(cin, ans)) {
				cout << "\nInput closed. Exiting.\n";
				exit(0);
			}
			bool cont = !ans.empty() && (ans[0] == 'y' || ans[0] == 'Y');

			if (!cont) {
				setupHero();
				stage = 0;
			}
			else {
				cout << "Resuming your adventure...\n\n";
			}
		}
		else {
			setupHero();
			stage = 0;
		}


		bool playing = true;
		while (playing) {
			switch (stage) {
				case 0: {
					introStory();
					stage = 1;
					break;
				}
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:{
					CampaignResult result = runBattle(stage);

					if (result == CampaignResult::Defeat) {
						hero = Player();
						legendWandered = 0;
						legendWilderness = 0;
						stage = 0;
						
						saveGame();
						return;
					}
					if (result == CampaignResult::Quit) {
						saveGame();
						return;
					}

					stage++;
					saveGame();
					break;
				}
				case 2:
				case 4:
				case 6:{
					runEvent();
					stage++;
					saveGame();
					break;
				}
				case 9: {
					runEnding();
					playing = false;
					break;
				}
			}
		}
	}


private:
	Player hero;	//The Player's Character
	int stage = 0; //Campaign Progress Tracker

	// -- Legend Tracking --
	int legendWandered = 0;
	int legendWilderness = 0;

	// -- Tracking Enemy Type --
	int lastEnemyType = -1;

	// -- Hero Creator --
	void setupHero() {
		cout << "\n--- Create Your Hero ---\n";

		cout << "Enter your Hero's name: ";
		if (!getline(cin, hero.name)) {
			cout << "\nInput stream closed. Exiting.\n";
			exit(0);
		}

		if (hero.name.empty()) {
			hero.name = "Hero";
		}

		string arch = promptArch(hero.name);
		if (arch == "paladin") {
			hero.archetype = Archetype::Paladin;
			hero.maxHP = 60;
			hero.attack = 8;
			hero.defense = 6;

		}
		else {
			hero.archetype = Archetype::Alchemist;
			hero.maxHP = 45;
			hero.attack = 11;
			hero.defense = 3;
		}

		hero.hp = hero.maxHP;
		hero.mark = 'X';

		cout << "\nCreated " << hero.name << " the " << (hero.archetype == Archetype::Paladin ? "paladin" : "alchemist") << "!\n";
		cout << "HP " << hero.hp << "/" << hero.maxHP << ", ATK " << hero.attack << ", DEF " << hero.defense << "\n\n";

	}


// -- Story --
	void introStory() {
		cout << "...The Story Begins with an Evil Necromancer Halut attacking the Kingdom of Oplana...\n";
		cout << "You the Hero: " << hero.name << " has been entrusted by the king of Oplana to stop the evil necromancer and his minions.\n";
	}

	void runEnding() {
		cout << "Hurrah! The Necromancer Halut had been defeated "
			<< "thanks to you " << hero.name << " Oplana has been saved!\n";

		if (legendWilderness > legendWandered) {
			cout << hero.name << " became a legend of the wilds.\n";
		}
		else if (legendWandered > legendWilderness) {
			cout << hero.name << " became a protector of people and of the roads.\n";
		}
		else {
			cout << hero.name << " walks the line between legend and mystery.\n";
		}

		cout << "Your Legend:\n";
		cout << "Wandered Path Chosen: " << legendWandered << " times\n";
		cout << "Wilderness Path Chosen: " << legendWilderness << " times\n";
	}


// -- Events --
	void runEvent() {
		randomEvent();

		cout << "\nCurrent Stats: HP " << hero.hp << "/" << hero.maxHP
			<< ", ATK " << hero.attack << ", DEF " << hero.defense << "\n\n";
	}


// -- Battles --

	CampaignResult runBattle(int stage) {
		int isQuit = 0;
		if (stage == 3 || stage == 5 || stage == 7 || stage == 9) {
			cout << "Continue on or Quit? (1 Quit, 0 Continue) ";
			if (!(cin >> isQuit)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid input, continuing the battle.\n";
				isQuit = 0;
			}
			else {
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
		if (isQuit == 1) {
			cout << "Saving and returning to menu...\n";
			saveGame();
			return CampaignResult::Quit;
		}

		cout << "\n--- Battle Start! ---\n";

		Enemy enemy = createEnemyForStage(stage);

		cout << hero.name << " encounters " << enemy.stats.name << "!\n";
		cout << "Your HP: " << hero.hp << "/" << hero.maxHP << " | Enemy HP: " << enemy.stats.hp << "/" << enemy.stats.maxHP << "\n";

		//Abilites state
		bool usedBrittleBones = false;
		bool usedCorporeal = false;
		int thickSkinHitsLeft = 0;
		int heroCurseRounds = 0;
		bool bossBuffApplied = false;

		handleEnemyAbilities(enemy, hero, roundOutcome::Tie, usedBrittleBones, usedCorporeal, thickSkinHitsLeft,
			heroCurseRounds, bossBuffApplied);

		//Play until either Hero or Enemy dies
		while (hero.hp > 0 && enemy.stats.hp > 0) {
			cout << "\nA new round of Tic-Tac-Toe begins!\n";

			roundOutcome result = playOneBoard(hero, enemy.stats);

			if (result == roundOutcome::HeroWin) {
				int effectiveHeroAtk = hero.attack;
				if (heroCurseRounds > 0) {
					effectiveHeroAtk = hero.attack - 3;
					if (effectiveHeroAtk < 0) effectiveHeroAtk = 0;
					cout << "The curse weakens your attack this round (-3 ATK).\n";
				}

				int damage = calculateDamage(effectiveHeroAtk, enemy.stats.defense);

				if (thickSkinHitsLeft > 0) {
					int reduced = damage - 3;
					if (reduced < 1) reduced = 1;
					cout << enemy.stats.name << "'s thick skin reduces the blow! (-3 damage)\n";
					damage = reduced;
					thickSkinHitsLeft = 0; // used up
				}

				applyDamage(enemy.stats, damage);
				printDamage(hero.name, enemy.stats.name, damage,
					enemy.stats.hp, enemy.stats.maxHP);
			}
			else if (result == roundOutcome::EnemyWin) {
				// --- Enemy damage (normal) ---
				int damage = calculateDamage(enemy.stats.attack, hero.defense);
				applyDamage(hero, damage);
				printDamage(enemy.stats.name, hero.name, damage,
					hero.hp, hero.maxHP);
			}
			else {
				cout << "No Damage was dealt this round.\n";
			}

			handleEnemyAbilities(
				enemy,
				hero,
				result,
				usedBrittleBones,
				usedCorporeal,
				thickSkinHitsLeft,
				heroCurseRounds,
				bossBuffApplied
			);

			// Tick down hero curse duration at end of round
			if (heroCurseRounds > 0) {
				heroCurseRounds--;
				if (heroCurseRounds == 0) {
					cout << "The dark curse fades. Your strength returns.\n";
				}
			}

			cout << "\nStatus: HP: " << hero.hp << "/" << hero.maxHP << " | Enemy HP: " << enemy.stats.hp << "/" << enemy.stats.maxHP << "\n\n";
		}

		if (hero.hp <= 0) {
			cout << "\n" << hero.name << " has fallen in battle...\n";
			cout << "The Campaign has Ended...\n";
			cout << "Returning to the Main Menu\n";

			return CampaignResult::Defeat;
		}
		else if (enemy.stats.hp <= 0) {
			cout << "\n" << enemy.stats.name << " is defeated!\n";
		}

		return CampaignResult::Victory;
	}


// -- Enemy Abilities --

	void handleEnemyAbilities(Enemy& enemy, Player& hero, roundOutcome result, bool& usedBrittleBones, bool& usedCorporeal,
							int& thickSkinHitsLeft, int& heroCurseRounds, bool& bossBuffApplied) {

		// --- Boss opening ritual (BossBuff) applied once at start ---
		if (!bossBuffApplied && enemy.a1 == EnemyAbility::Opening) {
			cout << "Halut begins the battle with a dark ritual, empowering himself!\n";
			enemy.stats.maxHP += 10;
			enemy.stats.hp += 10;
			if (enemy.stats.hp > enemy.stats.maxHP) enemy.stats.hp = enemy.stats.maxHP;
			enemy.stats.attack += 2;
			cout << enemy.stats.name << " gains +10 HP and +2 ATK!\n";
			bossBuffApplied = true;
		}

		// --- Skeleton: Brittle Bones (one-time) ---
		if (enemy.a1 == EnemyAbility::BrittleBones &&
			!usedBrittleBones &&
			result == roundOutcome::HeroWin &&
			enemy.stats.hp > 0 && hero.hp > 0) {

			cout << enemy.stats.name << "'s brittle bones crack, it's broken bones hurt\n";
			enemy.stats.attack += 3;
			enemy.stats.hp -= 5;
			if (enemy.stats.hp < 1) enemy.stats.hp = 1;
			cout << enemy.stats.name << " gains +3 ATK but loses 5 HP. "
				<< "ATK: " << enemy.stats.attack
				<< ", HP: " << enemy.stats.hp << "/" << enemy.stats.maxHP << "\n";

			usedBrittleBones = true;
		}

		// --- Zombie: Thick Skin (next hit -3 damage) ---
		if (enemy.a1 == EnemyAbility::ThickSkin &&
			thickSkinHitsLeft == 0 &&
			result == roundOutcome::HeroWin &&
			enemy.stats.hp > 0 && hero.hp > 0) {

			int chance = randomInt(0, 99);
			if (chance < 40) {
				cout << enemy.stats.name << "'s skin hardens, reducing the next blow!\n";
				thickSkinHitsLeft = 1;
			}
		}

		// --- Ghost: Corporeal (one-time HP+5, maxHP+5, -3 ATK) ---
		if (enemy.a1 == EnemyAbility::Corporeal &&
			!usedCorporeal &&
			enemy.stats.hp > 0 &&
			enemy.stats.hp <= enemy.stats.maxHP / 2) {

			cout << enemy.stats.name << " begins to take on a more solid form...\n";
			enemy.stats.maxHP += 5;
			enemy.stats.hp += 5;
			if (enemy.stats.hp > enemy.stats.maxHP) enemy.stats.hp = enemy.stats.maxHP;
			enemy.stats.attack -= 3;
			if (enemy.stats.attack < 0) enemy.stats.attack = 0;

			cout << enemy.stats.name << " becomes corporeal! +5 max HP, +5 HP, -3 ATK.\n";
			cout << "HP: " << enemy.stats.hp << "/" << enemy.stats.maxHP
				<< ", ATK: " << enemy.stats.attack << "\n";

			usedCorporeal = true;
		}

		// --- Boss second ability: BossCurse (-3 ATK for hero for 2 rounds) ---
		if (enemy.a2 == EnemyAbility::CurseWeakness &&
			result == roundOutcome::EnemyWin &&
			hero.hp > 0 &&
			heroCurseRounds == 0) {

			int chance = randomInt(0, 99);
			if (chance < 50) {
				cout << enemy.stats.name << " utters a dark curse! Your strength falters!\n";
				heroCurseRounds = 2;
			}
		}
	}


// -- Enemys Possible --

	Enemy createEnemyForStage(int stage) {
		Enemy e;

		if (stage == 8) {
			e.stats.name = "Necromancer Halut";
			e.stats.mark = 'O';
			e.stats.maxHP = 55;
			e.stats.hp = 55;
			e.stats.attack = 12;
			e.stats.defense = 4;

			//Special Abilities
			e.a1 = EnemyAbility::Opening;
			e.a2 = EnemyAbility::CurseWeakness;

			return e;
		}

		int roll;
		do {
			roll = randomInt(0, 2); //0 - Skeleton, 1 - Zombie, 2 - Ghost
		} while (roll == lastEnemyType && lastEnemyType != -1);

		lastEnemyType = roll;

		switch (roll) {
		case 0: {
			e.stats.name = "Skeleton";
			e.stats.mark = 'O';
			e.stats.maxHP = 30;
			e.stats.hp = 30;
			e.stats.attack = 7;
			e.stats.defense = 2;
			e.a1 = EnemyAbility::BrittleBones;
			break;
		}
		case 1: {
			e.stats.name = "Zombie";
			e.stats.mark = 'O';
			e.stats.maxHP = 28;
			e.stats.hp = 28;
			e.stats.attack = 8;
			e.stats.defense = 3;
			e.a1 = EnemyAbility::ThickSkin;
			break;
		}
		case 2:
		default: {
			e.stats.name = "Ghost";
			e.stats.mark = 'O';
			e.stats.maxHP = 12;
			e.stats.hp = 12;
			e.stats.attack = 11;
			e.stats.defense = 1;
			e.a1 = EnemyAbility::Corporeal;
			break;
		}
		}
		
		int hpBonus = max(0, stage - 1) * 2;
		int atkBonus = max(0, stage - 1) / 2;

		e.stats.maxHP += hpBonus;
		e.stats.hp += hpBonus;
		e.stats.attack += atkBonus;

		return e;
	}


// -- Event Gen --

	void randomEvent() {
		while (true) {
			cout << "\nChoose Path:\n"
				<< " Wandered  (Less Dangerous, Less Rewards)\n"
				<< " Wilderness  (More Danergous, More Rewards)\n"
				<< "Enter Path: ";

			string choice;
			if (!getline(cin, choice)) {
				cout << "\nInvalid closed. Exiting.\n";
				exit(0);
			}

			string lower;
			for (char c : choice) {
				lower += static_cast<char>(tolower(static_cast<unsigned char>(c)));
			}

			if (lower == "wandered") {
				legendWandered++;
				randomEventWandered();
				break;
			}
			else if (lower == "wilderness") {
				legendWilderness++;
				randomEventWilderness();
				break;
			}
			else {
				cout << "Please enter either: Wandered or Wilderness.\n";
			}
		}
	}

	void randomEventWandered() {
		cout << "\n --- A Random Wandered Event Occurs! ---\n";
		int roll = randomInt(0, 3);

		switch (roll) {
		case 0:
			eventHealingFountain();
			break;
		case 1:
			eventTrainingGrounds();
			break;
		case 2:
			eventChurch();
			break;
		case 3:
			eventNothing();
			break;
		}
	}

	void randomEventWilderness() {
		cout << "\n --- A Random Wilderness Event Occurs! ---\n";
		int roll = randomInt(0, 4);

		switch (roll) {
		case 0:
			eventShimmeringLake();
			break;
		case 1:
			eventWildTraining();
			break;
		case 2:
			eventMysteriousShrine();
			break;
		case 3:
			eventNothing();
			break;
		case 4:
			eventAnimalAtk();
			break;
		}
	}


// -- Events Possible --

	void eventTrainingGrounds() {
		cout << hero.name << " visits a quiet training yard.\n";
		hero.attack += 1;
		cout << "Attack increases by 1. ATK is now " << hero.attack << ".\n";
	}

	void eventChurch() {
		cout << hero.name << " finds a small church where a priest offers a blessing.\n";
		int heal = randomInt(5, 10);
		int oldHP = hero.hp;
		hero.hp += heal;
		if (hero.hp > hero.maxHP) hero.hp = hero.maxHP;
		cout << "You recover " << (hero.hp - oldHP) << " HP.\n";
	}

	void eventNothing() {
		cout << hero.name << " wanders for a while, but nothing remarkable happens.\n";
	}

	void eventShimmeringLake() {
		cout << hero.name << " discovers a shimmering lake in the wilderness.\n";
		cout << "(You can make this a stronger heal or buff later.)\n";
	}

	void eventWildTraining() {
		cout << hero.name << " trains alone in the wild, pushing body and mind.\n";
		hero.attack += 1;
		hero.defense += 1;
		cout << "ATK +1, DEF +1. ATK: " << hero.attack << ", DEF: " << hero.defense << "\n";
	}

	void eventAnimalAtk() {
		cout << hero.name << " is ambushed by wild beasts!\n";
		int dmg = randomInt(3, 8);
		hero.hp -= dmg;
		if (hero.hp < 0) hero.hp = 0;
		cout << "You take " << dmg << " damage. HP: " << hero.hp << "/" << hero.maxHP << "\n";
	}

	void eventHealingFountain() {
		cout << "\n" << hero.name << " discovers a glowing healing fountain.\n";
		cout << "Clear water pulses with magical warmth...\n";

		if (hero.hp == hero.maxHP) {
			cout << "You already feel fully restored. The water has no effect.\n";
			return;
		}

		hero.hp = hero.maxHP;
		cout << "You drink from the fountain and are fully healed!\n";
		cout << "HP restored to " << hero.hp << "/" << hero.maxHP << ".\n";
	}

	void eventMysteriousShrine() {
		cout << "\n" << hero.name << " approaches a dark shrine glowing with eerie light...\n";
		cout << "An ancient voice whispers: \"Power... for a price.\" \n";
		cout << "Do you touch the altar? (y/n): ";

		string input;
		if (!getline(cin, input)) {
			cout << "\nInput closed. Exiting.\n";
			exit(0);
		}

		bool yes = !input.empty() && (input[0] == 'y' || input[0] == 'Y');

		if (!yes) {
			cout << "You step away, unwilling to risk your fate.\n";
			return;
		}

		cout << "A surge of energy flows through the altar...\n";

		int outcome = randomInt(0, 1); // 0 = Blessing, 1 = Curse

		if (outcome == 0) {
			cout << "A HOLY LIGHT bursts forth!\n";
			cout << "The shrine grants you a Holy Sword!\n";
			cout << "Attack increases by 5!\n";

			hero.attack += 5;
			cout << "ATK is now " << hero.attack << ".\n";
		}
		else {
			cout << "A DARK CURSE grips your soul...\n";
			cout << "Your life force is drained!\n";

			hero.maxHP -= 10;
			if (hero.maxHP < 1) hero.maxHP = 1;

			if (hero.hp > hero.maxHP) hero.hp = hero.maxHP;

			cout << "Max HP is reduced by 10! (Now " << hero.maxHP << ")\n";
			cout << "Current HP: " << hero.hp << "/" << hero.maxHP << "\n";
		}
	}


// -- Stat Calculations & Game --

	int calculateDamage(int attack, int defense) {
		int raw = attack - defense;
		if (raw < 1) {
			raw = 1; //Minimum Damage
		}
		return raw;
	}

	void applyDamage(Player& target, int damage) {
		target.hp -= damage;
		if (target.hp < 0) {
			target.hp = 0;
		}
	}

	void printDamage(const string& attackerName, const string& defenderName, int damage, int defenderHP, int defenderMAXHP) {
		cout << attackerName << " deals " << damage << " damage to " << defenderName << "! (HP: " << defenderHP << "/" << defenderMAXHP << ")\n";
	}


// -- Round Outcome --

	roundOutcome playOneBoard(Player& heroPlayer, Player& enemyPlayer) {
		Board board;
		board.clearBoard();

		bool heroTurn = true;

		while (true) {
			board.printBoard();

			if (heroTurn) {
				int idx = promptMove(board, heroPlayer.mark, heroPlayer.name);
				board.set(idx, heroPlayer.mark);

			}
			else {
				int idx = randomEmptyCell(board);
				if (idx == -1) {
					break; //No Moves Left
				}
				cout << enemyPlayer.name << " choose cell " << (idx + 1) << ".\n";
				board.set(idx, enemyPlayer.mark);
			}

			char w = board.winner();
			if (w == heroPlayer.mark) {
				board.printBoard();
				cout << w << " won the battle!\n";
				return roundOutcome::HeroWin;
			}
			else if (w == enemyPlayer.mark) {
				board.printBoard();
				cout << w << " won the battle!\n";
				return roundOutcome::EnemyWin;
			}
			else if (board.isFull()) {
				board.printBoard();
				cout << "This round ends in a Tie!\n";
				return roundOutcome::Tie;
			}

			heroTurn = !heroTurn; //Swaps Turns
		}

		return roundOutcome::Tie;
	}


// -- Saving / Loading --

	void saveGame() {
		ofstream out("campaign_save.txt");
		if (!out) {
			cout << "(Warning: could not open save file for writing.)\n";
			return;
		}

		out << hero.name << "\n";
		out << static_cast<int>(hero.archetype) << " " << hero.mark << "\n";
		out << hero.hp << " " << hero.maxHP << " "
			<< hero.attack << " " << hero.defense << "\n";
		out << stage << "\n";
		out << legendWandered << " " << legendWilderness << "\n";
	}

	bool loadGame() {
		ifstream in("campaign_save.txt");
		if (!in) {
			return false;  // no save file
		}

		string nameLine;
		if (!getline(in, nameLine)) {
			return false;
		}
		hero.name = nameLine;

		int archInt;
		in >> archInt >> hero.mark;
		in >> hero.hp >> hero.maxHP >> hero.attack >> hero.defense;
		in >> stage;
		in >> legendWandered >> legendWilderness;

		if (!in) {
			return false; // read error / corrupt
		}

		// Converts back
		switch (archInt) {
		case 1: hero.archetype = Archetype::Alchemist; break;
		case 2: hero.archetype = Archetype::Paladin; break;
		default: hero.archetype = Archetype::None; break;
		}

		// Safety Checks
		if (hero.maxHP < 1) hero.maxHP = 1;
		if (hero.hp < 0) hero.hp = 0;
		if (hero.hp > hero.maxHP) hero.hp = hero.maxHP;
		if (stage < 0) stage = 0;
		if (legendWandered < 0) legendWandered = 0;
		if (legendWilderness < 0) legendWilderness = 0;


		in.ignore(numeric_limits<streamsize>::max(), '\n');

		return true;
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
			<< "3 for Campaign Tic Tac Toe\n"
			<< "4 to Quit\n"
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
			case 3: {
				cout << "\nCampaign Tic Tac Toe Chosen:\n";
				CampaignGame game;
				game.run();
				break;
			}
			case 4:
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