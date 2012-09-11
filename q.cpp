#include <cstdio>
#include <vector>
#include "quickcheck/quickcheck/quickcheck.hh"

using namespace quickcheck;
using namespace std;

class PReverseSquare : public Property< vector<int> > {
	bool holdsFor(const vector<int>& xs) {
		vector<int> ys = xs;
		return xs == ys;
	}
	bool isTrivialFor(const vector<int>& xs) {
		return xs.size() < 2;
	}
	const string classify(const vector<int>& xs) {
		if(xs.size() < 1) return "empty";
		if(xs.size() < 2) return "singletons";
		if(xs.size() < 4) return "le 4";
		return "empty";
	}
	void generateInput(size_t n, vector<int>& xs) {
		int t;
		generate(n, t);
		xs.push_back(t);
	}
};

int main() {
	PReverseSquare prs;
	prs.check();
	return 0;
}

