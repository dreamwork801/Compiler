/*
	Michael Wahlberg
	vm.c V1

	Added:
	-	Wrote functionality.
	-	Added flag to output to console. Avoiding doing it later.

	Testing:
	-	Given Test Case: [X]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define INPUT "mcode.txt"
#define OUTPUT "stacktrace.txt"

void fetch();
void execute();
int base(int l, int base);
void initStack();
void initOps();
void oprExec();
void printStack();

typedef struct {
	int op; //Op Code
	int l; //Lexicographical level
	int m; //
} opc;

int sp = 0, bp = 1, pc = 0;
int halt = 0;
int opCount = 0;
int DEBUG = 0;
int bool = 0;
int stack[MAX_STACK_HEIGHT];
int arFlag[MAX_STACK_HEIGHT];
int recursiveLevel = 0;
int currentProcedureStack[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cpsIndex = 0; //CurrentProcedureStack Index

FILE * ifp;
FILE * ofp;

//Op code stuff.
opc ops[MAX_CODE_LENGTH];
opc ir;

char * OP[] = {
	"JUNK",
	"lit",
	"opr",
	"lod",
	"sto",
	"cal",
	"inc",
	"jmp",
	"jpc",
	"sio",
	"sio",
	"sio"
};

int main(int argc, char * argv[])
{
	ofp = fopen(OUTPUT, "w");

	if(argc > 1)
	{
		DEBUG = 0;
		bool = 1;
	}

	if(DEBUG)
		printf("Michael Wahlberg\n\tvm.c\nDEBUG initiated.\n\n");

	initStack();
	initOps();

	if(DEBUG || bool)
		printf("\t\t\t\tpc\tbp\tsp\tstack\nInitial Values\t\t\t0\t1\t0\n");
	fprintf(ofp, "\t\t\t\t\tpc\tbp\tsp\tstack\nInitial Values\t\t0\t1\t0\n");

	while(sp != -1 && !halt)
	{
		fetch();
		if(DEBUG || bool)
			printf("%d\t", pc-1);
		fprintf(ofp, "%d\t", pc-1);
		execute();

		//break;
	}

	fclose(ofp);

	return 0;
}

void fetch()
{
	ir = ops[pc];
	pc++;
}

void execute()
{
	switch(ir.op)
	{
		case 1: //LIT
			sp++;
			stack[sp] = ir.m;
			break;
		case 2: //OPR
			oprExec();
			break;
		case 3: //LOD
			sp++;
			if (ir.l == 0)
                stack[sp] = stack[base(ir.l, bp) + ir.m];
            else
                stack[sp] = stack[base(ir.l + recursiveLevel, bp) + ir.m];
			break;
		case 4: //STO
            if (ir.l == 0)
                stack[base(ir.l, bp) + ir.m] = stack[sp];
            else
                stack[base(ir.l + recursiveLevel, bp) + ir.m] = stack[sp];
			sp--;
			break;
		case 5: //CAL
			stack[sp + 1] = 0;
			stack[sp + 2] = base(ir.l, bp);
			stack[sp + 3] = bp;
			stack[sp + 4] = pc;
			arFlag[sp] = 1;
			bp = sp+1;
			pc = ir.m;
			// If we are calling the procedure we are already in, its recursive and we need up the recursive level varaible
			if (currentProcedureStack[cpsIndex] == ir.m)
                recursiveLevel++;
            cpsIndex++;
			currentProcedureStack[cpsIndex] = ir.m;
			break;
		case 6: //INC
			sp += ir.m;
			break;
		case 7: //JMP
			pc = ir.m;
			break;
		case 8: //JPC
			if(stack[sp] == 0)
				pc = ir.m;
			sp--;
			break;
		case 9: //SIO
			if(ir.m == 0)
			{
				printf("%d\n", stack[sp]);
				sp--;
			}
			else if(ir.m == 1)
			{
				sp++;
				scanf("%d", &stack[sp]);
			}
			else if(ir.m == 2)
			{
				printStack();
				exit(0);
			}
			else
				if(DEBUG)
					printf("Incorrect SIO m-code at: %d\n", pc);
			break;
		default:
			if(DEBUG)
				printf("Incorrect Op code at: %d.\n", pc-1);
			break;
	}
	printStack();
}

void oprExec()
{
	switch(ir.m)
	{
		case 0:
            if (bp - 1 == 0) //Sometimes a not needed OPR 0 0 is added, so lets ignore it if this is the case
                break;
			sp = bp - 1;
			pc = stack[sp + 4];
			bp = stack[sp + 3];
			arFlag[sp] = 0;
			if (currentProcedureStack[cpsIndex] == currentProcedureStack[cpsIndex - 1]) // If we are getting out of recursion, lower the recursive level
                recursiveLevel--;
            cpsIndex--;
			break;
		case 1:
			stack[sp] = -1 * stack[sp];
			break;
		case 2:
			sp--;
			stack[sp] += stack[sp+1];
			break;
		case 3:
			sp--;
			stack[sp] -= stack[sp+1];
			break;
		case 4:
			sp--;
			stack[sp] *= stack[sp+1];
			break;
		case 5:
			sp--;
			stack[sp] /= stack[sp+1];
			break;
		case 6:
			stack[sp] = stack[sp]%2;
			break;
		case 7:
			sp--;
			stack[sp] = stack[sp] % stack[sp+1];
			break;
		case 8:
			sp--;
			stack[sp] = stack[sp] == stack[sp+1];
			break;
		case 9:
			sp--;
			stack[sp] = stack[sp] != stack[sp+1];
			break;
		case 10:
			sp--;
			stack[sp] = stack[sp] < stack[sp+1];
			break;
		case 11:
			sp--;
			stack[sp] = stack[sp] <= stack[sp+1];
			break;
		case 12:
			sp--;
			stack[sp] = stack[sp] > stack[sp+1];
			break;
		case 13:
			sp--;
			stack[sp] = stack[sp] >= stack[sp+1];
			break;
		default:
			if(DEBUG)
				printf("Incorrect m-code on OPR at: %d.\n", pc);
			break;
	}
}

void initStack()
{
	if(DEBUG)
		printf("Initiating the stack.\n");
	int i;
	for(i = 0; i < MAX_STACK_HEIGHT; i++)
		stack[i] = 0;
}

void initOps()
{
	if(DEBUG)
		printf("Initiating ops.\n");
	if(DEBUG || bool)
		printf("Line\tOP\tL\tM\n");
	fprintf(ofp, "Line\tOP\tL\tM\n");
	ifp = fopen(INPUT, "r");
	int i = 0;

	while(fscanf(ifp, "%d %d %d", &ops[opCount].op, &ops[opCount].l, &ops[opCount].m) != -1)
	{
		if(DEBUG || bool)
			printf("%d\t%s\t%d\t%d\n", opCount, OP[ops[opCount].op], ops[opCount].l, ops[opCount].m);
		fprintf(ofp, "%d\t%s\t%d\t%d\n", opCount, OP[ops[opCount].op], ops[opCount].l, ops[opCount].m);
		opCount++;
	}

	fclose(ifp);
}

void printStack()
{
	int i;
	if(DEBUG || bool){
	    if (ir.op == 3 || ir.op == 4)
            printf("%s\t%d\t%d\t\t%d\t%d\t%d\t", OP[ir.op], ir.l + recursiveLevel, ir.m, pc, bp, sp);
        else
            printf("%s\t%d\t%d\t\t%d\t%d\t%d\t", OP[ir.op], ir.l, ir.m, pc, bp, sp);
	}
    if (ir.op == 3 || ir.op == 4)
        fprintf(ofp, "%s\t%d\t%d\t\t%d\t%d\t%d\t", OP[ir.op], ir.l + recursiveLevel, ir.m, pc, bp, sp);
	else
        fprintf(ofp, "%s\t%d\t%d\t\t%d\t%d\t%d\t", OP[ir.op], ir.l, ir.m, pc, bp, sp);
	for(i = 1; i <= sp; i++)
	{
		if(DEBUG || bool)
			printf("%d ", stack[i]);
		fprintf(ofp, "%d ", stack[i]);
		if(arFlag[i] == 1 && i < sp)
		{
			if(DEBUG || bool)
				printf("| ");
			fprintf(ofp, "| ");

		}
	}
	if(DEBUG || bool)
		printf("\n");
	fprintf(ofp, "\n");
}

/**********************************************/
/* Find base L levels down */
/* */
/**********************************************/
int base(int l, int base) // l stand for L in the instruction format
{
	 int b1; //find base L levels down
	 b1 = base;
	 while (l > 0)
	 {
		 b1 = stack[b1 + 2];
		 l--;
	 }
	 return b1;
}
