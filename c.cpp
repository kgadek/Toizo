/*********************************************************************
** 
** To begin with: english is often easier to use in programming :)
** 
*********************************************************************/
#include <cstdio>



/*******************************
  Struktury danych
*******************************/
template <class T>	// homogeniczna lista, przyda się...
struct lst {		// ok, tak naprawdę to nie jest lista
					// tylko element listy
	lst() : tl(NULL) {}
	lst(const T &_hd, lst<T> * const _tl = NULL) : hd(_hd), tl(_tl) {}
	lst(lst<T> *r) {
		hd = r->hd;
		lst<T> **t = &(r->tl);
		for(r=r->tl; r != NULL; t = &((*t)->tl), r=r->tl)
			(*t) = new lst<T>(r->hd);
	}
	static void del(lst<T> *p) {	// ~lst jest tutaj zbyt mocne,
									// wolimy deafaultowe zachowanie
									// dzięki czemu nie mamy stack-eatera
		for(lst<T> *n = p->tl; p != NULL; p=n, n=n->tl)
			delete p;
	}
	static lst<T>* range(T A, const T B) {
		if(A > B)
			return NULL;
		lst<T> *head = new lst<T>(A), **t = &(head->tl);
		for(++A; A < B; ++A, t = &((*t)->tl))
			*t = new lst<T>(A);
		return head;
	}
	T hd;
	lst<T> *tl;
};

struct node {
	char type;			// mem: XY*4
	lst<char> *rots;	// mem: XY*4*8
	char rot;			// mem: XY*4
} **board;



/*******************************
  Parametry gry
*******************************/
lst<char> rots[] = {
	lst<char>::range(0,1), // A
	lst<char>::range(0,2),
	lst<char>::range(0,4), // C
	lst<char>::range(0,1),
	lst<char>::range(0,2), // E
	lst<char>::range(0,4),
	lst<char>::range(0,4), // G
	lst<char>::range(0,4),
	lst<char>::range(0,4)  // I
};



/*******************************
  Zmienne globalne
*******************************/
int X, Y, XY; //rozmiary oraz iloczyn X*Y


/*******************************
  Deklaracje funkcji
*******************************/
int main() {
	// wczytywanie planszy
	scanf(" %d %d ", &X, &Y);
	printf("%d %d\n", X, Y); // od razu wypisujemy -- why not?
	XY = X * Y;

	board = new node*[Y];	// alokacja przy pomocy dwóch new ^.^
	(*board) = new node[XY];
	for(int i=1,j=X; i<Y; ++i, j+=X)
		board[i] = *board+j;

	char *input = new char[3*X],	// bufor wejściowy
		 inp;
	int boardPos = 0,	// pozycja w tablicy board[0][]
		inpPos = 0,		// pozycja w buforze wejściowym
		X3 = 3*X,		// nie ufamy kompilatorom, oj nie.
		posBat;			// pozycja baterii
	while( fgets(input, X3+1, stdin) != NULL ) {
			// ^-- fgets jest szybszy niż scanf
		for(inpPos = 0; inpPos < X3; inpPos += 3, ++boardPos) {
			inp = input[inpPos];
			if(inp == 'H' || ('D' <= inp && inp <= 'F'))
				posBat = boardPos;
			board[0][boardPos].type = inp - 'A';
			/*dbg*/printf("[%d] = %c\n", boardPos, board[0][boardPos].type + 'A');
		}
	}

	// wypisywanie wyniku
	int yy, xx;
	for(yy = 0; yy < Y; ++yy) {
		printf("%c%d", board[yy][0].type+'A', board[yy][0].rot+1);
		for(xx = 1; xx < X; ++xx)
			printf(" %c%d", board[yy][xx].type+'A', board[yy][xx].rot+1);
		printf("\n");
	}
	return 0;
}

