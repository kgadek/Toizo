#include <cstdio>
#include <ctime>
#include <memory>

using namespace std;

#define TSIZE 1000000

typedef unsigned int uint;

template <class T>
class cntr {
public:
	static uint cntrr;
	cntr() { ++(cntr<T>::cntrr); }
	uint getCnt() { return T::cntrr; }
};
template <class T>
uint cntr<T>::cntrr = 0;

class Maybe : cntr<Maybe> {
	uint *v;
public:
	Maybe() : v(0) {}
	Maybe(uint *val) : v(val) {}

	Maybe(Maybe &other) : v(other.v) { other.v = 0; }
	Maybe& operator=(Maybe &other) {
		delete v;
		v = other.v;
		other.v = 0;
		return *this;
	}

	Maybe& operator>=(Maybe& (*f)(uint&)) {
		if(!isNothing())
			(*this) = f(*v);
		return *this;
	}

	bool isNothing() const { return !v; }
	uint operator*() { return *v; }
	~Maybe() { delete v; v=0; }
	static Maybe& ret(uint *v = 0) { return *(new Maybe(v)); }
};

uint add3u(uint x) { return x+3; }
uint sub3u(uint x) { return x-3; }

Maybe& add3(uint &x) {
	return Maybe::ret(new uint(x+3));
}
Maybe& sub3(uint &x) {
	if(x<3) return Maybe::ret();
	x -= 3;
	return Maybe::ret(new uint(x));
}

int main() {
	{
		Maybe m(new uint(3));
		Maybe m2(m);
		unique_ptr<uint> lol(new uint(20));
		int b, a = time(0);
		for(int i=TSIZE;i>=0;--i) {
			m2 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3 >= add3 >= sub3;
		}
		b = time(0);
		if(m2.isNothing()) printf("Nothing - %d\n", b-a);
		else printf("Just %2d - %d\n", *m2,b-a);
		printf("... %d Maybe's allocated btw\n", cntr<Maybe>::cntrr);

	}
	{
		uint m(3);
		uint m2=m;
		int b, a = time(0);
		for(int i=TSIZE;i>=0;--i) {
		m2 = add3u(m);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		m2 = add3u(m2);
		if(m2>=3) m2 = sub3u(m2);
		}
		b = time(0);
		printf("Just %2d - %d\n",m2,b-a);
	}
}

