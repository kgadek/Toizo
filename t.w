%\font\body="DejaVu Sans" at 6pt \body

@ Rozwiązanie zadania.

@c
#include <cstdio>
#include <cassert>
@< Definicje urządzeń @>@;
@< Definicje struktur @>@;
int posBat; // wskazanie na pozycję baterii
@< Zmienne globalne i deklaracje funkcji @>@;
@< Procedura |main| @>@;
@< Definicje funkcji @>@;



@ @< Definicje urządzeń @>=
char rotations[] = {1, 2, 4, 1, 2, 4, 4, 4, 4};



@ Początek programu.

Najważniejsza część |main| to
{
\parindent = 45 pt
\item{1. } wstępne przetworzenie tablicy
\item{2. } rozpoczęcie backtrackingu od wyboru obrotu baterii.
}

@< Procedura |main| @>=
int main() {
	@< Wczytanie planszy @>@;
    @< Preprocessing @>@;
    char batType = board[0][posBat].type;
    board[0][posBat].type = -1;
    for(int i=0; i<rotations[batType]; ++i) {
        board[0][posBat].rot = i;
        backtrack(1); // zaczynamy od 1 bo wybraliśmy jeden klocek
    }
    board[0][posBat].type = batType;
	@< Wyświetlenie rozwiązania @>@;
}



@ Urządzenie.

@d nodeTypeRotMod ('I'-'A'+1)
@< Definicje struktur @>=
struct node {
	char type; // typ
    char rot; // ustalony obrót |@t$\le$@>rotations[type]|
};



@* Operacje I/O.

@< Zmienne globalne i deklaracje funkcji @>=
int X, Y;
int XY; // |XY @t$=$@> X*Y|
node **board;



@ Wczytywanie planszy.

Tak naprawdę nieistotne są początkowe obroty elementów. Jedyne co nas interesuje to typ (patrz: |enum blockType|). Redukuje to ilość typów komórek do 9 ('A' - 'I').

Wczytujemy całymi wierszami -- dość szybkie, a kosztuje nas to $1500 = 1.46$ KiB. Uwaga -- założyłem, że na początku i na końcu wiersza nie ma białych znaków.

Przy okazji znajdujemy pozycję baterii (zapisujemy w zmiennej |posBat@t$\colon\ $@> board[0][posBat]| wskazuje na baterię).

@d INPUTLEN 1500
@< Wczytanie planszy @>=
char input[INPUTLEN];

scanf(" %d %d ", &X, &Y); // Rozmiar planszy: |X| kolumn, |Y| wierszy
printf("%d %d\n", X, Y); // od razu wypisujemy -- why not?
XY = X*Y;
board = new node*[Y];
(*board) = new node[XY];
for(int i=1; i<Y; ++i)
	board[i] = *board+i*X;

int boardPos = 0; // pozycja w tablicy |board[0]|. Magicznie wypełniamy wszystkie wiersze i kolumny :P
int inpPos = 0; // pozycja w |input|
while( fgets(input, INPUTLEN, stdin) != NULL)
	for(inpPos = 0; input[inpPos] != 0; inpPos += 3) {
        if(input[inpPos] == 'H' || ( 'D' <= input[inpPos] && input[inpPos] <= 'F'))
            posBat = boardPos;
		board[0][boardPos].type = input[inpPos]-'A';
		board[0][boardPos++].rot = 0;
    }



@ @< Wyświetlenie rozwiązania @>=
for(int yy = 0; yy<Y; ++yy) {
	printf("%c%d", board[yy][0].type + 'A', board[yy][0].rot+1);
	for(int xx = 1; xx<X; ++xx)
		printf(" %c%d", board[yy][xx].type + 'A', board[yy][xx].rot+1);
	printf("\n");
}



@* Szukanie rozwiązania.

@< Zmienne globalne i... @>=
void backtrack(int);



@* Backtracking.

Parametr |d| to wartość ustalonych urządzeń.

@< Definicje funkcji@>=
void backtrack(int d) {
    /*dbg*/for(int i=0;i<d;++i) putc('>', stdout);
    /*dbg*/printf(" backtrack(%d)\n", d);
    if(d == XY) {
        // TODO: save solution?
        return;
    }
    int selected;
    @<Select@>@;
    /*dbg*/for(int i=0;i<d;++i) putc('>', stdout);
    /*dbg*/printf(" selected pos=%d\n", selected);
    char selType = board[0][selected].type;
    board[0][selected].type = -1;
    for(int i=0; i<rotations[selType]; ++i) {
        board[0][selected].rot = i;
        backtrack(d+1);
    }
    board[0][selected].type = selType;
    /*dbg*/for(int i=0;i<d;++i) putc('<', stdout);
    /*dbg*/printf(" backtrack(%d)\n", d);
}



@ Wybieranie.

@< Select @>=
for(selected=0; board[0][selected].type == -1; ++selected) ;


@ Heurystyki: preprocessing.

@<Preprocessing@>=
;
