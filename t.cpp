// vim: tabstop=4:shiftwidth=4:cindent:nu

#include <cstdio>
#include <cstdlib>
#include "Unionfind-cpp/set.h"

using namespace std;

typedef unsigned int uint;
typedef unsigned char uchar;

struct node : public set<node> {
	char type; /**< Typ obiektu. Na początku ujemna, gdy dodatnia -> ustalona pozycja. */
	char rot;  /**< Dopuszczalne obroty obiektu. */
};
char rots[]        = {1,2,4,1,2,4,4,4,4};               /**< Ilość obrotów do rozważenia dla kolejnych typów klocków. */
char rot2numrots[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4}; /**< Konwersja możliwych obrotów do ich liczby. */


uint X,        /**< Ilość kolumn. */
	 Y,        /**< Ilość wierszy. */
	 XY;       /**< Taki mały helper; xy = x*y. */
uint batt_pos; /**< Pozycja baterii. */
node **board;  /**< Reprezentacja planszy. */
node *brd;     /**< brd = (*board). */


void __dbgprint(const char*, int elem = -1, uint shift = 0);
uint __dbgbacktracks = 0;
vector<uint> __dbgbacktracklvls;
void read_board();
void print_board();
void h1();
void backtrack(uint);

/** Ustaw pozycję. @param r Pozycja w notacji binarnej (1,2,4,8). */
inline void make_setr(uint p, char r)         { brd[p].type      = abs(brd[p].type);      brd[p].rot      = r; }
/** Ustaw pozycję. @param r Pozycja w notacji binarnej (1,2,4,8). */
inline void make_setr(uint x, uint y, char r) { board[y][x].type = abs(board[y][x].type); board[y][x].rot = r; }
inline void make_set(uint p)         { brd[p].type      = abs(brd[p].type); }         /**< Ustaw znacznik 'ustalony'. */
inline void make_set(uint x, uint y) { board[y][x].type = abs(board[y][x].type); }    /**< Ustaw znacznik 'ustalony'. */
inline void make_unset(uint p)         { brd[p].type      = -abs(brd[p].type); }      /**< Usuń znacznik 'ustalony'. */
inline void make_unset(uint x, uint y) { board[y][x].type = -abs(board[y][x].type); } /**< Usuń znacznik 'ustalony'. */
inline bool is_set(uint p)        { return brd[p].type      > 0; }                    /**< Czy ma znacznik 'ustalony'? */
inline bool is_set(uint x,uint y) { return board[y][x].type > 0; }                    /**< Czy ma znacznik 'ustalony'? */
inline bool has_rotation(uint p, char r)         { return brd[p].rot      & (1<<r); } /**< Czy rotacja r jest możliwa? @param r Obroty, notacja naturalna (0,1,2,3). */
inline bool has_rotation(uint x, uint y, char r) { return board[y][x].rot & (1<<r); } /**< Czy rotacja r jest możliwa? @param r Obroty, notacja naturalna (0,1,2,3). */
inline char get_chtype(uint p)         { return abs(brd[p].type)      + 'A'-1; }      /**< Zwraca typ. */
inline char get_chtype(uint x, uint y) { return abs(board[y][x].type) + 'A'-1; }      /**< Zwraca typ. */
inline char get_chrots(uint p)         { return brd[p].rot; }                         /**< Zwraca dostępne rotacje. */
inline char get_chrots(uint x, uint y) { return board[y][x].rot; }                    /**< Zwraca dostępne rotacje. */
inline char get_chrot(uint p) {                                                       /**< Zwraca rotację. */
	if(brd[p].rot > 2) return brd[p].rot==4?'3':'4';
	else               return brd[p].rot==2?'2':'1';
}
inline char get_chrot(uint x, uint y) {                                               /**< Zwraca rotację. */
	if(board[y][x].rot > 2) return board[y][x].rot==4?'3':'4';
	else                    return board[y][x].rot==1?'1':'2';
}
inline void rem_rotation(uint p, char r) {                                            /**< Usuwa opcję rotacji. @param r Rotacja w notacji naturalnej (0,1,2,3). */
	brd[p].rot &= (0xF - (1<<r));
	if(rot2numrots[brd[p].rot] <= 1) make_set(p);
}
inline void rem_rotation(uint x, uint y, char r) {                                    /**< Usuwa opcję rotacji. @param r Rotacja w notacji naturalnej (0,1,2,3). */
	board[y][x].rot &= (0xF - (1<<r));
	if(rot2numrots[board[y][x].rot] <= 1) make_set(x,y);
}


/** Główna metoda. */
int main() {
	read_board();
	__dbgprint("Po wczytaniu i H2");
	h1();
	__dbgprint("Po H1");

	// backtrack
	char opts = get_chrots(batt_pos);
	make_set(batt_pos);
	__dbgbacktracklvls.push_back(0);
	for(char i=1; i<=8; i<<=1) {
		if(! (i & opts) ) continue;
		++__dbgbacktracklvls[0];
		brd[batt_pos].rot = i;
		__dbgprint("Backtrack: pozycja baterii", batt_pos, 0);
		backtrack(1);
	}
	printf("Call statistics:________________________________________\n");
	printf("  * \t%-20s #%4d\n", "backtrack", __dbgbacktracks);
	printf("Backtrack statistics:___________________________________\n");
	uint __dbgtmp = 0;
	for(vector<uint>::iterator it = __dbgbacktracklvls.begin(); it!=__dbgbacktracklvls.end(); ++it, ++__dbgtmp) {
		printf("  %-2d  --  %2d ", __dbgtmp, *it);
		for(uint i=0; i<*it; ++i) printf("|");
		printf("\n");
	}
	return 0;
}


void backtrack(uint depth) {
	++__dbgbacktracks;
	if(__dbgbacktracklvls.size() < depth) __dbgbacktracklvls.resize(depth);
	++__dbgbacktracklvls[depth];
	uint p=0;
	for(p=0;(p<XY) && is_set(p); ++p) ;			// TODO: this is a very poor select
	if(p==XY) return;
	char opts = get_chrots(p);
	make_set(p);
	for(char i=1; i<=8; i<<=1) {
		if(! (i & opts) ) continue;
		brd[p].rot = i;
		__dbgprint("Backtrack", p, depth);
		backtrack(depth+1);
	}
	make_unset(p);
}


/** Wczytujemy wierszami, maksymalnie 1500 znaków na raz (oraz dodatkowo znak końca łańcucha).
 * @warning Zakładamy, że na wejściu nie ma zbędnych białych znaków. */
#define INPUTLEN 1501

/** Wczytywanie planszy.
 * Alokuje reprezentację; ustawia zmienne ::X, ::Y i ::XY. Znajduje baterię (ustala ::batt_pos).
 * Uruchamia heurezę H2. */
void read_board() {
	char input[INPUTLEN];
	scanf(" %d %d ", &X, &Y);
	XY = X*Y;

	// Podwójna notacja: board[x][y] <==> brd[i] dla i == X*y + x
	board = new node*[Y];
	brd = (*board) = new node[XY];
	for(uint i=0;i<Y;++i) board[i] = *board + i*X;

	uint board_pos = 0;
	char *inp;
	while(fgets(input,INPUTLEN,stdin) != NULL) {
		for(inp=input; *inp; inp+=3, ++board_pos) { // ustawianie .type .rot
			if((*inp) == 'H' || ('D' <= (*inp) && (*inp) <= 'F')) // szukanie baterii
				batt_pos = board_pos;
			brd[board_pos].type = 'A' - (*inp) - 1;
			switch(*inp) {
				case 'A': case 'D': make_setr(board_pos,1<<0); break;
				case 'B': case 'E': brd [board_pos].rot = 0x3; break;
				case 'I':	// Heureza H2.
						  	// Wykonuje redukcje na styku żarówka-żarówka.
									brd [board_pos].rot = 0xF;
									if(inp > input && inp[-3]=='I') { // poziomo
										rem_rotation(board_pos-1 ,0);
										rem_rotation(board_pos   ,2);
									}
									if(board_pos>X && get_chtype(board_pos-X)=='I') { // pionowo
										rem_rotation(board_pos-X , 1);
										rem_rotation(board_pos   , 3);
									}
									break;
				default:            brd [board_pos].rot = 0xF;
			}
		}
	}
}


/** Wyświetlanie planszy. Nic ciekawego tu się nie dzieje. */
void print_board() {
	printf("%d %d\n", X,Y);
	uint x, y;
	node *b = brd;
	for(y=0;y<Y;++y) {
		printf("%c", get_chtype(x,y));
		for(x=1;x<X;++x,++b)
			printf("%c%d ", get_chtype(x,y), (*b).rot  + 1);
		printf("\n");
	}
}


/** Wyświetlanie planszy w trybie debug. */
void __dbgprint(const char *str, int elem, uint shift) {
	uint x,y,i;
	printf("%*s[%2d]: %2d > %s\n", shift<<1, "", shift, elem, str);
	for(y=0;y<Y;++y) {
		printf("%*s", (shift<<1)+3, "|");
		for(x=0;x<X;++x) {
			printf("    %c%c",is_set(x,y)?'*':' ', get_chtype(x,y));
			for(i=0;i<4;++i)
				printf("%c", has_rotation(x,y,i)? (is_set(x,y)?get_chrot(x,y):'+'):(is_set(x,y)?' ':'_'));
		}
		printf("\n");
	}
}

/** Heureza H1. Ustala sytuacje na krawędziach. */
void h1() {
	uint x,y,i=0;
	char cornern[] = {0,1,3,2}; // Mały helper: przekształcenie NW NE SW SE -> NW NE SE SW.
	// rogi
	for(y=0; y<Y; y+=Y-1)
		for(x=0;x<X; x+=X-1, ++i)
			switch(get_chtype(x,y)) {
				case 'B':                               make_setr(x,y,1<<0);          break ; // martwy node
				case 'C': case 'F': case 'G': case 'H': make_setr(x,y,1<<cornern[i]); break ;
				case 'I':                               rem_rotation(x,y, (cornern[i]+3)&0x3); rem_rotation(x,y, (cornern[i]+2)&0x3); // &0x3 <=> %4
				default: ;
			}
	for(i=0,y=0;y<Y;y+=Y-1,++i) // boki góra/dół
		for(x=X-2;x>0;--x)
			switch(get_chtype(x,y)) {
				case 'B':           make_setr(x,y,1);                                     break ;
				case 'C': case 'F': rem_rotation(x,y,(i+2)&3); rem_rotation(x,y,(i+3)&3); break ;
				case 'G': case 'H': make_setr(x,y,i<<1);                                  break ;
				case 'I':           rem_rotation(x,y,1|(2>>i));
				default:            ;
			}
	for(i=0,x=0;x<X;x+=X-1,++i) // boki lewo/prawo
		for(y=Y-2;y>0;--y)
			switch(get_chtype(x,y)) {
				case 'B':           make_setr(x,y,2);                             break ;
				case 'C': case 'F': rem_rotation(x,y,1^i); rem_rotation(x,y,2^i); break ;
				case 'G': case 'H': make_setr(x,y,1<<(1|(2>>i)));                 break ;
				case 'I':           rem_rotation(x,y,2-i<<1);
				default:            ;
			}
}
