#include <cstdio>
#include <vector>
#include <algorithm>

typedef unsigned int uint;

template <class T>
class binheap {
public:
	static std::vector<T*> heap;
	void insert() {
		uint i = T::heap.size(),
			 j;
		T::heap.push_back(this);
		while(i>0 && *T::heap[j=i<<1] < *T::heap[i]) {
			std::swap(T::heap[j], T::heap[i]);
			i <<= 1;
		}
	}
	static T* findmin() {
		uint s;
		if(s = T::heap.size() < 1) return NULL;
		T* res = T::heap[0];
		T::heap[0] = T::heap.back();
		T::heap.pop_back();
		--s;
		uint i = 0;
		while(i<s) {
			//TODO
		}
		return res;
	}
};
template <class T>
std::vector<T*> binheap<T>::heap = std::vector<T*>();

struct tbin : public binheap<tbin> {
	uint v;
	tbin(uint x) : v(x) {}
	bool operator<(const tbin &o) { return v < o.v; }
};

int main() {
	return 0;
}

