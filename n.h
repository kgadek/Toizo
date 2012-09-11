#ifndef _SET_H
#define _SET_H
#include "Unionfind-cpp/set.h"

using namespace std;

extern uint X;
extern uint Y;
extern uint XY;
/*struct eqstr { bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) < 0; } };
extern map<const char*, uint, eqstr> __dbg_calls;*/

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

#endif
