#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int power( int base, int exponent)

{   
    if (exponent == 0) return 1; 
    int temp = power(base, exponent/2);
    if (exponent % 2 == 0)
        return temp * temp;
    else
        return (base * temp * temp);
}
const char* __asan_default_options() { return "detect_leaks=0"; }

struct directive {
    char gate[17];
    int n;
    int s;
    int *inputs;
    int *outputs;
    int *selectors;
};

int incrInputs(int *arr, int icount) {
    int i;
    for (i = icount + 1; i >= 2; i--) {
        arr[i] = !arr[i];
        if (arr[i] == 1) {
            return 1;
        }
    }
    return 0;
}

void NOT(int *values, int iindex, int oindex) {
    values[oindex] = !values[iindex];
}

void AND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] && values[iindex2];
}

void NOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] || values[iindex2]);
}

void XOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] ^ values[iindex2];
}

void PASS(int *values, int iindex, int oindex) {
    values[oindex] = values[iindex];
}

void OR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] || values[iindex2];
}

void NAND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] && values[iindex2]);
}



void DECODER(int *values, int n, int *iindex, int *oindex) {
    int s = 0;
    int i;
    for (i = 0; i < power(2, n); i++) {
        values[oindex[i]] = 0;
    }
    for (i = 0; i < n; i++) {
        s += values[iindex[i]] * power(2, n - i - 1);
    }
    values[oindex[s]] = 1;
}

void MUX(int *values, int n, int *iindex, int *sindex, int oindex) {
    int s = 0;
    int i;
    for (i = 0; i < n; i++) {
        s += values[sindex[i]] * power(2, n - i - 1);
    }
    values[oindex] = values[iindex[s]];
}

void printNames(int size, char **arr) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%s\n", arr[i]);
    }
}

int indexOf(int size, char **arr, char *var) {
    int i;
    for (i = 0; i < size; i++) {
        if (strcmp(arr[i], var) == 0) {
            return i;
        }
    }
    return -1;
}

void resetValues(int size, int *arr) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = 0;
    }
    arr[1] = 1;
}

int main(int argc, char** argv) {
    if (argc - 1 != 1) {
        printf("Invalid number of arguments\n");
        return 0;
    }

    
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Invalid input\n");
        return 0;
    }

    
    int scount = 0;
    struct directive* steps = NULL;
    int size = 2;
    int icount = 0;
    int ocount = 0;
    int tcount = 0;
    char dir[17];
    char **names;
    int *values = malloc(size * sizeof(int));

    
    fscanf(file, " %s", dir);
    fscanf(file, "%d", &icount);
   
    int sum = 0;

    size += icount;
   
    names = malloc(size * sizeof(char *));
    names[0] = malloc(2 * sizeof(char));
    names[0] = "0\0";
    names[1] = malloc(2 * sizeof(char));
    names[1] = "1\0";
   
   
   
    sum+=2;  
   
    int i;
   
    sum += icount; 
   
   
    for (i = 0; i < icount; i++) {
 
        names[i + 2] = malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", names[i + 2]);
    }

    
    fscanf(file, " %s", dir);
    fscanf(file, "%d", &ocount);
    size += ocount;
   
    
    sum += size;
    names = realloc(names, size * sizeof(char *));
   
    sum+=ocount; 
   
    for (i = 0; i < ocount; i++) {
        names[i + icount + 2] = malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", names[i + icount + 2]);
    }

    
   
   
    int zt = 0;
    while (!feof(file)) {
        int numInputs = 2, numOutputs = 1;
        struct directive step;
        int sc = fscanf(file, " %s", dir);
        if (sc != 1) {
            break;
        }
        scount++;
        step.n = 0;
        step.s = 0;
        strcpy(step.gate, dir);

        if (strcmp(dir, "NOT") == 0) {
            numInputs = 1;
        }
        if (strcmp(dir, "PASS") == 0) {
            numInputs = 1;
        }
        if (strcmp(dir, "DECODER") == 0) {
            fscanf(file, "%d", &numInputs);
            step.n = numInputs;
            numOutputs = power(2, numInputs);
        }
        if (strcmp(dir, "MULTIPLEXER") == 0) {
            fscanf(file, "%d", &numInputs);
            step.s = numInputs;
            numInputs = power(2, numInputs);
        }

        step.inputs = malloc(numInputs * sizeof(int));
        step.outputs = malloc(numOutputs * sizeof(int));
        step.selectors = malloc(step.s * sizeof(int));

        char v[17];
        for (i = 0; i < numInputs; i++) {
            fscanf(file, "%*[: ]%16s", v);
            step.inputs[i] = indexOf(size, names, v);
        }

        for (i = 0; i < step.s; i++) {
            fscanf(file, "%*[: ]%16s", v);
            step.selectors[i] = indexOf(size, names, v);
        }
 
        for (i = 0; i < numOutputs; i++) {
            fscanf(file, "%*[: ]%16s", v);
            int idx = indexOf(size, names, v);
            if (idx == -1) {
                size++;
                tcount++;
                zt+=size;
                names = realloc(names, size * sizeof(char *));
                names[size - 1] = malloc(17 * sizeof(char));
                strcpy(names[size - 1], v);
                step.outputs[i] = size - 1;
            }
            else {
                step.outputs[i] = idx;
            }
        }
       

        if (!steps) {
            steps = malloc(sizeof(struct directive));
        } else {
            steps = realloc(steps, scount * sizeof(struct directive));
        }
        steps[scount - 1] = step;
    }
   

   
    free(values);
    values = malloc(size * sizeof(int));
   
    resetValues(size, values);

    while(1 < 2) {

        for (i = 0; i < icount; i++) {
            printf("%d ", values[i + 2]);
        }
        printf("|");


        for (i = 0; i < scount; i++) {
            struct directive step = steps[i];
            if (strcmp(step.gate, "NOT") == 0) {
                NOT(values, step.inputs[0], step.outputs[0]);
            }
            if (strcmp(step.gate, "AND") == 0) {
                AND(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "OR") == 0) {
                OR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "NAND") == 0) {
                NAND(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "NOR") == 0) {
                NOR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "XOR") == 0) {
                XOR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "PASS") == 0) {
                PASS(values, step.inputs[0], step.outputs[0]);
            }
            if (strcmp(step.gate, "DECODER") == 0) {
                DECODER(values, step.n, step.inputs, step.outputs);
            }
            if (strcmp(step.gate, "MULTIPLEXER") == 0) {
                MUX(values, step.s, step.inputs, step.selectors, step.outputs[0]);
            }
        }

        
        for (i = 0; i < ocount; i++) {
            printf(" %d", values[icount + i + 2]);
        }
        printf("\n");

        if (!incrInputs(values, icount)) {
            break;
        }
    }
    
   
    exit(0);
   
}

