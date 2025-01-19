#include <stdio.h>
#include <string.h>

#include "../include/bool.h"

#define ROWS		  6
#define COLS		  7
#define WIN_COUNT	  4
#define EMPTY_SYMBOL  '-'
#define PLAYER_SYMBOL '+'
#define AI_SYMBOL	  'o'

enum WINNER { PLAYER, AI, NONE, INVALID };

void print_board(char b[][COLS]) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			printf("%c ", b[i][j]);
		}
		printf("\n");
	}
}

int get_column() {
	int usercol = 0, ctr = 0, x = 0;

	do {
		printf("Select a column [1 - 7]: ");
		usercol = getchar();

		/* TODO: check for EOF */

		while ((x = getchar()) != '\n' && x != EOF)
			ctr++;
		usercol -= '0';
	} while (usercol < 1 || usercol > 7);

	return usercol - 1;
}

void player_turn(char b[][COLS]) {
	int usercol = 0;

	/*
	 * TODO: refactor to 1. temporary placement 2. validation 3. proceed/goto1 ?
	 * This approach MIGHT be re-usable by the AI
	 */

	do {
		usercol = get_column();
	} while (b[0][usercol] != EMPTY_SYMBOL);

	for (int i = ROWS; i >= 0; i--) {
		if (b[i][usercol] == EMPTY_SYMBOL) {
			b[i][usercol] = PLAYER_SYMBOL;
			break;
		}
	}

	return;
}

/*
 *	- - -
 *	- - -
 *	+ + +
 */
bool horizontal_win_check(char b[][COLS], int ri, int ci, char symbol) {
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
bool vertical_win_check(char b[][COLS], int ri, int ci, char symbol) {
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
bool diagonal1_win_check(char b[][COLS], int ri, int ci, char symbol) {
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
bool diagonal2_win_check(char b[][COLS], int ri, int ci, char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci - WIN_COUNT + 1 < 0)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci - i] == symbol)
			count++;

	return count == WIN_COUNT;
}

bool check_if_winner(char b[][COLS], bool playerturn) {
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

bool is_draw(char b[][COLS]) {
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (b[i][j] == EMPTY_SYMBOL)
				return false;
	return true;
}

void ai_turn(char b[][COLS]) {}

int main() {
	char board[ROWS][COLS];
	enum WINNER w = INVALID;

	memset(board, EMPTY_SYMBOL, ROWS * COLS);

	for (bool playerturn = true; w == INVALID; playerturn = !playerturn) {
		print_board(board);
		playerturn ? player_turn(board) : ai_turn(board);
		if (is_draw(board))
			w = NONE;
		else if (check_if_winner(board, playerturn))
			w = playerturn ? PLAYER : AI;
	}

	print_board(board);

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
