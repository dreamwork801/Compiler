//Zach Colby

#include <stdio.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 17 //CHANGE THIS LATER BACK TO 500!!!!!!!!!!!!!!!
#define MAX_LEXI_LEVELS 3

struct Instruction {
    int op; // opcode
    int l; // L
    int m; // M
    };

struct Instruction code[MAX_CODE_LENGTH];
struct Instruction ir;

int main()
{
    // index, stack pointer, base pointer, pc
    int i,sp,bp,pc,numInstructions = 0;

    ir.op = 0;
    ir.l = 0;
    ir.m = 0;

    // Initialize stack
    int stack[MAX_STACK_HEIGHT];
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
        fscanf(fpr, "%d", &code[i].op);
        fscanf(fpr, "%d", &code[i].l);
        fscanf(fpr, "%d", &code[i].m);
    }
    //This the number of instructions given in this file NEEDS FIXING!!!!!!!!!!!!!!!!!!!
    numInstructions = i;

    fprintf(fpw, "Line\tOP\tL\tM\n");

    // Print out the input
    for (i=0; i<numInstructions; i++){
        fprintf(fpw, "%d\t", i);

        char* optext = "ERR";
        switch (code[i].op) {
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
        fprintf(fpw, "%s\t", optext);
        fprintf(fpw, "%d\t", code[i].l);
        fprintf(fpw, "%d\n", code[i].m);
    }

    //Fetch
    pc = 0;
    ir.l = code[pc].l;
    ir.op = code[pc].op;
    ir.m = code[pc].m;
    printf("%d,%d,%d", ir.op,ir.l,ir.m);



    fclose(fpr);
    fclose(fpw);
    return 0;
}

