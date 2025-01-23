#include <limits.h>
#include <stdio.h>
#include <string.h>

#define true  1
#define false 0
typedef unsigned char bool;

#define ROWS	  6
#define COLS	  7
#define WIN_COUNT 4

#define EMPTY_SYMBOL  '-'
#define PLAYER_SYMBOL '+'
#define AI_SYMBOL	  'o'

#define MAX_DEPTH 6

#define RESET	"\x1b[0m"
#define RED		"\x1b[31m"
#define GREEN	"\x1b[32m"
#define YELLOW	"\x1b[33m"
#define BLUE	"\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN	"\x1b[36m"
#define WHITE	"\x1b[37m"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef char board[ROWS][COLS];
enum WINNER { PLAYER, AI, NONE, INVALID };

struct minimax_eval {
	int column;
	int score;
};

struct chain {
	int length;
	enum orientation { VERT, HORIZ, DIAG1, DIAG2 } orient;
};

#include <stdlib.h>
void print_board(const board b) {
	system("clear");
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			printf("%c ", b[i][j]);
		}
		printf("\n");
	}
}

int get_column_from_user() {
	int usercol = 0, x;
	unsigned int ctr;

	do {
		ctr = 0;
		printf("Select a column [1 - 7]: ");
		usercol = getchar();

		/* TODO: check for EOF */

		while ((x = getchar()) != '\n' && x != EOF)
			ctr++;

		usercol -= '0';
	} while (ctr > 0 || usercol < 1 || usercol > COLS);

	return usercol - 1;
}

void place_piece(board b, int column, bool playerturn) {
	const char sym = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	for (int i = ROWS - 1; i >= 0; i--) {
		if (b[i][column] == EMPTY_SYMBOL) {
			b[i][column] = sym;
			return;
		}
	}
}

void remove_piece(board b, int column) {
	for (int i = 0; i < ROWS; i++) {
		if (b[i][column] != EMPTY_SYMBOL) {
			b[i][column] = EMPTY_SYMBOL;
			return;
		}
	}
}

void player_turn(board b) {
	int usercol = 0;

	do {
		usercol = get_column_from_user();
	} while (b[0][usercol] != EMPTY_SYMBOL); /* is full column */

	place_piece(b, usercol, true);
}

/*
 *	- - -
 *	- - -
 *	+ + +
 */
int horizontal_chain_length(const board b, const int ri, const int ci,
							const char symbol) {
	int count = 0;

	if (ci + WIN_COUNT - 1 >= COLS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri][ci + i] == symbol)
			count++;

	return count;
}

/*
 *	- + -
 *	- + -
 *	- + -
 */
int vertical_chain_length(const board b, const int ri, const int ci,
						  const char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci] == symbol)
			count++;

	return count;
}

/*
 *	+ - -
 *	- + -
 *	- - +
 */
int diagonal1_chain_length(const board b, const int ri, const int ci,
						   const char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci + WIN_COUNT - 1 >= COLS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci + i] == symbol)
			count++;

	return count;
}

/*
 *	- - +
 *	- + -
 *	+ - -
 */
int diagonal2_chain_length(const board b, const int ri, const int ci,
						   const char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci - WIN_COUNT + 1 < 0)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci - i] == symbol)
			count++;

	return count;
}

struct chain longest_chain_from_point(const board b, const int ri, const int ci,
									  bool playerturn) {
	const char sym	 = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	struct chain ret = {INT_MIN, 0};

	int w = horizontal_chain_length(b, ri, ci, sym);
	int x = vertical_chain_length(b, ri, ci, sym);
	int y = diagonal1_chain_length(b, ri, ci, sym);
	int z = diagonal2_chain_length(b, ri, ci, sym);

	ret = (w > x) ? (struct chain){w, HORIZ} : (struct chain){x, VERT};

	return ret;
}

int is_winner(const board b, const bool playerturn) {
	/* TODO(?): multithread */

	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (longest_chain_from_point(b, i, j, playerturn).length ==
				WIN_COUNT)
				return true;

	return false;
}

int is_draw(const board b) {
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (b[i][j] == EMPTY_SYMBOL)
				return false;
	return true;
}

/*
 * @brief returns a score for a user based on their longest chain so far, with
 * additional points if their chain is open from each end
 */
int determine_sequence_score(const board b, const bool playerturn) {
	const char sym = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	int score = 0, ri = 0, ci = 0;
	struct chain longest = {INT_MIN, 0};

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			struct chain curr = longest_chain_from_point(b, i, j, playerturn);
			if (curr.length > longest.length) {
				longest = curr;
				ri		= i;
				ci		= j;
			}
		}
	}

	/* TODO */
	if (longest.length == 3) {
		switch (longest.orient) {
		case HORIZ:
			break;
		case VERT:
			break;
		case DIAG1:
			break;
		case DIAG2:
			break;
		}
	}

	return score * 10;
}

/*
 * @brief returns a score for a user based on where their pieces are on the
 * board; pieces towards the center are worth more
 */
int piece_location_score(const board b, const bool playerturn) {
	const int center = COLS / 2;
	const char sym	 = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	int score		 = 0;

	/* Piece Value: +0 +1 +2 +3 +2 +1 +0 */
	/* Columns:      0  1  2  3  4  5  6 */

	for (int i = center - 2; i <= center + 2; i++) {
		for (int j = ROWS - 1; b[j][i] != EMPTY_SYMBOL || j >= 0; j--) {
			if (b[j][i] == sym) {
				switch (i) {
				case 3:
					score += 1;
					/* FALLTHRU */
				case 2:
				case 4:
					score += 1;
					/* FALLTHRU */
				case 1:
				case 5:
					score += 1;
				}
			}
		}
	}

	return score;
}

/*
 * @brief evaluates the 'score' of a board w.r.t the player and the AI -- the
 * player's score would be increasingly positive, and the AI's would be
 * increasingly negative.
 */
int evaluate_heuristic(const board b) {
	int score = 0;

	if (is_winner(b, true))
		return INT_MAX;

	if (is_winner(b, false))
		return INT_MIN;

	score += determine_sequence_score(b, true);
	score -= determine_sequence_score(b, false);

	score += piece_location_score(b, true);
	score -= piece_location_score(b, false);

	return score;
}

struct minimax_eval minimax(board b, int depth, int alpha, int beta,
							bool playerturn) {
	struct minimax_eval ret = {0, 0};

	if (depth == MAX_DEPTH)
		return ret;

	for (int i = 0; i < COLS; i++) {
		if (b[0][i] != EMPTY_SYMBOL) /* is full column */
			continue;

		place_piece(b, i, false);

		/* recursive call/heuristic determination here */

		remove_piece(b, i);

		/* some other shit here? Updating alpha/beta if needed, I think */
	}

	return ret;
}

void ai_turn(board b) {
	int alpha	 = INT_MIN;
	int beta	 = INT_MAX;
	int aicolumn = minimax(b, 0, alpha, beta, false).column;
	place_piece(b, aicolumn, false);
}

int main() {
	board b;
	enum WINNER w = INVALID;

	memset(b, EMPTY_SYMBOL, ROWS * COLS);

	for (bool playerturn = true; w == INVALID; playerturn = !playerturn) {
		print_board(b);
		playerturn ? player_turn(b) : ai_turn(b);
		if (is_draw(b))
			w = NONE;
		else if (is_winner(b, playerturn))
			w = playerturn ? PLAYER : AI;
	}

	print_board(b);

	switch (w) {
	case PLAYER:
		printf("Player wins!\n");
		break;
	case AI:
		printf("AI wins!\n");
		break;
	case NONE:
		printf("Draw!\n");
		break;
	case INVALID:
		break;
	}

	return 0;
}
