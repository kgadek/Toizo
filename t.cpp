// vim: tabstop=4:shiftwidth=4:cindent:nu

#include <cstdio>
#include <cstdlib>
#include "set.h"

typedef unsigned int uint;
typedef unsigned char uchar;

struct node : public set<node> {
	char type; /**< Typ obiektu. */
	char rot; /**< Obrót obiektu o wielokrotność \f$90^\circ\f$ "clockwise". */
};
char rots[] = {1,2,4,1,2,4,4,4,4}; /**< Ilość obrotów do rozważenia dla kolejnych typów klocków. */


uint X, /**< Ilość kolumn. */
	 Y, /**< Ilość wierszy. */
	 XY; /**< Taki mały helper; XY = X*Y. */
uint batt_pos; /**< Pozycja baterii. */

node **board; /**< Reprezentacja planszy. */
node *brd; /**< brd = (*board). */


void read_board();
void print_board();


/** Główna metoda.
 */
int main() {
	read_board();
	print_board();
	return 0;
}


/** Wczytujemy wierszami, maks. 1500 znaków na raz.
 * Mały ale ważny komentarz: zakładamy, że na wejściu nie ma zbędnych białych znaków.
 */
#define INPUTLEN 1500

/** Wczytywanie planszy.
 * Alokuje reprezentację; ustawia zmienne ::X, ::Y i ::XY. Znajduje baterię (ustala ::batt_pos).
 */
void read_board() {
	char input[INPUTLEN];
	scanf(" %d %d ", &X, &Y);
	XY = X*Y;

	// Podwójna notacja: board[x][y] <==> brd[i] dla i == X*y + x
	board = (node**)malloc(Y*sizeof(node*));
	brd = (*board) = (node*)malloc(XY*sizeof(node));
	for(uint i=0;i<Y;++i) board[i] = *board + i*X;

	uint board_pos = 0;
	char *inp;
	while(fgets(input,INPUTLEN,stdin) != NULL) {
		for(inp=input; *inp; inp+=3, ++board_pos) { // ustawianie .type .rot
			if((*inp) == 'H' || ('D' <= (*inp) && (*inp) <= 'F'))
				batt_pos = board_pos;
			brd[board_pos].type = (*inp) - 'A';
			brd[board_pos].rot = 0; // nie wczytujemy początkowego obrotu elementu bo... po co?
		}
	}
}


/** Wyświetlanie planszy.
 * Nic ciekawego tu się nie dzieje.
 */
void print_board() {
	printf("%d %d\n", X,Y);
	uint x, y;
	node *b = brd;
	for(y=0;y<Y;++y) {
		printf("%c%d ", (*b).type + 'A', (*++b).rot  + 1);
		for(x=1;x<X;++x,++b)
			printf("%c%d ", (*b).type + 'A', (*b).rot  + 1);
		printf("\n");
	}
}


