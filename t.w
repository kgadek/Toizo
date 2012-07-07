%\font\body="DejaVu Sans" at 6pt \body

@ Rozwiązanie zadania.

@c
#include <cstdio>
#include <cassert>
@< Definicje struktur @>@;
@< Zmienne globalne i deklaracje funkcji @>@;
@< Procedura |main| @>@;
@< Definicje funkcji @>@;



@ @< Procedura |main| @>=
int main() {
	@< Wczytanie planszy @>@;
    backtrack(0);
	@< Wyświetlenie rozwiązania @>@;
}



@ Urządzenie.

@d nodeTypeRotMod ('I'-'A'+1)
@< Definicje struktur @>=
struct node {
	char type; // typ i obrót, np C4: |'C'-'A'+(4-1)*nodeTypeRotMod|
};



@* Operacje I/O.

@< Zmienne globalne i deklaracje funkcji @>=
int X, Y;
node **board;



@ Wczytywanie planszy.

Tak naprawdę nieistotne są początkowe obroty elementów. Jedyne co nas interesuje to typ (patrz: |enum blockType|). Redukuje to ilość typów komórek do 9 ('A' - 'I').

Wczytujemy całymi wierszami -- dość szybkie, a kosztuje nas to $1500 = 1.46$ KiB. Uwaga -- założyłem, że na początku i na końcu wiersza nie ma białych znaków.

@d INPUTLEN 1500
@< Wczytanie planszy @>=
char input[INPUTLEN];

scanf(" %d %d ", &X, &Y); // Rozmiar planszy: |X| kolumn, |Y| wierszy
printf("%d %d\n", X, Y); // od razu wypisujemy -- why not?
board = new node*[Y];
(*board) = new node[X*Y];
for(int i=1; i<Y; ++i)
	board[i] = *board+i*X;

int boardPos = 0; // pozycja w tablicy |board[0]|. Magicznie wypełniamy wszystkie wiersze i kolumny :P
int inpPos = 0; // pozycja w |input|
while( fgets(input, INPUTLEN, stdin) != NULL)
	for(inpPos = 0; input[inpPos] != 0; inpPos += 3)
		board[0][boardPos++].type = input[inpPos]-'A';



@ @< Wyświetlenie rozwiązania @>=
int xx, yy;
char tmpPrint;
for(yy = 0; yy<Y; ++yy) {
	tmpPrint = board[yy][0].type;
	printf("%c%d", tmpPrint % nodeTypeRotMod + 'A',
					tmpPrint / nodeTypeRotMod + 1);
	for(xx = 1; xx<X; ++xx) {
		tmpPrint = board[yy][xx].type;
		printf(" %c%d", tmpPrint % nodeTypeRotMod + 'A',
						tmpPrint / nodeTypeRotMod + 1);
	}
	printf("\n");
}



@* Szukanie rozwiązania.

@< Zmienne globalne i... @>=
void backtrack(int);



@ Backtracking.

@< Definicje funkcji@>=
void backtrack(int d) {
    /*dbg*/printf("backtrack(%d)\n", d);
}

