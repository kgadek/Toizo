// vim: tabstop=4:shiftwidth=4:cindent:nu

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <utility>

using namespace std;

uint X,               /**< Ilość kolumn. */
	 Y,               /**< Ilość wierszy. */
	 XY;              /**< Taki mały helper; xy = x*y. */

//********************************************************************************
//********************************************************************************
//********************************************************************************
typedef unsigned int uint;
typedef std::pair<uint,uint> stampedUnion; /**< Struktura wskazująca na operację union: id na stosie  x  stamp. */
typedef std::pair<int,uint> unionDescr; /**< Struktura opisująca operację union na stosie:  priorytet  x  stamp. */
typedef std::pair<stampedUnion,uint> stampedRank; /**< Struktura: wskazanie na union  x  wysokość drzewa następująca po wskazanej danej operacji union. */

/** Mixin: union-set z operacją set::backtrack().
 * Koszt pamięciowy: \f$4+20*U+16*N\f$ bajtów dla N node-ów i U operacji union. */
template <class T>
class set {
	T* p; /**< Link na node nadrzędny. */
	stampedUnion pU; /**< Wskazanie na opis operacji. Pozwala stwierdzić, czy link jest żywy. */
	std::vector<stampedRank> rL; /**< Lista ranków. Jest listą par (r,u) gdzie r to rank zaraz po wykonaniu union u. */

	static std::vector<unionDescr> uH; /**< Stos operacji union. */
	static uint uStmp; /**< Stamp do oznaczania operacji union. */

	/** Stwierdza, czy link wychodzący jest żywy, jeśli nie to go usuwa. @pre @verbatim set::p != NULL @endverbatim */
	bool isLinkAlive() {
		return ( pU.first < (uint)T::uH.size() && T::uH[pU.first].second == pU.second ) // jeśli link żywy to zwróci true
			|| (p = NULL); // jeśli nie to zwróci NULL == false
	}

	/** Zwraca rank. Jeśli może to skraca set::rL. Czas: \f$ O(\log \log n)\f$. */
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
	set& operator=(const set<T>&); /**< Za Eff-C++ I11, I27: blokada autogeneracji kodu. */
	template <class R> set(const set<R>&) /**< Za Eff-C++ I11, I27: blokada autogeneracji kodu. */;
public:
	/** Konstruktor, który niewiele robi. */
	set() : p(NULL),pU(),rL() {}

	/** Wyszukiwanie reprezentanta zbioru. Przy okazji - implicite poprzez set::isLinkAlive() - usuwa martwe linki. Czas: \f$O(\log n)\f$. */
	T* find() {
		T* n = static_cast<T*>(this);
		while(n->p != NULL && n->isLinkAlive())
			n = n->p;
		return n;
	}

	/** Operacja union. Czas: \f$O(\log n)\f$. */
	void unionW(T& oo /**< Łącze między bieżącym a wskazanym nodem. */,
			const int prio /**< Priorytet łącza. */) {
		T *pp = find(), // restoring phase
			*qq = oo.find();
		uint ppr = pp->getRank(),
			 qqr = qq->getRank();
		if(ppr > qqr) {
			swap(ppr,qqr);
			swap(pp,qq);
		}
		if(T::uH.empty() || prio >= T::uH.back().first) // linking phase
			T::uH.push_back(unionDescr(prio, T::uStmp++));
		if(pp == qq) return;
		pp->p = qq;
		pp->pU = stampedUnion(T::uH.size()-1, T::uH.back().second);
		if(!qq->rL.empty() && qq->rL.back().first == pp->pU) // rank updating phase
			qq->rL.back().second = std::max(ppr+1,qqr);
		else
			qq->rL.push_back(stampedRank(pp->pU, std::max(ppr+1,qqr)));
	}

	/** Cofanie. Anuluje tyle cofnięć, by ostatni union o najwyższym priorytecie przestał obowiązywać. */
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
	char type; /**< Typ obiektu. Ustalona pozycja gddy wartość dodatnia. */
	char rot;  /**< Dopuszczalne obroty obiektu. */
	node() : type(0), rot(0) {}
};

/** Ilość obrotów do rozważenia dla kolejnych typów klocków. */
char rots[] = { 1,  2,  4,  1,  2,  4,  4,  4, 4};
			//  A   B   C   D   E   F   G   H  I

/** Konwersja dopuszczalnych obrotów binarnie na ich ilość. */
char rot2numrots[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

/** Konwersja typu na połączenia wychodzące. */
char type2conns[] = {000, 0xF, 0x5, 0x3, 0xF, 0x5, 0x3, 0x7, 0x7, 0x1, };
						// A    B    C    D    E    F    G    H    I


/** Pozycja po ruchu w kierunku @a d.
 * @param p Pozycja (1D).
 * @param d Kierunek binarnie (1,2,4,8).
 * @return Pozycja lub -1 gdy poza planszą. */
inline int pos_goto(uint p, const char d) {
	//++__dbg_calls["pos_goto"];
	switch(d) {
		case 1 : if(!(p+1%X)) return -1; return p+1;
		case 2 : if(p>=XY-X)  return -1; return p+X;
		case 4 : if(!(p%X))   return -1; return p-1;
		default: if(p<X)      return -1; return p-X;
	}
}

/** Obraca rzeczy w zapisie binarnym.
 * @param conn Do obrotu, z zakresu 0-15.
 * @param rot Obrót (naturalnie). */
inline char rotate_bin(char conn, const char rot) {
	//++__dbg_calls["rotate_bin"];
	char l = (conn<<=rot) & 0xF;
	return l | (conn>>4);
}

/** Obraca rzeczy w zapisie binarnym.
 * @param conn Do obrotu, z zakresu 0-15.
 * @param rot Obrót (binarnie). */
inline char rotate_bin_wbin(const char conn, const char rot) {
	//++__dbg_calls["rotate_bin_wbin"];
	const char ret[] = {0,1,2,2,3,3,3,3,4};
	return rotate_bin(conn, ret[(int)rot]);
}

/** Test: czy element @a p łączy w kierunku @d.
 * @pre @verbatim is_set(p) @endverbatim
 * @param d Kierunek (binarnie). */
inline bool does_connects(const node &n, const char d) {
	//++__dbg_calls["does_connects"];
	return rotate_bin_wbin(type2conns[(int)n.type], n.rot>>1) & d;
}

/** Ustaw znacznik 'ustalony'. */
inline void make_set(node &p) {
	//++__dbg_calls["make_set/1"];
	p.type = abs(p.type);
}

/** Ustaw pozycję.
 * @param r Obrót (binarnie). */
inline void make_set(node &p, const char r) {
	//++__dbg_calls["make_set/2"];
	make_set(p);
	p.rot  = r;
}

/** Usuń znacznik 'ustalony'. */
inline void make_unset(node &p) {
	//++__dbg_calls["make_unset"];
	p.type = -abs(p.type);
}

/** Czy ma znacznik 'ustalony'? */
inline bool is_set(const node &p) {
	//++__dbg_calls["is_set"];
	return p.type > 0;
}

/** Test: czy obrót @a r jest dopuszczalny?
 * @param r Obrót (naturalnie). */
inline bool has_rotation(const node &p, const char r) {
	//++__dbg_calls["has_rotation"];
	return p.rot & (1<<r);
}

/** Zwraca typ. */
inline char get_chtype(const node &p) {
	//++__dbg_calls["get_chtype"];
	return abs(p.type) + 'A'-1;
}

/** Zwraca dostępne rotacje. */
inline char get_chrots(const node &p) {
	//++__dbg_calls["get_chrots"];
	return p.rot;
}

/** Zwraca rotację.
 * @pre @verbatim is_set(p) @endverbatim */
inline char get_chrot(const node &p) {
	//++__dbg_calls["get_chrot"];
	char ret[] = {'0','1','2','2','3','3','3','3','4'};
	return ret[(int)p.rot];
	/*if(p.rot > 2) return p.rot==4?'3':'4';
	else          return p.rot==2?'2':'1';*/
}

/** Usuwa jedną z możliwości rotacji.
 * @param r Rotacja (naturalnie). */
inline void rem_rotation(node &p, char r) {
	//++__dbg_calls["rem_rotation"];
	p.rot &= (0xF - (1<<r));
	if(rot2numrots[(int)p.rot] <= 1)
		make_set(p);
}

/** Czy element połączony z sąsiadem.
 * @param p Pozycja (1D).
 * @param bdir Kierunek (binarnie).
 * @return Pozycja sąsiada gdy połączony lub -1. */
inline int connects_in_bdir(node* brd, uint p, uint bdir) {
	int cnntd;
	char conns = rotate_bin_wbin(type2conns[(int)brd[p].type], brd[p].rot>>1);
	return ((bdir&conns)                                                // jeśli dany element ma przewód w tym kierunku
		&& (cnntd = pos_goto(p,bdir)) >= 0                              // i na końcu przewodu znajduje się element
		&& is_set(brd[cnntd])                                           // który został już ustawiony
		&& does_connects(brd[cnntd], rotate_bin(bdir,2))) ? cnntd : -1; // i ma przewód do którego się można stąd podłączyć
}
//********************************************************************************
//********************************************************************************
//********************************************************************************

typedef unsigned int uint;
typedef unsigned char uchar;


uint batt_pos;        /**< Pozycja baterii. */
node **board;         /**< Reprezentacja planszy. */
node *brd;            /**< brd = (*board). */
vector<node*> lbulbs; /**< Zbiór wszystkich żarówek. */
uint best_so_far;	  /**< Najlepsze znalezione rozwiązanie - ilość żarówek. */
char* snapshot_best;  /**< Najelpsze znalezione rozwiązanie - zapis. */

/*map<const char*, uint, eqstr> __dbg_calls;
vector<uint> __dbgbacktracklvls;

void //__dbgprint(const char*, int elem = -1, uint shift = 0);*/

inline uint bulbs_connected();
void read_board();
void print_board();
void h1();
void backtrack(uint);
void savesnapshot();
void restoresnapshot();


/** Główna metoda. */
int main() {
	read_board();
	//__dbgprint("Po wczytaniu i H2");
	h1();
	//__dbgprint("Po H1");

	char opts = get_chrots(brd[batt_pos]), // możliwości obrotu
		 pr,                               // priorytet operacji unionW
		 //conns,                            // dopuszczalne kierunki wychodzące
		 j,                                // kierunek wychodzący
		 i;                                // dany obrót
	uint p;                                // element w poprawianiu poheurezowym
	int cnntd;                             // sąsiad

	for(p=0;p<XY;++p)
		if(is_set(brd[p])) {
			//conns = rotate_bin_wbin(type2conns[(int)brd[p].type], brd[p].rot>>1);
			for(j=1; j<=8; j<<=1)                   // dla każdego kierunku wychodzącego:
				if((cnntd = connects_in_bdir(brd, p,j)) >= 0) {
					//++__dbg_calls["node::unionW"];
					brd[cnntd].unionW(brd[p], -10); // połącz element bieżący w danym kierunku
				}
		}
	//__dbgprint("Po poprawianiu linków");

	// backtrack
	make_set(brd[batt_pos]);
	//__dbgbacktracklvls.push_back(0);
	for(i=1; i<=8; i<<=1) { // dla każdego możliwego obrotu:
		if(! (i & opts) )   // jeśli dany obrót jest dopuszczalny
			continue;
		//++__dbgbacktracklvls[0];
		pr = 5;
		brd[batt_pos].rot = i;   // ustal obrót
		//conns = rotate_bin_wbin(type2conns[(int)brd[batt_pos].type], i>>1);
		for(j=1; j<=8; j<<=1) {                                      // dla każdego kierunku wychodzącego:
			if((cnntd = connects_in_bdir(brd, batt_pos,j)) >= 0) {
				//++__dbg_calls["node::unionW"];
				brd[cnntd].unionW(brd[batt_pos], --pr);              // połącz element bieżący w danym kierunku
			}
		}
		//__dbgprint("Backtrack: pozycja baterii", batt_pos, 0);
		backtrack(1);

		if(pr != 5) {
			//++__dbg_calls["node::backtrack"];
			node::backtrack();
		}
	}

	restoresnapshot();
	print_board();

	/*
	printf("Call statistics:________________________________________\n");
	uint __dbg_sumcalls = 0;
	for(map<const char*, uint, eqstr>::iterator it = __dbg_calls.begin(); it != __dbg_calls.end(); ++it)
		__dbg_sumcalls += (*it).second;
	for(map<const char*, uint, eqstr>::iterator it = __dbg_calls.begin(); it != __dbg_calls.end(); ++it) {
		printf("  * \t%-20s #%8d (%5.2f)    ", (*it).first, (*it).second, 100.0*(*it).second/__dbg_sumcalls);
		uint bars = (uint)(50.0*(*it).second/__dbg_sumcalls);
		while(bars-- > 0) printf("|");
		printf("\n");
	}
	printf("Backtrack statistics:___________________________________\n");
	uint __dbgtmp = 0;
	uint lll;
	for(vector<uint>::iterator it = __dbgbacktracklvls.begin(); it!=__dbgbacktracklvls.end(); ++it, ++__dbgtmp) {
		printf("  %2d  --  %2d ", __dbgtmp, *it);
		for(lll=0; lll<(*it); ++lll) printf("|");
		printf("\n");
	}*/
	return 0;
}


void backtrack(uint depth) {
	//++__dbg_calls["backtrack"];
	//if(__dbgbacktracklvls.size() < depth) __dbgbacktracklvls.push_back(0);
	//++__dbgbacktracklvls[depth];

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
	char opts = get_chrots(brd[p]),
		 pr,
		 //conns,
		 j,
		 prerot = brd[p].rot;
	int cnntd;
	make_set(brd[p]);
	for(char i=1; i<=8; i<<=1) {
		if(! (i & opts) ) continue;
		brd[p].rot = i;
		pr = 5;
		//conns = rotate_bin_wbin(type2conns[(int)brd[p].type], i>>1);
		for(j=1; j<=8; j<<=1) {                                      // dla każdego kierunku wychodzącego:
			if((cnntd = connects_in_bdir(brd, p,j)) >= 0) {
				//++__dbg_calls["node::unionW"];
				brd[cnntd].unionW(brd[p], --pr);                     // połącz element bieżący w danym kierunku
			}
		}
		//__dbgprint("Backtrack", p, depth);
		backtrack(depth+1);

		if(pr != 5) {
			//++__dbg_calls["node::backtrack"];
			node::backtrack();
		}
	}
	make_unset(brd[p]);
	brd[p].rot = prerot;
}


/** Wczytujemy wierszami, maksymalnie 1500 znaków na raz (oraz dodatkowo znak końca łańcucha).
 * @warning Zakładamy, że na wejściu nie ma zbędnych białych znaków. */
#define INPUTLEN 1502

/** Wczytywanie planszy.
 * Alokuje reprezentację; ustawia zmienne ::X, ::Y i ::XY. Znajduje baterię (ustala ::batt_pos).
 * Uruchamia heurezę H2. */
void read_board() {
	char input[INPUTLEN];
	scanf(" %u %u ", &X, &Y);
	XY = X*Y;

	snapshot_best = new char[XY];
	// Podwójna notacja: board[x][y] <=> brd[i] dla i == X*y + x
	board = new node*[Y];
	brd = (*board) = new node[XY];
	for(uint i=0;i<Y;++i) board[i] = *board + i*X;

	uint board_pos = 0;
	char *inp;
	while(fgets(input,INPUTLEN,stdin) != NULL) {
		for(inp=input; *inp; inp+=3, ++board_pos) { // ustawianie .type .rot
			if((*inp) == 'H' || ('D' <= (*inp) && (*inp) <= 'F')) // szukanie baterii
				batt_pos = board_pos;
			else if((*inp) == 'I')                                // szukanie żarówek
				lbulbs.push_back(brd+board_pos);
			brd[board_pos].type = 'A' - (*inp) - 1;
			switch(*inp) {
				case 'A': case 'D': make_set(brd[board_pos],1<<0); break;
				case 'B': case 'E': brd[board_pos].rot = 0x3; break;
				case 'I':			// H2.
						  			// Wykonuje redukcje na styku żarówka-żarówka.
									brd[board_pos].rot = 0xF;
									if(inp > input && inp[-3]=='I') { // poziomo
										rem_rotation(brd[board_pos-1] ,0);
										rem_rotation(brd[board_pos]   ,2);
									}
									if(board_pos>X && get_chtype(brd[board_pos-X])=='I') { // pionowo
										rem_rotation(brd[board_pos-X] , 1);
										rem_rotation(brd[board_pos]   , 3);
									}
									break;
				default:            brd[board_pos].rot = 0xF;
			}
		}
	}
}


/** Wyświetlanie planszy. Please disperse. There's nothing for you to see here. Keep moving. */
void print_board() {
	printf("%d %d\n", X,Y);
	uint x, y;
	node *b = brd;
	for(y=0;y<Y;++y) {
		printf("%c%c", get_chtype(board[y][0]), get_chrot(board[y][0]));
		for(x=1;x<X;++x,++b)
			printf(" %c%c", get_chtype(board[y][x]), get_chrot(board[y][x]));
		printf("\n");
	}
}


/* Wyświetlanie planszy w trybie debug. */
/*void __dbgprint(const char *str, int elem, uint shift) {
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
}*/

/** Heureza H1. Ustala sytuacje na krawędziach. */
void h1() {
	uint x,y,i=0;
	char cornern[] = {0,1,3,2}; // Mały helper: przekształcenie NW NE SW SE -> NW NE SE SW.
	// rogi
	for(y=0; y<Y; y+=Y-1)
		for(x=0;x<X; x+=X-1, ++i)
			switch(get_chtype(board[y][x])) {
				case 'B':           make_set(board[y][x],1<<0);			 break ; // martwy node
				case 'C': case 'F':
				case 'G': case 'H': make_set(board[y][x],1<<cornern[i]); break ;
				case 'I':           rem_rotation(board[y][x], (cornern[i]+3)&0x3);
									rem_rotation(board[y][x], (cornern[i]+2)&0x3);
				default: ;
			}
	for(i=0,y=0;y<Y;y+=Y-1,++i) // boki góra/dół
		for(x=X-2;x>0;--x)
			switch(get_chtype(board[y][x])) {
				case 'B':           make_set(board[y][x],1);           break ;
				case 'C': case 'F': rem_rotation(board[y][x],(i+2)&3);
									rem_rotation(board[y][x],(i+3)&3); break ;
				case 'G': case 'H': make_set(board[y][x],i<<1);        break ;
				case 'I':           rem_rotation(board[y][x],1|(2>>i));
				default:            ;
			}
	for(i=0,x=0;x<X;x+=X-1,++i) // boki lewo/prawo
		for(y=Y-2;y>0;--y)
			switch(get_chtype(board[y][x])) {
				case 'B':           make_set(board[y][x],2);             break ;
				case 'C': case 'F': rem_rotation(board[y][x],1^i);
									rem_rotation(board[y][x],2^i);       break ;
				case 'G': case 'H': make_set(board[y][x],1<<(1|(2>>i))); break ;
				case 'I':           rem_rotation(board[y][x],2-(i<<1));
				default:            ;
			}
}

/** Zwraca ilość podłączonych żarówek. */
inline uint bulbs_connected() {
	//++__dbg_calls["bulbs_connected"];

	uint ret = 0;
	node *SRC = brd[batt_pos].find();
	for(vector<node*>::iterator it = lbulbs.begin(); it != lbulbs.end(); ++it)
		if((*it)->find() == SRC && is_set(**it))
			++ret;
	return ret;
}

void savesnapshot() {
	for(uint i=0;i<XY;++i)
		snapshot_best[i] = brd[i].rot;
}

void restoresnapshot() {
	for(uint i=0;i<XY;++i)
		brd[i].rot = snapshot_best[i];
}
