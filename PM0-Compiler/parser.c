#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOL_TABLE_SIZE 100

// TODO DETERMINE IF THIS IS THE CORRECT VALUE. I JUST MADE IT UP
#define CODE_SIZE 100

#define INPUT "lexemelist.txt"
#define OUTPUT "mcode.txt"

//Constants
#define LIT 1
#define OPR 2
    #define OPR_RET 0
    #define OPR_NEG 1
    #define OPR_ADD 2
    #define OPR_SUB 3
    #define OPR_MUL 4
    #define OPR_DIV 5
    #define OPR_ODD 6
    #define OPR_MOD 7
    #define OPR_EQL 8
    #define OPR_NEQ 9
    #define OPR_LSS 10
    #define OPR_LEQ 11
    #define OPR_GTR 12
    #define OPR_GEQ 13
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8
#define SIO 9

void addToSymbolTable(int kind, char name[], int val);
void readLexemes();
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
void emit(int op, int l, int m);
void writeCode();
void error();

// Token types
typedef enum {
	nulsym = 1, identsym, numbersym, plussym,
	minussym, multsym, slashsym, oddsym,
	eqlsym, neqsym, lessym, leqsym, gtrsym,
	geqsym, lparentsym, rparentsym, commasym,
	semicolonsym, periodsym, becomessym,
	beginsym, endsym, ifsym, thensym, whilesym,
	dosym, callsym, constsym, varsym, procsym,
	writesym, readsym, elsesym
} tokenType;

//Struct for intermediate code generation
typedef struct code {
	int op;
	int l;
	int m;
}op;

// Struct for symbol
typedef struct symbol {
	int kind; //const = 1, var = 2, proc = 3
	char name[12];
	int val;
	int level;
	int addr;
}symbol;

char lexemes[1000][12]; // TODO should this be 100 or bigger?
char token[12]; // global variable for current token
int tokenCount = 0;
int cx = 0; // code index
int level = 0; //level
int localAddress = 0;
const int baseAddress = 4;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; // symbol table to hold symbols
op code[CODE_SIZE]; // This is the intermidiate code generation
int symbolIndex = 0;

int DEBUG = 0;
int bool = 0;

FILE * ifp;
FILE * fpw;

int main(int argc, char * argv[])
{
	if(argc > 1 && strcmp(argv[1], "-a") == 0)
	{
		bool = 1;
		DEBUG = 0;
	}
	if(DEBUG)
		printf("Begin parser.\n");
	readLexemes();
	program();
	writeCode();
	printf("No errors, the program is syntactically correct.\n\n");
	return 0;
}

// Read the lexemes from file and store them in an array
void readLexemes()
{
	if(DEBUG)
		printf("Reading lexemes.\n");
	int i = 0;
	ifp = fopen(INPUT, "r");

	while(fscanf(ifp, "%s", lexemes[i]) != -1)
	{
		if(DEBUG)
			printf("%s\n", lexemes[i]);
		i++;
	}

	fclose(ifp);
}

// Write the intermidiate code to a file
void writeCode()
{
    if(DEBUG)
		printf("Writing Intermediate Code Generation.\n");
	int i = 0;
	fpw = fopen(OUTPUT, "w+");
	if(bool == 1)
	{
		for (i=0; i<cx; i++){
	        printf("%d\t", code[i].op);
	        printf("%d\t", code[i].l);
	        printf("%d\n", code[i].m);
		}
	}
	for (i=0; i<cx; i++){
        fprintf(fpw, "%d\t", code[i].op);
        fprintf(fpw, "%d\t", code[i].l);
        fprintf(fpw, "%d\n", code[i].m);
	}
	if (DEBUG){
        int sx = 0;
        for (sx = 0; sx <= symbolIndex; sx++){
            printf("\n");
            printf("Kind: %d", symbol_table[sx].kind);
            printf(" Name: %s", symbol_table[sx].name);
            printf(" Val: %d", symbol_table[sx].val);
            printf(" Level: %d", symbol_table[sx].level);
            printf(" Addr: %d", symbol_table[sx].addr);
        }
	}

	fclose(ifp);
}

// Get the next token from the lexeme list
void get()
{
	strcpy(token,lexemes[tokenCount]);
	tokenCount++;
	if(DEBUG)
		printf("Looking at token #%d\t%s\n", tokenCount-1, token);
}

// Get the index in the symbol table for a particular symbol
int getSX(int kind)
{
    // There is also a getSX function like this in factor(). FYI
    int templevel = level;
    get();
    int sx = 0;
    while (templevel > -1){
        for (sx = 0; sx <= symbolIndex; sx++){
            if((strcmp(symbol_table[sx].name, token) == 0) && (symbol_table[sx].kind == kind) && (symbol_table[sx].level == templevel)){
                return sx;
            }
        }
        templevel--;
    }
    if(symbolIndex > 1000)
    	error(0); // Not sure if we will ever reach this.
}

// Add a symbole to the symbol table
void addToSymbolTable(int kind, char name[], int val)
{
    symbol_table[symbolIndex].kind = kind;
    strcpy(symbol_table[symbolIndex].name, name);
    symbol_table[symbolIndex].level = level;

    switch (kind)
    {
        case 1:
            symbol_table[symbolIndex].val = val;
            break;
        case 2:
            symbol_table[symbolIndex].addr = localAddress+baseAddress;
            localAddress++;
            break;
        case 3:
            symbol_table[symbolIndex].val = 0;
            symbol_table[symbolIndex].addr = cx;
            break;
        default:
            break;
    }
    symbolIndex++;
}

// Errors
void error(int errNum)
{
	if(errNum == 0)
	{
		printf("PLACEHOLDER ERROR.\n");
		exit(-1);
	}

	switch(errNum)
	{
		case 1:
			printf("Use = instead of :=.\n");
			break;
		case 2:
			printf("= must be followed by an identifier.\n");
			break;
		case 3:
			printf("Identifier must be followed by :=.");
			break;
		case 4:
			printf("const, var, procedure must be followed by identifier.\n");
			break;
		case 5:
			printf("Semicolon or comma missing.\n");
			break;
		case 6:
			printf("Incorrect symbol after procedure declaration.\n");
			break;
		case 7:
			printf("Statement expected.\n");
			break;
		case 8:
			printf("Incorrect symbol after statement part in block.\n");
			break;
		case 9:
			printf("Period expected.\n");
			break;
		case 10:
			printf("Semicolon between statements missing.\n");
			break;
		case 11:
			printf("Undeclared identifier.\n");
			break;
		case 12:
			printf("Assignment to constant or variable is meaningless.\n");
			break;
		case 13:
			printf("Assignment operator expected.\n");
			break;
		case 14:
			printf("call must be followed by an identifier.\n");
			break;
		case 15:
			printf("Call of a constant or variable is meaningless.\n");
			break;
		case 16:
			printf("then expected.\n");
			break;
		case 17:
			printf("Semicolon or } expected.\n");
			break;
		case 18:
			printf("do expected.\n");
			break;
		case 19:
			printf("Incorrect symbol following statement.\n");
			break;
		case 20:
			printf("Relational operator.\n");
			break;
		case 21:
			printf("Expression must not contain a procedure.\n");
			break;
		case 22:
			printf("Right parenthesis missing.\n");
			break;
		case 23:
			printf("The preceding factor cannot begin with this symbol.\n");
			break;
		case 24:
			printf("An expression cannot begin with this symbol.\n");
			break;
		case 25:
			printf("This number is too large.\n");
			break;
		case 26:
			printf("const must be followed by =.\n");
			break;
		case 27:
			printf("end expected.\n");
			break;
	}
	exit(errNum);
}

//Needs more testing but this should work with our lexemes.
int strToNum(char * token)
{
	int i = 0;
	int place = strlen(token);
	int result = 0;
	while(place > 0)
	{
		result += (token[i] - '0') * pow(10, place-1);
		place--;
		i++;
	}
	return result;
}

// Determines which relation OPR to return based on lexeme
int relation(int lexeme)
{
    switch(lexeme)
    {
        case eqlsym:
            return OPR_EQL;
        case neqsym:
            return OPR_NEQ;
        case lessym:
            return OPR_LSS;
        case leqsym:
            return OPR_LEQ;
        case gtrsym:
            return OPR_GTR;
        case geqsym:
            return OPR_GEQ;
        default:
            error(20);
    }
}

void program()
{
	if(DEBUG)
		printf("Starting program.\n");
	get();
	localAddress = 0;
	block();
	if(strToNum(token) != periodsym)
		error(9);
	emit(SIO, 0, 2);

}

void block()
{
	if(DEBUG)
		printf("In block.\n");
	if(strToNum(token) == constsym)
	{
		do
		{
		    char varname[12];
			get();
			if(strToNum(token) != identsym)
				error(4);
            get();
            strcpy(varname, token);
			get();
			if(strToNum(token) != eqlsym)
				error(26);
			get();
			if(strToNum(token) != numbersym)
				error(2);
            get();
            addToSymbolTable(1, varname, strToNum(token));
            get();
		} while(strToNum(token) == commasym);

		if(strToNum(token) != semicolonsym)
			error(17);
		get();
	}

	if(strToNum(token) == varsym)
	{
		do
		{
			get();
			if(strToNum(token) != identsym)
				error(4);
			get();
			addToSymbolTable(2, token, 0); // Not exactly sure if its zero.
			get();

		} while(strToNum(token) == commasym);
		if(strToNum(token) != semicolonsym)
			error(17);
		get();
	}

	int tmpLocal = localAddress;
	int tmpBase = baseAddress;
	emit(JMP, 0, cx+1);
	int levelTemp = cx-1;

	while(strToNum(token) == procsym)
	{
		get();
		if(strToNum(token) != identsym)
			error(4);
		get();
		addToSymbolTable(3, token, 0); // Changed from token to level
		get();
		level++;
		//int tempSymbolIndex = symbolIndex;
		localAddress = 0;
		if(strToNum(token) != semicolonsym)
			error(17);
		get();
		block();
		level--;
		//symbolIndex = tempSymbolIndex;
		code[levelTemp].m = cx;
		if(strToNum(token) != semicolonsym)
			error(17);
		get();
	}
	emit(INC, 0, tmpBase + tmpLocal);
	statement();
	emit(OPR, 0, OPR_RET);
}

void statement()
{
    int sx1 = 0; // temporary symbol table index
    int cx1 = 0; // temporary code index
    int cx2 = 0; // temporary code index

	if(DEBUG)
		printf("In statement.\n");
	if(strToNum(token) == identsym)
	{
        sx1 = getSX(2);
		get();
		if(strToNum(token) != becomessym)
			error(3);
		get();
		expression();
		emit(STO, level-symbol_table[sx1].level, symbol_table[sx1].addr);
	}
	else if(strToNum(token) == callsym)
	{
		get();
		if(strToNum(token) != identsym)
			error(14);
		sx1 = getSX(3);
		get();
		emit(CAL, level - symbol_table[sx1].level, symbol_table[sx1].addr);
	}
	else if(strToNum(token) == beginsym)
	{
		get();
		statement();
		while(strToNum(token) == semicolonsym)
		{
			get();
			statement();
		}
		if(strToNum(token) != endsym){
			error(27);
		}
		get();
	}
	else if(strToNum(token) == ifsym)
	{
		get();
		condition();
		if(strToNum(token) != thensym)
			error(16);
		get();
		cx1 = cx;
		emit(JPC, 0, 0);
		statement();
		// I'm not sure if I 100% like this fix. But it will have to do
		if (strToNum(lexemes[tokenCount]) == elsesym)
            get();
		if (strToNum(token) == elsesym)
		{
		    get();
		    cx2 = cx;
		    emit(JMP, 0, 0);
		    code[cx1].m = cx;
		    statement();
		    code[cx2].m = cx;
		}
		else
            code[cx1].m = cx;
	}
	else if(strToNum(token) == whilesym)
	{
	    cx1 = cx;
		get();
		condition();
		cx2 = cx;
		emit(JPC, 0 ,0);
		if(strToNum(token) != dosym)
			error(18);
        get();
		statement();
		emit(JMP, 0, cx1);
		code[cx2].m = cx;
	}
	else if (strToNum(token) == readsym)
	{
	    get();
	    if (strToNum(token) != identsym)
            error(0); // TODO implement
        int sx = getSX(2);
        get();
        emit(SIO, 0, 1);
        emit(STO, 0, symbol_table[sx].addr);
	}
	else if (strToNum(token) == writesym)
	{
	    get();
	    if (strToNum(token) != identsym)
            error(0); // TODO need to do some error checking. The if statement may not be valid either
        expression();
        emit(SIO, 0, 0);
	}
}

void condition()
{
	if(DEBUG)
		printf("In condition.\n");

	if(strToNum(token) == oddsym)
	{
		get();
		expression();
		emit(OPR, 0, OPR_ODD);
	}
	else
	{
		expression();
		int relOp = relation(strToNum(token));
		get();
		expression();
		emit(OPR, 0, relOp);
	}
}

void expression()
{
    int addop;
	if(DEBUG)
		printf("In expression.\n");

	if(strToNum(token) == plussym || strToNum(token) == minussym){
        addop = strToNum(token);
		get();
        term();
        if (addop == minussym)
            emit(OPR, 0, OPR_NEG); // negation
	}
	else
        term();

	while(strToNum(token) == plussym || strToNum(token) == minussym)
	{
	    addop = strToNum(token);
		get();
		term();
		if (addop == plussym)
            emit(OPR, 0, OPR_ADD); // addition
        else
            emit(OPR, 0, OPR_SUB); // subtraction
	}
}

void term()
{
    int mulop;
    factor();
	if(DEBUG)
		printf("In term.\n");

	while(strToNum(token) == multsym || strToNum(token) == slashsym)
	{
	    mulop = strToNum(token);
		get();
		factor();
		if (mulop == multsym)
            emit(OPR, 0, OPR_MUL); // multiplication
        else
            emit (OPR, 0, OPR_DIV); // division
	}
}

void factor()
{
	if(DEBUG)
		printf("In factor.\n");

	if(strToNum(token) == identsym)
	{
        get();
        int sx = 0;
        int templevel = level;
        while (templevel > -1){
            for (sx = 0; sx <= symbolIndex; sx++){
                if((strcmp(symbol_table[sx].name, token) == 0) && (symbol_table[sx].kind == 1 || symbol_table[sx].kind == 2) && symbol_table[sx].level == templevel){
                    templevel = -2;
                    break;
                }
            }
            templevel--;
        }
        switch (symbol_table[sx].kind){
            case 1: emit(LIT, 0, symbol_table[sx].val);
                break;
            case 2: emit(LOD, level- symbol_table[sx].level, symbol_table[sx].addr);
                break;
            default: error(21);
        }
		get();
	}
	else if(strToNum(token) == numbersym){
		get();
		emit(LIT, 0, strToNum(token));
		get();
	}
	else if(strToNum(token) == lparentsym)
	{
		get();
		expression();
		if(strToNum(token) != rparentsym)
			error(22);
		get();
	}
	else{
		error(11);
	}
}

void emit(int op, int l, int m)
{
    if (cx > CODE_SIZE)
        error(25);
    else
    {
        code[cx].op = op;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
}
