#include "n.h"
#include "quickcheck/quickcheck/quickcheck.hh"

using namespace quickcheck;

void quickcheck::generate(size_t n, node& out) {
	bool is_set;
	generate(n,is_set);

	char type, rot;
	generate(n, type);
	generate(n, rot);
	type = type%('I'-'A')+1;
	if(is_set)
		rot = 1<<((rot%3)+1);
	else {
		type = -type;
		rot = rot%0xF;
	}
}

class PMetaprop : public Property<node> {
	bool holdsFor(const node &n) {
		if(is_set(n)) {
		}
		return false;
	}
};

int main() {
	PMetaprop pm;
	pm.check();
	return 0;
}

