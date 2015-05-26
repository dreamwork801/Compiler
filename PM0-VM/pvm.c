//Zach Colby

#include <stdio.h>
#define n 17

struct Instruction {
    int op; //opcode
    int l; //L
    int m; //M
    };

struct Instruction code[n];

int main()
{
    int i = 0;
    //File reading from
    FILE *fpr;
    fpr = fopen("mcode.txt", "r");
    //File writing to
    FILE *fpw;
    fpw = fopen("stacktrace.txt", "w+");

    for (i = 0; i<n; i++){
        fscanf(fpr, "%d", &code[i].op);
        fscanf(fpr, "%d", &code[i].l);
        fscanf(fpr, "%d", &code[i].m);
    }

    fprintf(fpw, "Line\tOP\tL\tM\n");

    for (i=0; i<n; i++){
        fprintf(fpw, "%d\t", i);
        fprintf(fpw, "%d\t", code[i].op);
        fprintf(fpw, "%d\t", code[i].l);
        fprintf(fpw, "%d\n", code[i].m);
    }

    fclose(fpr);
    fclose(fpw);
    return 0;
}
