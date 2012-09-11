CXX         =   g++
CXXFLAGS    =   -O0 -g -ggdb3 -pg\
                -W -Wall -Wshadow -Wformat\
                -Wsequence-point -Wunused -Wchar-subscripts\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

.PHONY: clean doc all ctags

all: t doc

ctags:
	ctags -R --exclude=.git --exclude=doc --exclude=docs --exclude=quickcheck

clean:
	rm -f t tags gmon.txt gmon.out
	rm -rf doc

doc: t.cpp
	doxygen

