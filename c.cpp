/*********************************************************************
** 
** To begin with: english is often easier to use in programming mais
** dans les commentaires jednak polski me gusta :)
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
	static int size(const lst<T> *p) {
		int s=0;
		for(; p != NULL; ++s, p=p->tl) ;
		return s;
	}
	static lst<T>* drop(lst<T> *l, const T v) {
		lst<T> **t = &l;
		for(; (*t) != NULL; t = &((*t)->tl))
			if((*t)->hd == v) {
				(*t) = (*t)->tl;
				return l;
			}
		return l;
	}
	T hd;
	lst<T> *tl;
};

struct node {
	char type;			// typ. Jeśli ustalono ułożenie to f(X) = -X-1 < 0
						// mem: XY*1
	lst<char> *rots;	// mem: XY*4*8
	char rot;			// mem: XY*1
	node *p;			// wskaźnik na ojca (algo. union-find)
						// mem: XY*4
	int rank;			// ilość node'ów w drzewie (algo. union-find)
						// mem: XY*4
	int bulbs;			// ilość żarówek w drzewie (algo. union-find) 
						// mem: XY*4
} **board;
						//----------
						// mem: XY*46 < 11MB



/*******************************
  Parametry gry
*******************************/
lst<char> rots[] = {
	*lst<char>::range(0,1), // A
	*lst<char>::range(0,2),
	*lst<char>::range(0,4), // C
	*lst<char>::range(0,1),
	*lst<char>::range(0,2), // E
	*lst<char>::range(0,4),
	*lst<char>::range(0,4), // G
	*lst<char>::range(0,4),
	*lst<char>::range(0,4)  // I
};



/*******************************
  Zmienne globalne
*******************************/
int X, Y, XY; //rozmiary oraz iloczyn X*Y



/*******************************
  Deklaracje funkcji
*******************************/
			// te dwie funkcje radzą sobie z
			// X=0 Y=pozycja
void takePossOut(int xx, int yy, int dd);
			// na pozycji X,Y uznajemy, że R jest
			// niefajnym kierunkiem i tym kierunku
			// nie będzie niczego. Damy znać sąsiadom.



/*******************************
  Main
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
			board[0][boardPos].p = *board+boardPos;
			board[0][boardPos].rank = 1;
			board[0][boardPos].bulbs = inp == 'I';
			// heureza #1: jeśli 'A' to ustalamy ułożenie od razu.
			// nie robimy tego dla 'D' dla prostoty implementacji
			if(inp - 'A' == 0) {
				board[0][boardPos].type = 'A' - inp - 1 ;
				board[0][boardPos].rot = 0;
				board[0][boardPos].rots = NULL;
			} else {
				board[0][boardPos].type = inp - 'A';
				board[0][boardPos].rot = 0;
				board[0][boardPos].rots = new lst<char>(rots[inp-'A']);
			}
		}
	}
	int yy, xx; 		// to do wypisywania wyników i debugowania

	
	// heureza #2: sytuacja na rogach
	// 'BD' ustalamy na 0; 'CFGH' ustalamy na numer rogu (NW, NE, SE, SW);
	// dla 'I' redukujemy opcje do X i X+1, X-numer rogu;
	for(int i=0,j=0;i<4;++i) {
								// i - numer rogu
								// j - pozycja w board[0][]
		switch(i) {
			case 1: j=X-1; break;
			case 2: j=XY-1; break;
			case 3: j=XY-X;
		}
		node &p = board[0][j];
		if(p.type < 0) continue;
		if(p.type == 'B'-'A' || p.type == 'D'-'A') {
			p.type = 1-p.type;
			p.rot = 0;
			p.rots = NULL;	// formalnie rzecz biorąc to powinniśmy
							// tu czyścić pamięć ale OS zrobi to
							// za nas na końcu programu
		} else if(p.type == 'C'-'A' || ('F'-'A' <= p.type && p.type <= 'H'-'A') ) {
			p.type = -1-p.type;
			p.rot = i;
			p.rots = NULL;
		} else if(p.type == 'I'-'A') {
			p.rots = new lst<char>(i, new lst<char>(i+1));
		}
	}


	// heureza #3: sytuacja na bokach
	// dla boku i = 0..3 (N,E,S,W):
	// 'B' ustalamy na i%2; 'CF' redukujemy opcje do i,(i+1)%4
	// 'G' ustalamy na i; 'H' redukujemy do i (dla uproszczenia impl.)
	// 'I' redukujemy do i,(i+1)%4,(i+2)%4
	int tmpA=1, tmpB=X-2, tmpC=1; // od,do,inkrementor
	for(int i=0; i<4; ++i) {
		switch(i) {
			case 1: tmpA=2*X-1;  tmpB=XY-X-1; tmpC=X; break;
			case 2: tmpA=XY-X+1; tmpB=XY-2;   tmpC=1; break;
			case 3: tmpA=X;		 tmpB=XY-2*X; tmpC=X;
		}
		for(; tmpA<=tmpB; tmpA+=tmpC) {
			node &p = board[0][tmpA];
			if(p.type == 'B'-'A') {
				p.type = -1-p.type;
				p.rot = i%2;
				p.rots = NULL;
				takePossOut(tmpA%X, tmpA/X, p.rot);
				takePossOut(tmpA%X, tmpA/X, (p.rot+2)%4);
			} else if(p.type == 'C'-'A' || p.type == 'F'-'A') {
				p.rots = new lst<char>(i, new lst<char>((i+1)%4));
			} else if(p.type == 'G'-'A') {
				p.type = -1-p.type;
				p.rot = i;
				p.rots = NULL;
			} else if(p.type == 'H') {
				p.rots = new lst<char>(i);
			} else if(p.type == 'I'-'A') {
				p.rots = new lst<char>(i, new lst<char>((i+1)%4, new lst<char>((i+2)%4)));
			}
		}
	}


	// heureza #4: sąsiadujące żarówki
	// w takim wypadku jasne, że nie mogą być do siebie skierowane
	for(xx=0; xx < X; ++xx) {
		for(yy=0; yy < Y; ++yy) {
			node &p = board[yy][xx];
			if(p.type != 'I'-'A' && p.type != -1-('I'-'A'))
				continue;
			// ps. żarówki są na tyle nietypowe, że nie warto
			// korzystać tu z takePossOut/3, lepiej ręcznie usuwać
			if(xx+1 < X) {
				node &r = board[yy][xx+1]; // element on the right
				if(r.type == 'I'-'A' || r.type == -1-('I'-'A')) {
					p.rots = lst<char>::drop(p.rots, 0);
					r.rots = lst<char>::drop(r.rots, 2);
				}
			}
			if(yy+1 < Y) {
				node &b = board[yy+1][xx]; // element below
				if(b.type == 'I'-'A' || b.type == -1-('I'-'A')) {
					p.rots = lst<char>::drop(p.rots, 1);
					b.rots = lst<char>::drop(b.rots, 3);
				}
			}
			if(p.type >= 0 && lst<char>::size(p.rots) <= 1) {
				p.rot = p.rots != NULL ? p.rots->hd : 0;
				p.type = -1-p.type;
				p.rots = NULL;
			}
		}
	}


	// wyświetlenie wyników preprocessingu
/*dbg*/	for(yy = 0; yy < Y; ++yy) {
/*dbg*/		for(xx = 0; xx < X; ++xx) {
/*dbg*/			node *p = &(board[yy][xx]);
/*dbg*/			if((*p).type >= 0)
/*dbg*/				printf(" %c%d{", (*p).type+'A', (*p).rot+1);
/*dbg*/			else
/*dbg*/				printf(" %c%d{", -(*p).type+'A'-1, (*p).rot+1);
/*dbg*/			for(; (*p).rots != NULL; (*p).rots = (*p).rots->tl)
/*dbg*/				printf("%d", (*p).rots->hd);
/*dbg*/			printf("}");
/*dbg*/		}
/*dbg*/		printf("\n");
/*dbg*/	}


	// wypisywanie wyniku
	for(yy = 0; yy < Y; ++yy) {
		printf("%c%d", board[yy][0].type < 0 ? -board[yy][0].type+'A'-1 : board[yy][0].type+'A', board[yy][0].rot+1);
		for(xx = 1; xx < X; ++xx)
			printf(" %c%d", board[yy][xx].type < 0 ? -board[yy][xx].type+'A'-1 : board[yy][xx].type+'A', board[yy][xx].rot+1);
		printf("\n");
	}
	return 0;
}




/*******************************
  Definicje funkcji
*******************************/

// heureza #5: zabieranie możliwości sąsiadom
// Chodzi o prostą rzecz: gdy uznajemy, że
// z danego pola nigdy nie wyjdzie w kierunku
// dd żaden kabel, to sąsiad od strony dd
// nie powinien nawet rozważać opcji podłączenia
void takePossOut(int xx, int yy, int dd) {
}

