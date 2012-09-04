#include <cstdio>
#include <vector>
#include <cassert>

typedef unsigned int uint;
typedef std::pair<int,int> stampedUnion; /**< Struktura wskazująca na operację union: id na stosie  x  stamp. */
typedef std::pair<int,int> unionDescr; /**< Struktura opisująca operację union na stosie:  priorytet  x  stamp. */
typedef std::pair<stampedUnion, int> stampedRank; /**< Struktura: wskazanie na union  x  wysokość drzewa następująca po wskazanej danej operacji union. */

/** Klasa bazowa dla operacji union-set.
 * By użyć w dowolnej klasie Z, ta klasa musi
 *   -# dziedziczyć publicznie po set<Z>
 *   -# zadeklarować dwie zmienne statyczne:
 *   	\code
 *   	unsigned int setStamp;
 *   	std::vector<unionDescr> setH;
 *   	\endcode
 */
template <class T>
class set {
	T* p; /**< Link na node nadrzędny. */
	stampedUnion pU; /**< Wskazanie na opis operacji. Pozwala stwierdzić, czy link jest żywy. */
	std::vector<stampedRank> rL; /**< Lista ranków. Jest listą par (r,u) gdzie r to rank zaraz po wykonaniu union u. */

	/** Czy link wychodzący jest żywy, jeśli nie to go usuwa. Zakłada, że p != NULL. */
	bool isLinkAlive() {
		return ( pU.first < (int)T::setH.size() && T::setH[pU.first].second == pU.second ) // jeśli link żywy to zwróci true
			|| (p = NULL); // jeśli nie to zwróci NULL == false
	}

	/** Zwraca rank. Jeśli trzeba to skraca rL. */
	uint getRank() { // O(log log n)
		int l = -1, u = rL.size(), i;
		for(i = (l+u)>>1; l+1<u; i=(l+u)>>1)
			if( T::setH[rL[i].first.first].second == rL[i].first.second )
				l = i;
			else u = i;
		rL.resize(l+1);
		if(l==-1) return 1;
		return rL.back().second;
	}

	/** Swap. Działa, o ile a i b to różne obiekty. */
	template <class X> void swap(X &a, X &b) {
		X t = a;
		a = b; b = t;
	}

public:
	/** Konstruktor, który niewiele robi. */
	set() : p(NULL) {}

	/** Wyszukiwanie reprezentanta zbioru. Przy okazji - implicite, używając isLinkAlive() - usuwa martwe linki. */
	T* find() { // O(log n)
		T* n = static_cast<T*>(this);
		while(n->p != NULL && n->isLinkAlive())
			n = n->p;
		return n;
	}

	/** Operacja union o priorytecie prio między bieżącym elementem a oo. */
	void unionW(T* oo, int prio) { // O(log n)
		T *pp = find(), // restoring phase
			*qq = oo->find();
		if(pp == qq) return;
		uint ppr = pp->getRank(),
			 qqr = qq->getRank();
		if(ppr > qqr) {
			swap(ppr,qqr);
			swap(pp,qq);
		}
		if(T::setH.empty() || prio > T::setH.back().first) // linking phase
			T::setH.push_back(unionDescr(prio, T::setStamp++));
		pp->p = qq;
		pp->pU = stampedUnion(T::setH.size()-1, T::setH.back().second);
		qq->rL.push_back(stampedRank(pp->pU, std::max(ppr+1,qqr))); // rank updating phase
	}

	/** Cofanie. Anuluje tyle cofnięć, by ostatni union o najwyższym priorytecie przestał obowiązywać. */
	static void backtrack() {
		if(!T::setH.empty())
			T::setH.pop_back();
	}

};

class test : public set<test> {
public:
	static std::vector<unionDescr> setH;
	static uint setStamp;
};
std::vector<unionDescr> test::setH = std::vector<unionDescr>();
uint test::setStamp = 0;


typedef std::vector<test>::iterator testItrt;

template <class T>
struct quadr {
	T first, second, third, fourth;
};

void comparator(std::vector<test> &tab, uint s, const int cmp[]) {
	for(uint i=0;i<s;++i)
		assert(tab[i].find() == &tab[cmp[i]]);
}

int main() {
	const uint TABSIZE = 6;
	std::vector<test> tab(TABSIZE);
	comparator(tab, TABSIZE, (int []){0,1,2,3,4,5});
	{
		tab[0].unionW(&tab[1], 5);
		comparator(tab, TABSIZE, (int []){1,1,2,3,4,5});
		{
			tab[2].unionW(&tab[3], 4);
			comparator(tab, TABSIZE, (int []){1,1,3,3,4,5});
			{
				tab[0].unionW(&tab[2], 6);
				comparator(tab, TABSIZE, (int []){3,3,3,3,4,5});
				{
					tab[4].unionW(&tab[5], 1);
					comparator(tab, TABSIZE, (int []){3,3,3,3,5,5});
					{
						tab[1].unionW(&tab[5], 6);
						comparator(tab, TABSIZE, (int []){3,3,3,3,3,3});
						test::backtrack();
					}
				}
				test::backtrack();
			}
		}
		test::backtrack();

		tab[5].unionW(&tab[3], 1);
		comparator(tab, TABSIZE, (int []){0,1,2,3,4,3});
		{
			tab[2].unionW(&tab[3], 1);
			comparator(tab, TABSIZE, (int []){0,1,3,3,4,3});
			test::backtrack();
		}
		test::backtrack();
	}
	comparator(tab, TABSIZE, (int []){0,1,2,3,4,5});
	return 0;
}

