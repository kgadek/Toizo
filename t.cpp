// vim: tabstop=4:shiftwidth=4:cindent:nu

#include <cstdio>
#include <cstdlib>

typedef unsigned int uint;
typedef unsigned char uchar;

struct node {
	char type; //!< Typ obiektu: A=0, B=1, ...
	char rot; //!< Obrót obiektu: 0 = bez obrotu, 1 = obrót o 90 stopni ("clockwise").
	node* p; //!< Dla union-findSet: pointer na ojca.
	uint s; //!< Dla union-findSet: rozmiar poddrzewa.
};
char rotations[] = {1,2,4,1,2,4,4,4,4}; //!< Ilość obrotów kolejnych typów klocków (np. 'A' jest symetryczny, ma jeden wartościowy obrót).


uint X, //!< Ilość kolumn.
	 Y, //!< Ilość wierszy.
	 XY; //!< Taki mały helper; XY = X*Y.
uint batt_pos; //!< Pozycja baterii.

node **board; //!< Reprezentacja planszy.
node *brd; //!< brd = (*board).


void read_board();
void print_board();

node** rollbackHeap;
uint rollbackHeapHead = 0;
node* findSet(node*);
void commitOrRollback(const bool);
void unionSet(node* const, node* const);

/**
 * Główna metoda.
 */
int main() {
	read_board();
	print_board();

	// nie ma delete, OS po nas posprząta
	return 0;
}


/**
 * Wczytujemy wierszami, maks. 1500 znaków na raz.
 *
 * Mały ale ważny komentarz: zakładamy, że na wejściu nie ma zbędnych białych znaków.
 */
#define INPUTLEN 1500

/**
 * Wczytywanie planszy.
 *
 * Alokuje reprezentację; ustawia zmienne X,Y i XY; znajduje baterię.
 */
void read_board() {

	char input[INPUTLEN];
	scanf(" %d %d ", &X, &Y);
	XY = X*Y;
	rollbackHeap = (node**)malloc(2*XY*sizeof(node*));

	// board[x][y] == (*board)[i] == brd[i]   dla   i == X*y + x
	board = (node**)malloc(Y*sizeof(node*));
	brd = (*board) = (node*)malloc(XY*sizeof(node));
	for(uint i=0;i<Y;++i) board[i] = *board + i*X;
	for(uint i=0;i<XY;++i, ++brd) { // ustawianie .p .s
		(*brd).p = brd;
		(*brd).s = 1;
	}
	brd = (*board);

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


/**
 * Wyświetlanie planszy.
 *
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


/**
 * Wyszukiwanie reprezentanta zbioru.
 *
 * Używamy heurezy łączenia wg rozmiarów, więc koszt czasowy to O(log n).
 */
node* findSet(node *n) {
	while(n->p != n) n = n->p;
	return n;
}


/**
 * Commit na zbiorach.  
 */
void commitSet() {
	if(rollbackHeapHead == 0 || rollbackHeap[rollbackHeapHead-1] != 0)
		rollbackHeap[rollbackHeapHead++] = 0;
}


/**
 * Rollback do ostatniego commita (na zbiorach).
 */
void rollbackSet() {
	if(rollbackHeap[rollbackHeapHead-1] == 0 && rollbackHeapHead > 0)
		--rollbackHeapHead;
	while(rollbackHeapHead > 0 && rollbackHeap[--rollbackHeapHead] != 0) {
		node *n = rollbackHeap[rollbackHeapHead];
		n->p->s -= n->s;
		n->p = n;
	}
}


/**
 * Operacja union. 
 */
void unionSet(node* n, node* m) {
	if(findSet(n) == findSet(m)) return;
	if(n->s < m->s) {
		n->p = m;
		m->s += n->s;
		rollbackHeap[rollbackHeapHead++] = n;
	} else {
		m->p = n;
		n->s += m->s;
		rollbackHeap[rollbackHeapHead++] = m;
	}
}

