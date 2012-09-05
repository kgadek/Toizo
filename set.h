#ifndef __SET_H
#define __SET_H

#include <cstdio>
#include <utility>
#include <vector>

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
		if(T::uH.empty() || prio > T::uH.back().first) // linking phase
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

#endif
