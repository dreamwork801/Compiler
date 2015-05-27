//Zach Colby

#include <stdio.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

struct Instruction {
    int op; // opcode
    int l; // L
    int m; // M
    };

struct Instruction code[MAX_CODE_LENGTH];
struct Instruction ir;
int stack[MAX_STACK_HEIGHT];
int sp,bp,pc = 0;

//Gets the optext for the given opcode
char* getOpText(int opcode){
    char *optext;
    switch (opcode) {
        case 1:
            optext = "lit";
            break;
        case 2:
            optext = "opr";
            break;
        case 3:
            optext = "lod";
            break;
        case 4:
            optext = "sto";
            break;
        case 5:
            optext = "cal";
            break;
        case 6:
            optext = "inc";
            break;
        case 7:
            optext = "jmp";
            break;
        case 8:
            optext = "jpc";
            break;
        case 9:
            optext = "sio";
            break;
        default:
            optext = "ERR";
            break;
    }
    return optext;
}

int main()
{
    // index, stack pointer, base pointer, pc
    int i,numInstructions,halt = 0;

    ir.op = 0;
    ir.l = 0;
    ir.m = 0;

    // Initialize stack
    for (i=0; i<MAX_STACK_HEIGHT; i++){
        stack[i]=0;
    }

    // File reading from
    FILE *fpr;
    fpr = fopen("mcode.txt", "r");
    // File writing to
    FILE *fpw;
    fpw = fopen("stacktrace.txt", "w+");

    //Read from files
    for (i = 0; i<MAX_CODE_LENGTH; i++){
        if (fscanf(fpr, "%d", &code[i].op) == EOF){
            break;
        }
        fscanf(fpr, "%d", &code[i].l);
        fscanf(fpr, "%d", &code[i].m);
    }
    numInstructions = i;

    fprintf(fpw, "Line\tOP\tL\tM\n");

    // Write the input to file
    for (i=0; i<numInstructions; i++){
        fprintf(fpw, "%d\t", i);
        fprintf(fpw, "%s\t", getOpText(code[i].op));
        fprintf(fpw, "%d\t", code[i].l);
        fprintf(fpw, "%d\n", code[i].m);
    }

    //Fetch & execution
    pc = 0;
    sp = 0;
    bp = 1;
    halt = 0;

    fprintf(fpw, "\n\n\t\t\t\tpc\tbp\tsp\tstack\n");
    fprintf(fpw, "Initial values\t\t\t%d\t%d\t%d\n",pc,bp,sp);

    while (halt == 0){
        ir.l = code[pc].l;
        ir.op = code[pc].op;
        ir.m = code[pc].m;
        fprintf(fpw, "%d\t", pc);
        pc++;

        halt = execute(halt);

        fprintf(fpw, "%s\t%d\t%d\t%d\t%d\t%d\t", getOpText(ir.op), ir.l, ir.m, pc, bp, sp);
        for (i=1; i<=sp; i++){
            fprintf(fpw, "%d ", stack[i]);
            if (i%7 == 0 && sp>7) //TAKE A SECOND LOOK AT THIS!!!!!!!!!!!!!!!!!!!!!!!!!!
                fprintf(fpw, "| ");
        }
        fprintf(fpw, "\n");
    } // end loop

    fclose(fpr);
    fclose(fpw);
    return 0;
}

int base(int level, int b){
    while(level>0){
        b = stack[b + 2];
        level--;
    }
    return b;
}

int execute(int halt){
    switch (ir.op) {
        case 1: //LIT
            sp = sp+1;
            stack[sp] = ir.m;
            break;
        case 2: //OPR
            switch (ir.m){
                case 0: //RET
                    sp = bp - 1;
                    pc = stack[sp + 4];
                    bp = stack[sp + 3];
                    break;
                case 1: //NEG
                    stack[sp] = -stack[sp];
                    break;
                case 2: //ADD
                    sp = sp - 1;
                    stack[sp] = stack[sp] + stack[sp + 1];
                    break;
                case 3: //SUB
                    sp = sp - 1;
                    stack[sp] = stack[sp] - stack[sp + 1];
                    break;
                case 4: //MUL
                    sp = sp - 1;
                    stack[sp] = stack[sp] * stack[sp + 1];
                    break;
                case 5: //DIV
                    sp = sp - 1;
                    stack[sp] = stack[sp] / stack[sp + 1];
                    break;
                case 6: //ODD
                    stack[sp] = stack[sp] % 2;
                    break;
                case 7: //MOD
                    sp = sp - 1;
                    stack[sp] = stack[sp] % stack[sp + 1];
                    break;
                case 8: //EQL
                    sp = sp - 1;
                    stack[sp] = (stack[sp] == stack[sp + 1]);
                    break;
                case 9: //NEQ
                    sp = sp - 1;
                    stack[sp] = (stack[sp] != stack[sp + 1]);
                    break;
                case 10: //LSS
                    sp = sp - 1;
                    stack[sp] = (stack[sp] < stack[sp + 1]);
                    break;
                case 11: //LEQ
                    sp = sp - 1;
                    stack[sp] = (stack[sp] <= stack[sp + 1]);
                    break;
                case 12: //GTR
                    sp = sp - 1;
                    stack[sp] = (stack[sp] > stack[sp + 1]);
                    break;
                case 13: //GEQ
                    sp = sp - 1;
                    stack[sp] = (stack[sp] >= stack[sp + 1]);
                    break;
            }
            break;
        case 3: //LOD
            sp = sp + 1;
            stack[sp] = stack[base(ir.l, bp) + ir.m];
            break;
        case 4: //STO
            stack[base(ir.l, bp) + ir.m] = stack[sp];
            sp = sp - 1;
            break;
        case 5: //CAL
            stack[sp + 1] = 0;              //return value (FV)
            stack[sp + 2] = base(ir.l, bp);    //static link (SL)
            stack[sp + 3] = bp;             //dynamic link {DL)
            stack[sp + 4] = pc;             //return address (RA)
            bp = sp + 1;
            pc = ir.m;
            break;
        case 6: //INC
            sp = sp + ir.m;
            break;
        case 7: //JMP
            pc = ir.m;
            break;
        case 8: //JPC
            if (stack [sp] == 0){
                pc = ir.m;
            }
            sp = sp - 1;
            break;
        case 9: // SIO
            switch (ir.m){
                case 0:
                    printf("%d", stack [sp]);
                    sp = sp - 1;
                    break;
                case 1:
                    sp = sp + 1;
                    scanf ("%d", &stack[sp]);
                    break;
                case 2:
                    halt = 1;
                    break;
            } //end SIO switch
            break;
    } // end ir.op switch
    return halt;
}
