#include <cstdio>
#include <cassert>
#include <utility>
#include "set.h"

struct test : public set<test> { };

void comparator(std::vector<test> &tab, uint s, const int cmp[]) {
	for(uint i=0;i<s;++i)
		assert(tab[i].find() == &tab[cmp[i]]);
}

int main() {
	const uint TABSIZE = 6;
	std::vector<test> tab(TABSIZE);
	comparator(tab, TABSIZE, (int []){0,1,2,3,4,5});
	{
		tab[0].unionW(tab[1], 5);
		comparator(tab, TABSIZE, (int []){1,1,2,3,4,5});
		{
			tab[2].unionW(tab[3], 4);
			comparator(tab, TABSIZE, (int []){1,1,3,3,4,5});
			{
				tab[0].unionW(tab[2], 6);
				comparator(tab, TABSIZE, (int []){3,3,3,3,4,5});
				{
					tab[4].unionW(tab[5], 1);
					comparator(tab, TABSIZE, (int []){3,3,3,3,5,5});
					{
						tab[1].unionW(tab[5], 6);
						comparator(tab, TABSIZE, (int []){3,3,3,3,3,3});
						test::backtrack();
					}
				}
				test::backtrack();
			}
		}
		test::backtrack();

		tab[5].unionW(tab[3], 1);
		comparator(tab, TABSIZE, (int []){0,1,2,3,4,3});
		{
			tab[2].unionW(tab[3], 1);
			comparator(tab, TABSIZE, (int []){0,1,3,3,4,3});
			test::backtrack();
		}
		test::backtrack();
	}
	comparator(tab, TABSIZE, (int []){0,1,2,3,4,5});
	return 0;
}

