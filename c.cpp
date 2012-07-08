/*********************************************************************
** 
** To begin with: english is often easier to use in programming mais
** dans les commentaires jednak polski me gusta :)
** 
*********************************************************************/
#include <cstdio>
#include <vector>
#include <algorithm>



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
inline node* unionfindHead(int);
inline void unionfindJoin(int,int);
void setRotation(int,int);

void setBacktrackRotation(int, char);



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

			board[0][boardPos].p = *board+boardPos; // inicjujemy set
			board[0][boardPos].rank = 1;
			board[0][boardPos].bulbs = inp == 'I';

			board[0][boardPos].type = inp - 'A';
			board[0][boardPos].rot = 0;
			// heureza #1: jeśli 'A' lub 'D' to ustalamy ułożenie od razu.
			if(inp - 'A' == 0 || inp == 'D')
				setRotation(boardPos, 0);
			else // ... jak nie to nie
				board[0][boardPos].rots = new lst<char>(rots[inp-'A']);
		}
	}
	int yy, xx;			// to do wypisywania wyników i debugowania

	
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
		if(p.type == 'B'-'A') // 'AD' też rot=0 ale to już h#1 załatwia
			setRotation(j, 0);
		else if(p.type == 'C'-'A' || ('F'-'A' <= p.type && p.type <= 'H'-'A') )
			setRotation(j, i);
		else if(p.type == 'I'-'A') {
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
			if(p.type == 'B'-'A')
				setRotation(tmpA, i%2);
			else if(p.type == 'C'-'A' || p.type == 'F'-'A') {
				p.rots = new lst<char>(i, new lst<char>((i+1)%4));
			} else if(p.type == 'G'-'A')
				setRotation(tmpA, i);
			else if(p.type == 'H')
				setRotation(tmpA, i);
			else if(p.type == 'I'-'A') {
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
			if(p.type >= 0 && lst<char>::size(p.rots) <= 1)
				setRotation(xx+yy*X, p.rots != NULL ? p.rots->hd : 0);
		}
	}

	// ok, mogły się pojawić braki w ustalaniu obrotów
	for(int i=0; i<XY; ++i)
		if(board[0][i].type >= 0 && board[0][i].rots == NULL)
			setRotation(i, board[0][i].rot);


	// wyświetlenie wyników preprocessingu
/*dbg*/	for(yy = 0; yy < Y; ++yy) {
/*dbg*/		for(xx = 0; xx < X; ++xx) {
/*dbg*/			node *p = &(board[yy][xx]);
/*dbg*/			if((*p).type >= 0)
/*dbg*/				printf("  ||  %c%d{", (*p).type+'A', (*p).rot+1);
/*dbg*/			else
/*dbg*/				printf("  ||  %c%d{", -(*p).type+'A'-1, (*p).rot+1);
/*dbg*/			int reducer = 4;
/*dbg*/			for(; (*p).rots != NULL; (*p).rots = (*p).rots->tl, --reducer)
/*dbg*/				printf("%d", (*p).rots->hd);
/*dbg*/			while(reducer-- >= 0) printf(" ");
/*dbg*/			printf("}#%-2ld",p->p - board[0]);
/*dbg*/		}
/*dbg*/		printf("\n");
/*dbg*/	}

	std::vector< int > visitable; // zbiór wierzchołków osiągalnych ze źródła

	std::vector< std::pair<int,node*> > setRollback;	// struktura do operacji
													// rollback na union-find.
					// każda operacja unionJoin(a,b) powoduje dodanie
					// a lub b oraz oryginalnego pointera.
					// Po komplecie takich operacji dodawany jest strażnik.
	setRollback.push_back( std::pair<int,node*>(0,NULL));	// Oto strażnik.

	node &bat = board[0][posBat];
	if(bat.type >= 0)
		bat.type = -1-bat.type; // wyciągamy z pętli. Ziarnko do ziarnka...
	if(bat.rots == NULL)
		bat.rots = new lst<char>(bat.rot);
	for(lst<char> *r = board[0][posBat].rots; r != NULL; r=r->tl) {
		setBacktrackRotation(posBat, r->hd);
	}

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

node* unionfindHead(int x) {
	node* r = board[0]+x;
	for(; r->p != r; r=r->p) ;
	return r;
}

void unionfindJoin(int x, int y) {
	//if(x==y) ... // tego nie rozważamy bo unikamy takich połączeń
	node *xP = unionfindHead(x); // O(log(X+Y))
	node *yP = unionfindHead(y);
	if(xP == yP) return; // już połączone
	if(xP->rank > yP->rank) {
		node *t = xP;
		xP = yP;
		yP = t;
		// ew coś takiego: xP ^= yP ^= xP ^= yP;
	}
	// od teraz xP->rank <= yP->rank
	xP->p = yP;
	yP->rank += xP->rank;
	yP->bulbs += xP->bulbs;
}

// czy urządzenie na pozycji pos jest ustalone i łączy w kierunku dir
bool connectable(int pos, int dir) {
	int pType = 'A'-1-board[0][pos].type;
	if(pType < 'A') return false;
	if(pType == 'A'-'A' || pType == 'D'-'A') return true;
	dir = (4+dir-board[0][pos].rot)%4; // od teraz jakby zawsze rozważamy rot==0
	if(dir == 0) return true; // ciekawa własność urządzeń
	switch(pType) {
		case 'B': case 'E': return dir == 2;
		case 'C': case 'F': return dir == 1;
		case 'G': case 'H': return dir == 1 || dir == 2;
	}
	return false;
}
// usuwa połączenie w zadanym kierunku
void makeUnconnectable(int pos, int dir) {
	node &p = board[0][pos];
	if(p.type < 0) return;
	char type = p.type + 'A';
	switch(type) {
		case 'B': case 'E':
			lst<char>::drop(p.rots, dir%2);
			break;
		case 'C': case 'F':
			p.rots = lst<char>::drop(p.rots, dir);
			p.rots = lst<char>::drop(p.rots, (dir+3)%4);
			if(lst<char>::size(p.rots) <= 1)
				setRotation(pos, p.rots != NULL ? p.rots->hd : 0);
			break;
		case 'G': case 'H':
			setRotation(pos, (dir+1)%4);
			break;
		case 'A': case 'D': break; // dead-code o ile się nie pomyliłem
	}
}
// ustalona rotacja wynikająca z logiki "musi tak być zawsze"
void setRotation(int pos, int rot) {
	node &p = board[0][pos];
	char ptype = 'A'+p.type;
	p.type = -1-p.type;
	p.rots = NULL;	// formalnie rzecz biorąc powinniśmy
					// tutaj czyścić pamięć ale...
	p.rot = rot;
	// ok, teraz będzie brzydko... bardzo brzydko.
	char posRel[] = {
		pos%X<X-1?pos+1:-1, pos+X<XY?pos+X:-1, pos%X>=0?pos-1:-1, pos-X
	};
	switch(ptype) {
		case 'A': case 'D':
			if(posRel[0] >= 0 && connectable(posRel[0], 2))
				unionfindJoin(pos, posRel[0]);
			if(posRel[1] >= 0 && connectable(posRel[1], 3))
				unionfindJoin(pos, posRel[1]);
			if(posRel[2] >= 0 && connectable(posRel[2], 0))
				unionfindJoin(pos, posRel[2]);
			if(posRel[3] >= 0 && connectable(posRel[3], 1))
				unionfindJoin(pos, posRel[3]);
			break;
		case 'B': case 'E':
			if(posRel[rot] >= 0 && connectable(posRel[rot], (rot+2)%4))
				unionfindJoin(pos, posRel[rot]);
			if(posRel[1+rot] >= 0)
				makeUnconnectable(posRel[1+rot], (rot+3)%4);
			if(posRel[2+rot] >= 0 && connectable(posRel[2+rot], rot))
				unionfindJoin(pos, posRel[2+rot]); // nie ma modulo
			if(posRel[(3+rot)%4] >= 0)
				makeUnconnectable(posRel[(3+rot)%4], rot+1);
			break;
		case 'C': case 'F':
			if(posRel[rot] >= 0 && connectable(posRel[rot], (rot+2)%4))
				unionfindJoin(pos, posRel[rot%4]);
			if(posRel[(rot+1)%4] >= 0 && connectable(posRel[(rot+1)%4], (rot+3)%4))
				unionfindJoin(pos, posRel[(rot+1)%4]);
			break;
		case 'G': case 'H':
			if(posRel[rot] >= 0 && connectable(posRel[rot], (rot+2)%4))
				unionfindJoin(pos, posRel[rot]);
			if(posRel[(rot+1)%4] >= 0 && connectable(posRel[(rot+1)%4], (rot+3)%4))
				unionfindJoin(pos, posRel[(rot+1)%4]);
			if(posRel[(rot+2)%4] >= 0 && connectable(posRel[(rot+2)%4], rot))
				unionfindJoin(pos, posRel[(rot+2)%4]);
			break;
		case 'I':
			if(posRel[rot] >= 0 && connectable(posRel[rot], (rot+2)%4))
				unionfindJoin(pos, posRel[rot]);
	}
}

void setBacktrackRotation(int pos, char rot) {
}

