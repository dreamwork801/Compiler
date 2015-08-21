#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NUMBER_LENGTH 5
#define MAX_ID_LENGTH 11
#define INPUT "input.txt"
#define OUTPUT_LIST "lexemelist.txt"
#define OUTPUT_TAB "lexemetable.txt"
#define OUTPUT_CLEAN "cleaninput.txt"
#define MAX_SYMBOLS 13
#define MAX_RESERVEDWORDS 14

typedef enum {
	nulsym = 1, identsym, numbersym, plussym,
	minussym, multsym, slashsym, oddsym,
	eqlsym, neqsym, lessym, leqsym, gtrsym,
	geqsym, lparentsym, rparentsym, commasym,
	semicolonsym, periodsym, becomessym,
	beginsym, endsym, ifsym, thensym, whilesym,
	dosym, callsym, constsym, varsym, procsym,
	writesym, readsym, elsesym
} token;

char * reservedWords[] = { "const", "var", "procedure", "call",
	"begin", "end", "if", "then", "else",
	"while", "do", "read", "write", "odd"};

// If you add another symbol, update definition MAX_SYMBOLS
char symbols[13] = { '+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':' };

FILE * ifp;
FILE * ofpList;
FILE * ofpTab;
FILE * ofpClean;

int DEBUG = 0;
int bool = 0;

int inLen;
char * input;
int inputCount = 0;

void tokenize(char * token);


void readIn()
{
	ifp = fopen(INPUT, "r");
	ofpClean = fopen(OUTPUT_CLEAN, "w");


	int ignore = 0;
	char currChar, nextChar;
	input = malloc(sizeof(char) * 1000000);

	if(DEBUG)
		printf("Cleaning input.\n");

	while(!feof(ifp))
	{

		fscanf(ifp, "%c", &currChar);

		if(!ignore)
		{
			if(currChar == '/')
			{

				fscanf(ifp, "%c", &nextChar);

				if(nextChar == '*')
				{
					if(DEBUG)
						printf("Found a comment.\n\tignore set to 1.\n");
					ignore = 1;
				}

				else
				{
					input[inputCount++] = currChar;
					input[inputCount++] = nextChar;
				}
			}

			else
			{
				input[inputCount++] = currChar;
			}
		}

		else
		{
			if(currChar == '*')
			{
				//if(DEBUG)
				//	printf("%c", nextChar);
				fscanf(ifp, "%c", &nextChar);

				if(nextChar == '/')
				{
					if(DEBUG)
						printf("Comment ended.\n\tignore set to 0.\n");
					ignore = 0;
				}
				else
				{
					input[inputCount++] = currChar;
					input[inputCount++] = nextChar;
				}
			}
		}
	}
	inputCount--;
	inLen = inputCount;

	if(DEBUG)
		printf("\nClean code:\n");
	int i;
	for(i = 0; i < inputCount; i++)
	{
		if(DEBUG)
			printf("%c", input[i]);
		fprintf(ofpClean, "%c", input[i]);
	}

	fclose(ifp);
	fclose(ofpClean);
}

//Helper function for analyze().
int isSymbol(char character)
{
	if(DEBUG)
		printf("Determining if \'%c\' is a symbol.\n", character);
	int i;
	for (i = 0; i < MAX_SYMBOLS; i++)
		if (character == symbols[i])
			return 1;
	return 0;
}

void analyze()
{
	if(DEBUG)
		printf("Analyze clean input.\n");
    ofpTab = fopen(OUTPUT_TAB, "w");
	ofpList = fopen(OUTPUT_LIST, "w");

	int j, i = 0, temp = 0;
	char currChar;

	fprintf(ofpTab, "lexeme\ttoken type\n");


	for (j = 0; j < inputCount; j++)
	{
		char* currString = malloc(sizeof(char) * MAX_ID_LENGTH+1);
		memset(currString, '\0', MAX_ID_LENGTH+1);
		currChar = input[j];

        // We can skip spaces
		if (isspace(currChar))
			continue;

        else if (isSymbol(currChar))
		{
			if (currChar == ';')
				currString[0] = ';';
			else if (currChar == ',')
				currString[0] = ',';
			else if (currChar == '.')
				currString[0] = '.';
			else
			{
				while (isSymbol(currChar))
				{
					char tempChar = currChar;

					if (((currChar == ';') || (currChar == ',') || (currChar == '.')) && (i > 0))
						break;

					if(DEBUG)
						printf("Adding %c\n%s\n", currChar, currString);
					currString[i] = currChar;

					currChar = input[++j];

					if(tempChar == ':' && currChar != '=')
					{
						printf(" Invalid symbol: \'%c\'", currString[0]);
						exit(EXIT_FAILURE);
					}
					i++;
				}
				currChar = input[--j];
			}
			tokenize(currString);
		}

        else if (isalpha(currChar))
		{
			while (isalpha(currChar) || isdigit(currChar))
			{
				if (i > MAX_ID_LENGTH)
				{
					printf("Identifier is too large.\n");
					exit(-1);
				}

				currString[i] = currChar;
				currChar = input[++j];
				i++;
			}
			currChar = input[j--];

			tokenize(currString);
		}

        else if (isdigit(currChar))
		{
			while (isdigit(currChar) || isalpha(currChar))
			{
				if (isalpha(currChar))
				{
					printf("Identifiers cannot begin with a number.\n");
					exit(-1);
				}
				if (i >= MAX_NUMBER_LENGTH)
				{
					printf("Number is too large.\n");
					exit(-1);
				}
				currString[i] = currChar;
				currChar = input[++j];
				i++;
			}

			currChar = input[--j];
			fprintf(ofpTab, "%s\t\t3\n", currString);
			fprintf(ofpList, "3 %s ", currString);

			if(bool)
			{
				//printf("%s\t\t3\n", currString);
				printf("3 %s ", currString);
			}
		}

		// Error
		else
		{

			printf("Invalid symbol: \'%c\'", currChar);
            exit(EXIT_FAILURE);
		}

		// Reset variables
		i = 0;
		memset(currString, '\0', MAX_ID_LENGTH+1);
		free(currString);
	}

	if (bool)
		printf("\n");

	if(DEBUG)
		printf("Closing output files for list and table.\n");
    fclose(ofpTab);
	fclose(ofpList);
    return;
}


void tokenize(char * token)
{
    if(DEBUG)
        printf("Tokenizing %s", token);
    int tokenType = 2;
    char * tempToken = token;

    //Check against each keyword until finding match.
    if(!strcmp(token, "const"))
    {
        if(DEBUG)
            printf("Found const.\n");
        tokenType = constsym;
    }
    else if(!strcmp(token, "var"))
    {
        if(DEBUG)
            printf("Found var.\n");
        tokenType = varsym;
    }
    else if(!strcmp(token, "procedure"))
    {
        if(DEBUG)
            printf("Found procedure.\n");
        tokenType = procsym;
    }
    else if(!strcmp(token, "call"))
    {
        if(DEBUG)
            printf("Found call.\n");
        tokenType = callsym;
    }
    else if(!strcmp(token, "begin"))
    {
        if(DEBUG)
            printf("Found begin.\n");
        tokenType = beginsym;
    }
    else if(!strcmp(token, "end"))
    {
        if(DEBUG)
            printf("Found end.\n");
        tokenType = endsym;
    }
    else if(!strcmp(token, "if"))
    {
        if(DEBUG)
            printf("Found if.\n");
        tokenType = ifsym;
    }
    else if(!strcmp(token, "then"))
    {
        if(DEBUG)
            printf("Found then.\n");
        tokenType = thensym;
    }
    else if(!strcmp(token, "else"))
    {
        if(DEBUG)
            printf("Found else.\n");
        tokenType = elsesym;
    }
    else if(!strcmp(token, "while"))
    {
        if(DEBUG)
            printf("Found while.\n");
        tokenType = whilesym;
    }
    else if(!strcmp(token, "do"))
    {
        if(DEBUG)
            printf("Found do.\n");
        tokenType = dosym;
    }
    else if(!strcmp(token, "read"))
    {
        if(DEBUG)
            printf("Found read.\n");
        tokenType = readsym;
    }
    else if(!strcmp(token, "write"))
    {
        if(DEBUG)
            printf("Found write.\n");
        tokenType = writesym;
    }
    else if(!strcmp(token, "odd"))
    {
        if(DEBUG)
            printf("Found odd.\n");
        tokenType = oddsym;
    }

    //Check against each symbol until finding a match.
    else if(!strcmp(token, "+"))
    {
        if(DEBUG)
            printf("Found +.\n");
        tokenType = plussym;
    }
    else if(!strcmp(token, "-"))
    {
        if(DEBUG)
            printf("Found -.\n");
        tokenType = minussym;
    }
    else if(!strcmp(token, "*"))
    {
        if(DEBUG)
            printf("Found *.\n");
        tokenType = multsym;
    }
    else if(!strcmp(token, "/"))
    {
        if(DEBUG)
            printf("Found /.\n");
        tokenType = slashsym;
    }
    else if(!strcmp(token, "("))
    {
        if(DEBUG)
            printf("Found (.\n");
        tokenType = lparentsym;
    }
    else if(!strcmp(token, ")"))
    {
        if(DEBUG)
            printf("Found ).\n");
        tokenType = rparentsym;
    }
    else if(!strcmp(token, ">="))
    {
        if(DEBUG)
            printf("Found >=.\n");
        tokenType = geqsym;
    }
    else if(!strcmp(token, "<="))
    {
        if(DEBUG)
            printf("Found <=.\n");
        tokenType = leqsym;
    }
    else if(!strcmp(token, "<>"))
    {
        if(DEBUG)
            printf("Found <>.\n");
        tokenType = neqsym;
    }
    else if(!strcmp(token, "="))
    {
        if(DEBUG)
            printf("Found =.\n");
        tokenType = eqlsym;
    }
    else if(!strcmp(token, ","))
    {
        if(DEBUG)
            printf("Found ,.\n");
        tokenType = commasym;
    }
    else if(!strcmp(token, "."))
    {
        if(DEBUG)
            printf("Found ..\n");
        tokenType = periodsym;
    }
    else if(!strcmp(token, "<"))
    {
        if(DEBUG)
            printf("Found <.\n");
        tokenType = lessym;
    }
    else if(!strcmp(token, ">"))
    {
        if(DEBUG)
            printf("Found >.\n");
        tokenType = gtrsym;
    }
    else if(!strcmp(token, ";"))
    {
        if(DEBUG)
            printf("Found ;.\n");
        tokenType = semicolonsym;
    }
    else if(!strcmp(token, ":="))
    {
        if(DEBUG)
            printf("Found :=.\n");
        tokenType = becomessym;
    }

    //Print to table and list.
    if(tokenType == 2)
    {
        if(DEBUG)
            printf("Found identifier.\n");

        fprintf(ofpList, "%d ", tokenType);
        fprintf(ofpList, "%s ", token);

        fprintf(ofpTab, "%s\t\t%d\n", token, tokenType);

        if(DEBUG)
        {
            printf("%d ", tokenType);
            printf("%s\n", token);
        }
        if(bool)
        {
            printf("%d ", tokenType);
            printf("%s ", token);
        }
    }
    else
    {
        if(DEBUG)
            printf("Found reserved word or symbol.\n");

        fprintf(ofpTab, "%s\t\t%d\n", token, tokenType);
        fprintf(ofpList, "%d ", tokenType);

        if(DEBUG)
        {
            printf("%s\t\t%d\n", token, tokenType);
        }
        if(bool)
        {
            //printf("%s\t\t%d\n", token, tokenType);
            printf("%d ", tokenType);
        }
    }
}

int main(int argc, char * argv[])
{
    if(argc > 1 && strcmp(argv[1], "-l") == 0)
    {
        bool = 1;
        DEBUG = 0;
    }
	readIn();
	analyze();
	return 0;
}
