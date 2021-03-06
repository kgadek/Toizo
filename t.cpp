// vim: tabstop=4:shiftwidth=4:cindent:nu

//#define DBG

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <utility>

using namespace std;

#ifdef DBG
#include "quickcheck/quickcheck/quickcheck.hh"
using namespace quickcheck;
#endif

uint X,               /**< Ilość kolumn. */
	 Y,               /**< Ilość wierszy. */
	 XY;              /**< Taki mały helper; xy = x*y. */

/** Enum opisujący kierunki.
 * @warning Nie zmieniać kolejności! */
enum directions {
	DIR_E  = 0, DIR_S , DIR_W , DIR_N,
	DIR_NW = 4, DIR_NE, DIR_SE, DIR_SW
};

//********************************************************************************
//********************************************************************************
//********************************************************************************
typedef unsigned char uchar;                      /**< Czasem używam uchar. W sumie nie ale cii... */
typedef unsigned int uint;                        /**< Często używam uint. */
typedef std::pair<uint,uint> stampedUnion;        /**< Struktura wskazująca na operację union: id na stosie  *  stamp. */
typedef std::pair<int,uint> unionDescr;           /**< Struktura opisująca operację union na stosie:  priorytet  *  stamp. */
typedef std::pair<stampedUnion,uint> stampedRank; /**< Struktura: wskazanie na union  *  wysokość drzewa następująca po wskazanej danej operacji union. */

/** Mixin: union-set z operacją set::backtrack().
 * Koszt pamięciowy: \f$4+20*U+16*N\f$ bajtów dla N node-ów i U operacji union. */
template <class T>
class set {
	T* p;                              /**< Link na node nadrzędny. */
	stampedUnion pU;                   /**< Wskazanie na opis operacji. Pozwala stwierdzić, czy link jest żywy. */
	std::vector<stampedRank> rL;       /**< Lista ranków. Jest listą par (r,u) gdzie r to rank zaraz po wykonaniu union u. */

	static std::vector<unionDescr> uH; /**< Stos operacji union. */
	static uint uStmp;                 /**< Stamp do oznaczania operacji union. */

	/** Stwierdza, czy link wychodzący jest żywy, jeśli nie to go usuwa.
	 * @pre @verbatim set::p != NULL @endverbatim
	 * @return Prawda gddy link żywy. */
	bool isLinkAlive() {
		return ( pU.first < (uint)T::uH.size() && T::uH[pU.first].second == pU.second )
			|| (p = NULL);
	}

	/** Zwraca rank. Jeśli tylko może to skraca set::rL.
	 * Czas: \f$ O(\log \log n)\f$.
	 * @return Wysokość poddrzewa o korzeniu w bieżącym węźle. */
	uint getRank() {
		int l = -1, u = rL.size(), i;
		for(i = (l+u)>>1; l+1<u; i=(l+u)>>1)
			if( T::uH[rL[i].first.first].second == rL[i].first.second )
				l = i;
			else u = i;
		rL.resize(l+1);
		return l==-1 ? 1 : rL.back().second;
	}

	/** Swap. */
	template <class X> void swap(X &a, X &b) {
		X t = a;
		a = b;
		b = t;
	}
	set& operator=(const set<T>&);        /**< Za Efficient C++, items I11, I27: blokada autogeneracji kodu. */
	template <class R> set(const set<R>&) /**< Za Efficient C++, items I11, I27: blokada autogeneracji kodu. */;
public:
	/** Konstruktor, który niewiele robi. */
	set() : p(NULL),pU(),rL() {}

	/** Wyszukiwanie reprezentanta zbioru.
	 * Czas: \f$O(\log n)\f$.
	 * @remark Implicite (poprzez set::isLinkAlive()) usuwa martwe linki. */
	T* find() {
		T* n = static_cast<T*>(this);
		while(n->p != NULL && n->isLinkAlive())
			n = n->p;
		return n;
	}

	/** Operacja union.
	 * Czas: \f$O(\log n)\f$.
	 * @remark Informacja o łączu (na stosie) zostaje zapisana zawsze, nawet gdy elementy należą do tego
	 * samego zbioru.
	 * @param oo Łącze między bieżącym a wskazanym nodem.
	 * @param prio Priorytet łącza. */
	void unionW(T& oo, const int prio) {
		T *pp    = find(),                                          // restoring phase
		  *qq    = oo.find();
		uint ppr = pp->getRank(),
			 qqr = qq->getRank();
		if(ppr > qqr) {
			swap(ppr,qqr);
			swap(pp,qq);
		}
		if(T::uH.empty() || prio >= T::uH.back().first)             // linking phase
			T::uH.push_back(unionDescr(prio, T::uStmp++));
		if(pp == qq) return;
		pp->p = qq;
		pp->pU = stampedUnion(T::uH.size()-1, T::uH.back().second);
		if(!qq->rL.empty() && qq->rL.back().first == pp->pU)        // rank updating phase
			qq->rL.back().second = std::max(ppr+1,qqr);
		else
			qq->rL.push_back(stampedRank(pp->pU, std::max(ppr+1,qqr)));
	}

	/** Cofanie. Anuluje tyle cofnięć, by set::unionW() o najwyższym priorytecie przestał obowiązywać. */
	static void backtrack() {
		if(!T::uH.empty())
			T::uH.pop_back();
	}

};
template <class T>
std::vector<unionDescr> set<T>::uH = std::vector<unionDescr>();
template <class T>
uint set<T>::uStmp = 0;
//********************************************************************************
//********************************************************************************
//********************************************************************************
struct node : public set<node> {
	char type;                  /**< Typ obiektu. Ustalona pozycja gddy wartość dodatnia. */
	uchar rot;                  /**< Dopuszczalne obroty obiektu. */
	node() : type(0), rot(0) {} /**< Konstruktor, który niewiele robi. */
};

/** Ilość obrotów do rozważenia dla kolejnych typów klocków. */
char rots[] = {1,2,4,1,2,4,4,4,4};
			// A B C D E F G H I

/** Konwersja dopuszczalnych obrotów binarnie na ich ilość. */
char rot2numrots[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

/** Konwersja typu na połączenia wychodzące. */
uchar type2conns[] = {000,0xF,0x5,0x3,0xF,0x5,0x3,0x7,0x7,0x1,};
				       // A   B   C   D   E   F   G   H   I

/** (Pure) Pozycja po ruchu w kierunku @a d.
 * @param p Pozycja (1D).
 * @param d Kierunek binarnie.
 * @return Pozycja lub -1 gdy poza planszą. */
inline int pos_goto(const uint x, const uint xy, const uint p, const char d) {
	switch(d) {
		case 1 : if(!((p+1)%x)) return -1; return p+1;
		case 2 : if(p>=xy-x)    return -1; return p+x;
		case 4 : if(!(p%x))     return -1; return p-1;
		default: if(p<x)        return -1; return p-x;
	}
}

#ifdef DBG
class PPosGoto : public Property<uint,uint,uint,char> {
	bool holdsFor(const uint &x, const uint &y, const uint &p, const char &d) {
		uint xy = x*y;
		return (pos_goto(x,xy,0,4)    == -1)
			&& (pos_goto(x,xy,x-1,1)  == -1)
			&& (pos_goto(x,xy,xy-1,1) == -1)
			&& (pos_goto(x,xy,xy-1,2) == -1);
	}
	bool accepts(const uint &x, const uint &y, const uint &p, const char &d) {
		return x*y > 0;
	}
	void generateInput(size_t n, uint &x, uint &y, uint &p, char &d) {
		generate(n,x);
		generate(n,y);
		generate(n,p);
		generate(n,d);
		d = 1<<(d%4);
	}
};
#endif

/** Obraca rzeczy w zapisie binarnym.
 * @param conn Do obrotu, z zakresu 0-15.
 * @param rot Obrót (naturalnie), z zakresu 0-3. */
inline char rotate_bin(uchar conn, const uchar rot) {
	uchar l = (conn<<=rot) & 0xF;
	return l | (conn>>4);
}

#ifdef DBG
class PRotateBin : public Property<uchar,uchar> {
	bool holdsFor(const uchar& conn, const uchar& rot) {
		if(        (rotate_bin(conn,0) == conn)
				&& (rotate_bin(conn,4) == conn))
			return true;
		return false;
	}
	void generateInput(size_t n, uchar& conn, uchar& rot) {
		generate(n, conn);
		conn %= 0x10;
		generate(n, rot);
		rot %= 4;
	}
};
#endif

/** Obraca rzeczy w zapisie binarnym.
 * @param conn Do obrotu, z zakresu 0-15.
 * @param rot Obrót (binarnie), z zakresu 0-8. */
inline uchar rotate_bin_wbin(const uchar conn, const uchar rot) {
	const uchar ret[] = {0,1,2,2,3,3,3,3,4};
	return rotate_bin(conn, ret[(uint)rot]);
}

#ifdef DBG
class PRotateBinWbin : public Property<uchar,uchar> {
	bool holdsFor(const uchar& conn, const uchar& rot) {
		return (rotate_bin_wbin(conn,0) == conn)
			&& (rotate_bin_wbin(conn,0) == rotate_bin(conn,0))
			&& (rotate_bin_wbin(conn,1) == rotate_bin(conn,1))
			&& (rotate_bin_wbin(conn,2) == rotate_bin(conn,2))
			&& (rotate_bin_wbin(conn,4) == rotate_bin(conn,3))
			&& (rotate_bin_wbin(conn,8) == rotate_bin(conn,4));
	}
	void generateInput(size_t n, uchar& conn, uchar& rot) {
		generate(n, conn);
		conn %= 0x10;
		generate(n, rot);
		rot %= 9;
	}
};
#endif

/** Test: czy element @a p łączy w kierunku @d.
 * @pre @verbatim is_set(p) @endverbatim
 * @param d Kierunek (binarnie). */
inline bool does_connects(const node &n, const uchar d) {
	return rotate_bin_wbin(type2conns[(int)n.type], n.rot>>1) & d;
}

/** Ustaw znacznik 'ustalony'. */
inline void mark_set(node &p) {
	p.type = abs(p.type);
}

/** Ustaw obrót.
 * @param r Obrót (binarnie). */
inline void set_chrot(node &p, const uchar r) {
	p.rot = r;
}

/** Ustaw pozycję.
 * @param r Obrót (binarnie). */
inline void mark_set(node &p, const uchar r) {
	mark_set(p);
	set_chrot(p,r);
}

/** Usuń znacznik 'ustalony'. */
inline void make_unset(node &p) {
	p.type = -abs(p.type);
}

/** Czy ma znacznik 'ustalony'? */
inline bool is_set(const node &p) {
	return p.type > 0;
}

/** Test: czy obrót @a r jest dopuszczalny?
 * @param r Obrót (naturalnie), z zakresu 0-7. */
inline bool has_rotation(const node &p, const uchar r) {
	return p.rot & (1<<r);
}

/** Zwraca typ. */
inline char get_chtype(const node &p) {
	return abs(p.type) + 'A' - 1;
}

/** Zwraca dostępne rotacje. */
inline uchar get_chrots(const node &p) {
	return p.rot;
}

/** Zwraca rotację.
 * @pre @verbatim is_set(p) @endverbatim */
inline char get_chrot(const node &p) {
	char ret[] = {'1','1','2','1','3','1','1','1','4'};
	return ret[(int)(p.rot&0xF)];
}

#ifdef DBG
class PGetChrot : public Property<uchar> {
	bool holdsFor(const uchar& rot) {
		node lol;
		lol.rot;
		char ret = get_chrot(lol);
		return '0' <= ret && ret <= '4';
	}
};
#endif

/** Usuwa jedną z możliwości rotacji.
 * @param r Rotacja (naturalnie), 0-3. */
inline void rem_rotation(node &p, uchar r) {
	p.rot &= (0xF ^ (1<<r));
	if(rot2numrots[(int)p.rot] <= 1)
		mark_set(p);
}

/** Czy element połączony z sąsiadem.
 * @param p Pozycja (1D).
 * @param bdir Kierunek (binarnie).
 * @return Pozycja sąsiada gdy połączony lub -1. */
inline int connects_in_bdir(node* brd, uint p, uint bdir) {
	uchar conns = rotate_bin_wbin(type2conns[(int)brd[p].type], brd[p].rot>>1);
	int cnntd;
	return ((bdir&conns)                                                // jeśli dany element ma przewód w tym kierunku
		&& (cnntd = pos_goto(X,XY,p,bdir)) >= 0                         // i na końcu przewodu znajduje się element
		&& is_set(brd[cnntd])                                           // który został już ustawiony
		&& does_connects(brd[cnntd], rotate_bin(bdir,2))) ? cnntd : -1; // i ma przewód do którego się można stąd podłączyć
}
//********************************************************************************
//********************************************************************************
//********************************************************************************


uint batt_pos;        /**< Pozycja baterii. */
node **board;         /**< Reprezentacja planszy. */
node *brd;            /**< brd = (*board). */
vector<node*> lbulbs; /**< Zbiór wszystkich żarówek. */
uint best_so_far = 0; /**< Najlepsze znalezione rozwiązanie - ilość żarówek. */
char* snapshot_best;  /**< Najelpsze znalezione rozwiązanie - zapis. */

inline uint bulbs_connected();
void read_board();
void print_board();
void h1();
void backtrack(uint);
void savesnapshot();
void restoresnapshot();

/** Główna metoda. */
/* ------------------------------------------------------------------------------ */
int main() {
#ifdef DBG
	printf("Quickcheck: PGetChrot\n\n");
	(new PGetChrot())->check();
	printf("\nQuickcheck: PPosGoto\n\n");
	(new PPosGoto())->check();
	printf("\nQuickcheck: PRotateBin\n\n");
	(new PRotateBin())->check();
	printf("\nQuickcheck: PRotateBinWbin\n\n");
	(new PRotateBinWbin())->check();
#endif

	read_board();
	h1();

	char pr,                               // priorytet operacji unionW
		 j,                                // kierunek wychodzący
		 opts = get_chrots(brd[batt_pos]), // opcje obrotów
		 i;                                // dany obrót
	uint p;                                // element w poprawianiu poheurezowym
	int cnntd;                             // sąsiad

	for(p=0;p<XY;++p) // poprawki po heurezach
		if(is_set(brd[p]))
			for(j=1; j<=8; j<<=1)
				if((cnntd = connects_in_bdir(brd, p,j)) >= 0)
					brd[cnntd].unionW(brd[p], -10);

	best_so_far = bulbs_connected();
	savesnapshot();
	// backtrack
	mark_set(brd[batt_pos]);
	for(i=1; i<=8; i<<=1) { // dla każdego możliwego obrotu:
		if(! (i & opts) )
			continue;
		pr = 5;
		set_chrot(brd[batt_pos], i);
		for(j=1; j<=8; j<<=1)
			if((cnntd = connects_in_bdir(brd, batt_pos,j)) >= 0)
				brd[cnntd].unionW(brd[batt_pos], --pr);
		backtrack(1);

		if(pr != 5)
			node::backtrack();
	}

	restoresnapshot();
	print_board();

	return 0;
}


void backtrack(uint depth) {
	uint blbs = bulbs_connected();
	if(blbs == lbulbs.size()) {
		print_board();
		exit(0);
	} else if(blbs > best_so_far) {
		best_so_far = blbs;
		savesnapshot();
	}
	uint p=0;                                // SELECT
	for(p=0;(p<XY) && is_set(brd[p]); ++p) ; // TODO: this is a very poor select indeed
	if(p==XY) return;                        // EXIT CONDITION
	char pr,
		 j,
		 opts = get_chrots(brd[p]), // opcje obrotów
		 prerot = brd[p].rot;
	int cnntd;
	mark_set(brd[p]);
	for(char i=1; i<=8; i<<=1) {
		if(! (i & opts) )
			continue;
		pr = 5;
		set_chrot(brd[p], i);
		for(j=1; j<=8; j<<=1)                             // dla każdego kierunku wychodzącego:
			if((cnntd = connects_in_bdir(brd,p,j)) >= 0)
				brd[cnntd].unionW(brd[p], --pr);          // połącz element bieżący w danym kierunku
		backtrack(depth+1);

		if(pr != 5)
			node::backtrack();
	}
	make_unset(brd[p]);
	set_chrot(brd[p], prerot);
}


/** Wczytywanie planszy.
 * Alokuje reprezentację; ustawia zmienne ::X, ::Y, ::XY i ::snapshot_best. Znajduje baterię (ustala ::batt_pos).
 * Uruchamia heurezę H2. */
void read_board() {
	scanf(" %u %u ", &X, &Y);
	XY = X*Y;

	snapshot_best = new char[XY];
	board = new node*[Y]; // Podwójna notacja: board[x][y] <=> brd[i] dla i == X*y + x
	brd = (*board) = new node[XY];
	for(uint i=0;i<Y;++i) board[i] = *board + i*X;

	char chr;
	char dgt;
	for(uint board_pos=0; board_pos < XY; ++board_pos) {
		scanf(" %c%c ", &chr, &dgt);
		if(chr == 'H' || ('D' <= chr && chr <= 'F')) // szukanie baterii
			batt_pos = board_pos;
		else if(chr == 'I')                                // szukanie żarówek
			lbulbs.push_back(brd+board_pos);
		brd[board_pos].type = 'A' - 1 - chr;
		switch(chr) {
			case 'A': case 'D': mark_set(brd[board_pos],1<<0);
								break;
			case 'B': case 'E': set_chrot(brd[board_pos], 0x3);
								break;
			case 'I':			// H2.
								// Wykonuje redukcje na styku żarówka-żarówka.
								set_chrot(brd[board_pos], 0xF);
								if((board_pos%X)>0 && get_chtype(brd[board_pos-1])=='I') { // poziomo
									rem_rotation(brd[board_pos-1] ,0);
									rem_rotation(brd[board_pos]   ,2);
								}
								if(board_pos>X && get_chtype(brd[board_pos-X])=='I') {     // pionowo
									rem_rotation(brd[board_pos-X] , 1);
									rem_rotation(brd[board_pos]   , 3);
								}
								break;
			default:            set_chrot(brd[board_pos], 0xF);
		}
	}
}


/** Wyświetlanie planszy. Please disperse. There's nothing for you to see here. Keep moving. */
void print_board() {
	printf("%d %d\n", X,Y);
	uint x, y;
	for(y=0;y<Y;++y) {
		printf("%c%c", get_chtype(board[y][0]), get_chrot(board[y][0]));
		for(x=1;x<X;++x)
			printf(" %c%c", get_chtype(board[y][x]), get_chrot(board[y][x]));
		putchar('\n');
	}
}


/** Wyświetlanie planszy w trybie debug.
 * @todo Oczywiście usunąć. */
void __dbgprint(const char *str, int elem, uint shift) {
	uint x,y,i;
	node *SRC = brd[batt_pos].find();
	printf("%*s[%2d]: %2d=%2d:%-2d w/ %2d bulbs > %s\n", shift<<1, "", shift, elem, elem%X, elem/X, bulbs_connected(), str);
	for(y=0;y<Y;++y) {
		if(y > 0) {
			printf("%*s", (shift<<1)+3, "|");
			for(x=0;x<X;++x)
				printf("%-18s", (board[y][x].find() == board[y-1][x].find()) ? "     |" : "");
			printf("\n");
		}
		printf("%*s", (shift<<1)+3, "|");
		for(x=0;x<X;++x) {
			printf ("   %c%c%c[;%dm%c%c[;%dm",board[y][x].find()==SRC?'*':' ', is_set(board[y][x])?'!':' ', 0x1B, y*X+x==(uint)(elem?31:0), get_chtype(board[y][x]), 0x1B, 0);
			for(i=0;i<4;++i)
				printf("%c", has_rotation(board[y][x],i)? (is_set(board[y][x])?get_chrot(board[y][x]):'+'):(is_set(board[y][x])?' ':'_'));
			if(x < X-1)
				printf("   %5s", (board[y][x].find() == board[y][x+1].find()) ? "-----" : "");
		}
		printf("\n");
	}
}

/** Heureza H1_1: rogi.
 * @param cornern Który róg poprawiamy. */
inline void h1_1(int x, int y, directions corner) {
	if(x<0 || y<0) return;
	switch(get_chtype(board[y][x])) {
		case 'B':           mark_set(board[y][x],1<<0); //martwy node
							break;
		case 'C': case 'F':
		case 'G': case 'H': mark_set(board[y][x], 1<<(corner-4));
							break;
		case 'I':           rem_rotation(board[y][x], (corner-2)&0x3);
							rem_rotation(board[y][x], (corner-1)&0x3);
							break;
		default: ;
	}
}

/** Heureza H1_2: boki.
 * @param i Początek obszaru.
 * @param iinc To, ile dokładamy do @a i by przejść dalej.
 * @param iend Koniec obszaru.
 * @param border Którą krawędź przetwarzamy. */
inline void h1_2(int i, int iinc, int iend, directions border) {
	for(; i!=iend; i+=iinc) {
		switch(get_chtype(brd[i])) {
			case 'B':           mark_set(brd[i], 2>>(border&0x1));
								break;
			case 'C': case 'F': rem_rotation(brd[i], border&0x3);
								rem_rotation(brd[i], (border-1)&0x3);
								break;
			case 'I':			rem_rotation(brd[i], border);
								break;
			case 'G':			mark_set(brd[i], 1<<((border+1)&0x3));
			default: ;
		}
	}
}

/** Heureza H1. Ustala sytuacje na krawędziach. */
void h1() {
	h1_1(0  , 0  , DIR_NW);
	h1_1(X-1, 0  , DIR_NE);
	h1_1(X-1, Y-1, DIR_SE);
	h1_1(0  , Y-1, DIR_SW);

	if(1 <= X-1)     h1_2 (1     , 1 , X-1 , DIR_N);
	if(2*X <= XY)    h1_2 (2*X-1 , X , XY-1, DIR_E);
	if(X >= 2)       h1_2 (XY-2  , -1, XY-X, DIR_S);
	if(XY >= (X<<1)) h1_2 (XY-2*X, -X, 0   , DIR_W);
}

/** Zwraca ilość żarówek podłączonych do źródła w danej chwili. */
inline uint bulbs_connected() {
	uint ret = 0;
	node *SRC = brd[batt_pos].find();
	for(vector<node*>::iterator it = lbulbs.begin(); it != lbulbs.end(); ++it)
		if((*it)->find() == SRC && is_set(**it))
			++ret;
	return ret;
}

/** Zapisuje obecny stan obrotów elementów na planszy.
 * @todo Da się zrobić 2x bardziej efektywnie pamięciowo.
 */
void savesnapshot() {
	for(uint i=0;i<XY;++i)
		snapshot_best[i] = brd[i].rot;
}

/** Odtwarza zapisany stan obrotów elementów na planszy. */
void restoresnapshot() {
	for(uint i=0;i<XY;++i)
		brd[i].rot = snapshot_best[i];
}

