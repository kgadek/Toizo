#include <cstdio>
#include <cstdlib>

/**
 * Klasa bazowa: union-set.
 *
 * By użyć, dana klasa X musi dziedziczyć set<X> i publicznie zadeklarować static int zz.
 */
template <class T> // oppa oppa mixin style!
class set {
public:
	T* p; // link
	int getzz() {
		return T::zz;
	}
	/*int troll() {
		return printf("trolling is a art\n");
	}
	T* self() {
		return static_cast<T*>(this);
	}
	void unionWith(set&, const int); //!< O(log log n)
	set& find(); //!< O(log n)
	void backtrack(); //!< O(1)*/
};

class test : public set<test> {
public:
	static int zz;
	int x;
};
int test::zz = 0;

/**
 * Rotfl.
 * 
 * Lool.
 */
int main() {
	int n = 10;
	test *tab = new test[n];
	for(int i=0;i<n;++i) tab[i].x=i;
	printf("%d\n", tab[0].getzz());

	//...

	delete [] tab;
	return 0;
}

