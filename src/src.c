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

#define RESET		 "\x1b[0m"
#define RED			 "\x1b[31m"
#define GREEN		 "\x1b[32m"
#define YELLOW		 "\x1b[33m"
#define BLUE		 "\x1b[34m"
#define MAGENTA		 "\x1b[35m"
#define CYAN		 "\x1b[36m"
#define WHITE		 "\x1b[37m"
#define BOLD_RED	 "\x1b[1;31m"
#define BOLD_GREEN	 "\x1b[1;32m"
#define BOLD_YELLOW	 "\x1b[1;33m"
#define BOLD_BLUE	 "\x1b[1;34m"
#define BOLD_MAGENTA "\x1b[1;35m"
#define BOLD_CYAN	 "\x1b[1;36m"
#define BOLD_WHITE	 "\x1b[1;37m"

typedef char board[ROWS][COLS];
enum WINNER { PLAYER, AI, NONE, INVALID };
struct minimax_eval {
	int column;
	int score;
};

void print_board(board b) {
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
	char sym = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	for (int i = ROWS; i >= 0; i--) {
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

	return;
}

/*
 *	- - -
 *	- - -
 *	+ + +
 */
bool horizontal_win_check(board b, int ri, int ci, char symbol) {
	int count = 0;

	if (ci + WIN_COUNT - 1 >= COLS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri][ci + i] == symbol)
			count++;

	return count == WIN_COUNT;
}

/*
 *	- + -
 *	- + -
 *	- + -
 */
bool vertical_win_check(board b, int ri, int ci, char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci] == symbol)
			count++;

	return count == WIN_COUNT;
}

/*
 *	+ - -
 *	- + -
 *	- - +
 */
bool diagonal1_win_check(board b, int ri, int ci, char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci + WIN_COUNT - 1 >= COLS)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci + i] == symbol)
			count++;

	return count == WIN_COUNT;
}

/*
 *	- - +
 *	- + -
 *	+ - -
 */
bool diagonal2_win_check(board b, int ri, int ci, char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci - WIN_COUNT + 1 < 0)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci - i] == symbol)
			count++;

	return count == WIN_COUNT;
}

bool is_winner(board b, bool playerturn) {
	char sym = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;

	/* TODO(?): multithread */

	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (horizontal_win_check(b, i, j, sym) ||
				vertical_win_check(b, i, j, sym) ||
				diagonal1_win_check(b, i, j, sym) ||
				diagonal2_win_check(b, i, j, sym))
				return true;

	return false;
}

bool is_draw(board b) {
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (b[i][j] == EMPTY_SYMBOL)
				return false;
	return true;
}

int evaluate_heuristic(board b, bool playerturn) {
	int score = 0;

	/* TODO: Double check return values */

	if (is_winner(b, playerturn))
		return INT_MAX;

	if (is_winner(b, !playerturn))
		return INT_MIN;

	/*
	 * CHECK: What other heuristics could I keep?
	 *
	 * 1. My longest connecting sequence is longer than opponent's? Obvious.
	 * 2. Prefer more pieces in the center? Makes sense as more paths possible.
	 * 3. Just stopped opponent from connecting 4? Ideal but how to check this?
	 * 4. My longest connecting sequence is accessible from both ends?
	 */

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
	case PLAYER: printf("Player wins!\n");
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
