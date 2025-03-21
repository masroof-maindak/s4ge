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

#define RESET "\x1b[0m"
#define RED	  "\x1b[31m"
#define BLUE  "\x1b[34m"
#define BOLD  "\x1b[1m"
#define CLEAR "\x1b[2J\x1b[H"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef char board[ROWS][COLS];
enum WINNER { PLAYER, AI, DRAW, INVALID };

struct mm_res {
	int column;
	int score;
};

struct chain {
	int length;
	enum orientation { VERT = 0, HORIZ, DIAG1, DIAG2 } orient;
};

void print_board(board b) {
	printf(CLEAR);
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			char *clr = RESET;
			if (b[i][j] == PLAYER_SYMBOL)
				clr = BOLD BLUE;
			else if (b[i][j] == AI_SYMBOL)
				clr = BOLD RED;
			printf("%s%c " RESET, clr, b[i][j]);
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
 *	x + +
 */
int horizontal_chain_length(board b, const int ri, const int ci,
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
 *	- x -
 *	- + -
 *	- + -
 */
int vertical_chain_length(board b, const int ri, const int ci,
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
 *	x - -
 *	- + -
 *	- - +
 */
int diagonal1_chain_length(board b, const int ri, const int ci,
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
 *	- - x
 *	- + -
 *	+ - -
 */
int diagonal2_chain_length(board b, const int ri, const int ci,
						   const char symbol) {
	int count = 0;

	if (ri + WIN_COUNT - 1 >= ROWS || ci - WIN_COUNT + 1 < 0)
		return false;

	for (int i = 0; i < WIN_COUNT; i++)
		if (b[ri + i][ci - i] == symbol)
			count++;

	return count;
}

struct chain longest_chain_from_point(board b, const int ri, const int ci,
									  bool playerturn) {
	const char sym	 = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	struct chain ret = {INT_MIN, 0};
	int len[4];

	len[0] = vertical_chain_length(b, ri, ci, sym);
	len[1] = horizontal_chain_length(b, ri, ci, sym);
	len[2] = diagonal1_chain_length(b, ri, ci, sym);
	len[3] = diagonal2_chain_length(b, ri, ci, sym);

	for (int i = 0; i < 4; i++)
		if (ret.length < len[i])
			ret = (struct chain){len[i], i};

	return ret;
}

bool is_winner(board b, const bool playerturn) {
	/* TODO(?): multithread */

	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			if (longest_chain_from_point(b, i, j, playerturn).length ==
				WIN_COUNT)
				return true;

	return false;
}

int is_draw(board b) {
	for (int i = 0; i < COLS; i++)
		if (b[0][i] == EMPTY_SYMBOL)
			return false;
	return true;
}

/* CHECK: why does adding the `inline` qualifier break this? */
bool is_valid_position(const int r1, const int c1, const int r2, const int c2) {
	return (r1 >= 0 && r1 < ROWS && c1 >= 0 && c1 < COLS && r2 >= 0 &&
			r2 < ROWS && c2 >= 0 && c2 < COLS);
}

/*
 * @brief returns a score for a user based on their longest chain so far, with
 * additional points if their chain is open from each end
 */
int compute_chain_score(board b, const bool playerturn) {
	struct chain longest = {INT_MIN, 0};
	bool open			 = false;
	int score = 0, r = 0, c = 0;

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			struct chain curr = longest_chain_from_point(b, i, j, playerturn);
			if (curr.length > longest.length) {
				longest = curr;
				r		= i;
				c		= j;
			}
		}
	}

	score = longest.length;
	score = score * score * 10;

	if (longest.length < 3)
		return score;

	switch (longest.orient) {
	case HORIZ:
		open = is_valid_position(r, c - 1, r, c + WIN_COUNT) &&
			   (b[r][c - 1] == EMPTY_SYMBOL &&
				b[r][c + WIN_COUNT] == EMPTY_SYMBOL);
		break;
	case VERT:
		open = is_valid_position(r - 1, c, r + WIN_COUNT, c) &&
			   (b[r - 1][c] == EMPTY_SYMBOL &&
				b[r + WIN_COUNT][c] == EMPTY_SYMBOL);
		break;
	case DIAG1:
		open = is_valid_position(r - 1, c - 1, r + WIN_COUNT, c + WIN_COUNT) &&
			   (b[r - 1][c - 1] == EMPTY_SYMBOL &&
				b[r + WIN_COUNT][c + WIN_COUNT] == EMPTY_SYMBOL);
		break;
	case DIAG2:
		open = is_valid_position(r - 1, c + 1, r + WIN_COUNT, c - WIN_COUNT) &&
			   (b[r - 1][c + 1] == EMPTY_SYMBOL &&
				b[r + WIN_COUNT][c - WIN_COUNT] == EMPTY_SYMBOL);
	}

	return open ? score + 1000 : score;
}

/*
 * @brief returns a score for a user based on where their pieces are on the
 * board; pieces towards the center are worth more
 */
int piece_location_score(board b, const bool playerturn) {
	const char sym	 = playerturn ? PLAYER_SYMBOL : AI_SYMBOL;
	int score		 = 0;

	const int piecevals[COLS] = {0, 1, 2, 3, 2, 1, 0};

	for (int i = 0; i < COLS; i++)
		for (int j = 0; j < COLS; j++)
			score += b[j][i] == sym ? piecevals[i] : 0;

	return score;
}

/*
 * @brief evaluates the 'score' of a board w.r.t the player and the AI --
 * the player's score would be increasingly positive, and the AI's would be
 * increasingly negative.
 */
int evaluate_heuristic(board b, enum WINNER w) {
	int score = 0;

	if (w == PLAYER)
		return INT_MAX;
	else if (w == AI)
		return INT_MIN;
	else if (w == DRAW)
		return score;

	score += compute_chain_score(b, true);
	score -= compute_chain_score(b, false) / 2;

	score += piece_location_score(b, true);
	score -= piece_location_score(b, false) / 2;

	return score;
}

/*
 * @brief determines if the game is over i.e minimax node has reached a
 * terminal state
 */
enum WINNER terminal_state_reached(board b) {
	if (is_draw(b))
		return DRAW;
	else if (is_winner(b, true))
		return PLAYER;
	else if (is_winner(b, false))
		return AI;
	else
		return INVALID;
}

struct mm_res alphabeta(board b, const int depth, int alpha, int beta,
						const bool playerturn) {
	enum WINNER w	   = terminal_state_reached(b);
	struct mm_res best = {.column = 5, .score = playerturn ? INT_MIN : INT_MAX};

	if (depth <= 0 || w != INVALID) {
		best.score = evaluate_heuristic(b, w);
		return best;
	}

	/* Central columns are favoured by the heuristic, and picking more
	 * likely choices first leads to greater odds of pruning occuring */

	const int goodcols[COLS] = {3, 2, 4, 1, 5, 0, 6};

	for (int i = 0; i < COLS && alpha < beta; i++) {
		int aicol = goodcols[i];

		if (b[0][aicol] != EMPTY_SYMBOL) /* is full column */
			continue;

		place_piece(b, aicol, playerturn);
		struct mm_res child = alphabeta(b, depth - 1, alpha, beta, !playerturn);
		child.column		= aicol;
		remove_piece(b, aicol);

		if (playerturn) {
			if (child.score > best.score)
				best = child;
			alpha = MAX(best.score, alpha);
		} else {
			if (child.score < best.score)
				best = child;
			beta = MIN(best.score, beta);
		}
	}

	return best;
}

void ai_turn(board b) {
	int aicolumn = alphabeta(b, MAX_DEPTH, INT_MIN, INT_MAX, false).column;
	place_piece(b, aicolumn, false);
}

int main() {
	board b;
	bool playerturn = true;
	enum WINNER w	= INVALID;

	memset(b, EMPTY_SYMBOL, ROWS * COLS);

	while (w == INVALID) {
		print_board(b);
		playerturn ? player_turn(b) : ai_turn(b);
		w		   = terminal_state_reached(b);
		playerturn = !playerturn;
	}

	print_board(b);

	if (w == DRAW)
		printf("Draw.\n");
	else
		printf("%s wins.\n", w == PLAYER ? "Player" : "AI");

	return 0;
}
