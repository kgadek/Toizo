#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <algorithm>

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
	T* self() { return static_cast<T*>(this); }
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
		if( self()->usH[pU.first].second == pU.second )
			return true;
		p = self();
		return false;
	}

	/** Zwraca rank. Jeśli trzeba to redukuje wartość rP. */
	uint getRank() {
		if(rS == 0) return 1;
		int l = -1, u = rP+1, i;
		for(i = rP>>1; l+1<u; i=(l+u)>>1)
			if( self()->usH[rL[i].second.first].second == rL[i].second.second )
				l = i;
			else u = i;
		if(l==-1)
			return 1;
		rP = l+1;
		return rL[l].first;
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
		T *pp = find(), // restoring phase
			*qq = oo->find();
		if(pp->getRank() > qq->getRank())
			swap(pp,qq); // od teraz pp jest mniejsze niż qq
		uint ppr = pp->getRank(),
			 qqr = qq->getRank();
		if(!self()->usHP || prio > self()->usH[self()->usHP-1].first) { // linking phase
			if(!self()->usH)
				self()->usH = (stampedUnion*)realloc(self()->usH, (self()->usHS = (self()->usHS+1)<<1)*sizeof(stampedUnion));
			self()->usH[self()->usHP] = stampedUnion(self()->usHP++, self()->usST++);
		}
		stampedUnion L = stampedUnion(self()->usHP, self()->usST);
		pp->p = qq;
		pp->pU = L;
		if(qq->rP == qq->rS)
			qq->rL = (stampedRank*)realloc(qq->rL, (qq->rS = (qq->rS+1)*2)*sizeof(stampedRank));
		qq->rL[(qq->rP)++] = stampedRank(ppr+qqr,L); // rank updating phase
	}

	/** Cofanie. Anuluje tyle cofnięć, by union o najwyższym priorytecie przestał obowiązywać. */
	static void backtrack() {
		if(T::usHP > 0)
			T::usHP--;
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

uint NN = 20;
uint RR = 30;
std::pair<int,int> *RRS;
int *RRD;

int* mksnap(test* tab) {
	int* nt = new int[NN];
	for(uint i=0;i<NN;++i)
		nt[i] = tab[i].find()->x;
	return nt;
}
bool snapeq(int *a, int *b) {
	for(uint i=0;i<NN;++i)
		if(a[i]!=b[i])
			return false;
	return true;
}
void mktest(uint it, test *tt, int beforePrio = 0) {
	if(it >= RR)
		return;
	int* before = mksnap(tt);
	printf(">> %2d: %3d <-> %3d #%2d%s:", it, RRS[it].first, RRS[it].second, RRD[it], RRD[it] >= beforePrio ? " !!" : "   ");
	for(uint i=0;i<NN;++i) printf(" %2d",tt[i].find()->x);
	printf("\n                          ");
	tt[RRS[it].first].unionW(tt+RRS[it].second, RRD[it]);
	for(uint i=0;i<NN;++i) printf(" %2d",tt[i].find()->x);
	printf("\n");

	mktest(it+1, tt, std::max(beforePrio, RRD[it]));

	if(RRD[it] >= beforePrio) {
		printf("<< %2d\n",it);
		test::backtrack();
		int* after = mksnap(tt);
		if(! snapeq(before, after)) {
			printf("   !! NO :< %d\n    ", it);
			for(uint i=0;i<NN;++i) printf(" %2d",before[i]);
			printf("\n    ");
			for(uint i=0;i<NN;++i) printf(" %2d",after[i]);
			printf("\n");
		}
	}
	return;
}

int main() {
	srandom(time(NULL));
	test *tab = new test[NN];
	for(uint i=0;i<NN;++i) tab[i].x=i;
	RRS = new std::pair<int,int> [RR];
	RRD = new int [RR];
	for(uint i=0;i<RR;++i) {
		RRS[i] = std::pair<int,int>(random()%NN, random()%NN);
		RRD[i] = ((unsigned int)random())%NN;
		printf("(%d,%d |%d) ", RRS[i].first, RRS[i].second, RRD[i]);
	}
	printf("\n");

	mktest(0, tab);

	delete [] tab;
	return 0;
}

