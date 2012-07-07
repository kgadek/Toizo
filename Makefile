CXX         =   clang++
CXXFLAGS    =   -O0 -g\
                -W -Wall -Wshadow -Wformat \
                -Wsequence-point -Wunused\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

c:

t: t.w
	cweave t.w && xetex t.tex
	ctangle t.w - t.cpp && clang++ -W -Wall -Wshadow\
		-Wformat -Wsequence-point -Wunused\
		-Wuninitialized -Wfloat-equal -ansi\
		-pedantic -Weffc++ -g -O0 -lm t.cpp -o t
texclean:
	rm -f t.c t.idx t.log t.scn t.tex t.toc 
progclean:
	rm -f t
clean: texclean progclean
	rm c

hardclean: clean
	rm -f t.pdf t.dvi t.cpp
