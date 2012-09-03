#include <cstdio>
#include <cstdlib>
#include <utility>

typedef std::pair<int,int> stampedUnion;
typedef unsigned int uint;
typedef std::pair<int,stampedUnion> stampedRank;

/** Po prostu swap.
 */
template <class T>
void swap(T &a, T &b) {
	T tmp = a;
	a = b;
	b = tmp;
}

/** Klasa bazowa dla operacji union-set. */
template <class T> // oppa oppa mixin style!
class set {
	/** Helper, przydatny przy CRTP; zwraca pointer na bieżący obiekt */
	T* self() const { return static_cast<T*>(this); }
public:
	/** Konstruktor, który niewiele robi. */
	set() : rL(NULL), rS(0), rP(0) { p = static_cast<T*>(this); }

	T* p; /**< Link na node nadrzędny. */
	stampedUnion pU; /**< Wskazanie na opis operacji. Pozwala stwierdzić, czy link jest żywy. */
	stampedRank *rL; /**< Lista ranków. Jest listą par (r,u) gdzie r to rank zaraz po wykonaniu union u. */
	uint rS; /**< Lista ranków - rozmiar listy. */
	uint rP; /**< Lista ranków - pointer na koniec listy. */

	/** Czy link wychodzący jest żywy. Jeśli nie to go usuwa. */
	bool isLinkAlive() {
		if( self()->usH[pU.first] == pU.second )
			return true;
		p = self();
		return false;
	}

	/** Zwraca rank. Jeśli trzeba to redukuje wartość rP. */
	uint getRank() {
		if(rP==0)
			return 1;
		uint i = 0,
			 j = rP;
		for(uint m = j>>1; i<j; m=(i+j)>>2)
			if( self()->usH[rL[m].second.first] == rL[m].second.second )
				i = m;
			else j = m;
		return rL[rP = i].first;
	}

	/** Wyszukiwanie reprezentanta zbioru. Przy okazji - implicite, używając isLinkAlive() - usuwa martwe linki. */
	T* find() {
		T* n = self();
		while(n->p != n && n->isLinkAlive())
			n = n->p;
		return n;
	}

	/** Operacja union o priorytecie prio między bieżącym elementem a oo. */
	void unionW(T* oo, int prio) {
		T* pp = find(), // restoring phase
			qq = oo->find();
		if(pp->getRank > qq->getRank)
			swap(pp,qq); // od teraz pp jest mniejsze niż qq
		uint ppr = pp->getRank(),
			 qqr = qq->getRank();
		if(prio > self()->usH[self()->usHP]) // linking phase
			self()->usH[self()->usHP] = stampedUnion(self()->usHP++, self()->usST++);
		stampedUnion L = stampedUnion(self()->usHP, self()->usTS);
		pp->p = qq;
		pp->pU = L;
		if(rP == rS)
			rL = (stampedRank*)realloc(rL, (rS = (rS+1)<<1)*sizeof(stampedRank));
		qq->rL[qq->rP++] = ppr+qqr; // rank updating phase
	}

	/** Cofanie. Anuluje tyle cofnięć, by union o najwyższym priorytecie przestał obowiązywać. */
	void backtrack() {
		if(self()->usHP > 0)
			self()->usHP--;
	}

};

class test : public set<test> {
public:
	static stampedUnion *usH;
	static uint usHS; // size
	static uint usHP; // pointer
	static uint usST; // stamp

	int x;
};
stampedUnion* test::usH = NULL;
uint test::usHP = 0;
uint test::usHS = 0;
uint test::usST = 0;

int main() {
	int n = 10;
	test *tab = new test[n];
	for(int i=0;i<n;++i) tab[i].x=i;

	//...

	delete [] tab;
	return 0;
}

