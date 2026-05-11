#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 101

struct Costumer {
    char name[100];
    double payment;
    double realPayment;
};

struct Operation {
    char owner[100];
    double amount;
    char exclude[MAX][100];
    int excludeCount;
};

bool contains(char array[][100], int size, char* search) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i], search) == 0) {
            return true;
        }
    }
    return false;
}

void applyOperation(struct Costumer* costumer, struct Operation operation, int costumerSize) {
    for (int i = 0; i < costumerSize; i++) {
        if (strcmp(costumer[i].name, operation.owner) == 0) {
            costumer[i].payment += operation.amount;
        }
    }

    double share = operation.amount / (costumerSize - operation.excludeCount);

    for (int i = 0; i < costumerSize; i++) {
        if (!contains(operation.exclude, operation.excludeCount, costumer[i].name)) {
            costumer[i].realPayment += share;
        }
    }
}

int main() {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("output.txt", "w");

    int costumersSize;
    fscanf(in, "%d", &costumersSize);

    struct Costumer costumersArr[MAX];

    for (int i = 0; i < costumersSize; i++) {
        fscanf(in, "%s", costumersArr[i].name);
        costumersArr[i].payment = 0;
        costumersArr[i].realPayment = 0;
    }

    char line[8000];
    fgets(line, sizeof(line), in);

    while (fgets(line, sizeof(line), in)) {
        if (strlen(line) <= 1) {
            continue;
        }

        struct Operation op;
        op.excludeCount = 0;

        char *word = strtok(line, " \n");
        if (!word) {
            continue;
        }
        strcpy(op.owner, word);

        word = strtok(NULL, " \n");
        if (!word) {
            continue;
        }
        op.amount = atof(word);

        char *rest = strtok(NULL, "\n");

        if (rest) {
            char *slashPos = strchr(rest, '/');

            if (slashPos) {
                slashPos++;

                char *name = strtok(slashPos, ", \n");
                while (name) {
                    strcpy(op.exclude[op.excludeCount++], name);
                    name = strtok(NULL, ", \n");
                }
            }
        }

        applyOperation(costumersArr, op, costumersSize);
    }

    for (int i = 0; i < costumersSize; i++) {
        fprintf(out, "%s %.1f %.1f\n",
                costumersArr[i].name,
                costumersArr[i].payment,
                costumersArr[i].realPayment);
    }

    double balance[MAX];
    for (int i = 0; i < costumersSize; i++) {
        balance[i] = costumersArr[i].payment - costumersArr[i].realPayment;
    }

    for (int i = 0; i < costumersSize; i++) {
        if (balance[i] < 0) {
            double debt = -balance[i];

            for (int j = 0; j < costumersSize; j++) {
                if (balance[j] > 0) {
                    double pay = debt < balance[j] ? debt : balance[j];

                    if (pay > 0) {
                        fprintf(out, "%s %.1f %s\n",
                                costumersArr[i].name,
                                pay,
                                costumersArr[j].name);

                        debt -= pay;
                        balance[j] -= pay;
                    }

                    if (debt <= 0) {
                        break;
                    }
                }
            }
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}