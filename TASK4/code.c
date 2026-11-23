#include <stdio.h>

struct account {
    int number;
    float balance;
};

/* Buggy pass-by-value version for address proof */
void addInterest_byValue(struct account a, float rate) {
    printf("  [Inside by-value copy]   &a = %p\n", (void *)&a);
    a.balance = a.balance + a.balance * rate / 100.0f;
}

/* Repaired pass-by-pointer version */
void addInterest(struct account *acc, float rate) {
    if (acc == NULL) {
        return;
    }
    printf("  [Inside by-pointer]      acc = %p\n", (void *)acc);
    acc->balance = acc->balance + acc->balance * rate / 100.0f;
}

int main(void) {
    struct account list[3] = {
        {101, 5000.0f},
        {102, 12000.0f},
        {103, 800.0f}
    };
    struct account *p = list;

    printf("=== Evidence of Pass-by-Value Copy ===\n");
    printf("[In main]                &list[0] = %p\n", (void *)&list[0]);
    addInterest_byValue(list[0], 4.0f);
    printf("[After by-value call]    list[0].balance = %.2f (unchanged)\n\n", list[0].balance);

    printf("=== Repaired Execution (Pass-by-Pointer) ===\n");
    for (int i = 0; i < 3; i++) {
        addInterest(&list[i], 4.0f);
    }

    printf("\n=== Final Account Balances ===\n");
    for (int i = 0; i < 3; i++) {
        printf("Account %d: %.2f\n", (p + i)->number, (p + i)->balance);
    }

    return 0;
}
