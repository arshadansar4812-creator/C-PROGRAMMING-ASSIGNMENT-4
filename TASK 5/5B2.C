#include <stdio.h>

extern int g_token;
extern int g_veg_token;
extern int g_nonveg_token;

/* (a) Automatic local */
int token_auto(void) {
    int t = 1000;
    return ++t;
}

/* (b) Static local (single counter) */
int token_static(void) {
    static int t = 1000;
    return ++t;
}

/* (b-mod) Static local for two independent counters: 0 = veg, 1 = non-veg */
int token_static_split(int is_nonveg) {
    static int veg = 1000;
    static int nonveg = 1000;
    return is_nonveg ? ++nonveg : ++veg;
}

/* (c) Global shared via extern */
int token_extern(void) {
    return ++g_token;
}

int main(void) {
    printf("(a) auto  : %d %d %d\n", token_auto(), token_auto(), token_auto());
    printf("(b) static: %d %d %d\n", token_static(), token_static(), token_static());
    printf("(c) extern: %d %d %d\n", token_extern(), token_extern(), token_extern());

    /* (d) register loop variable in printing loop */
    printf("(d) register loop tokens: ");
    for (register int i = 0; i < 3; i++) {
        printf("%d ", 1001 + i);
    }
    printf("\n");

    /* Task 5.2: Veg vs Non-Veg independent counters */
    printf("Split static -> Veg: %d %d | Non-Veg: %d %d\n",
           token_static_split(0), token_static_split(0),
           token_static_split(1), token_static_split(1));

    printf("Split extern -> Veg: %d %d | Non-Veg: %d %d\n",
           ++g_veg_token, ++g_veg_token,
           ++g_nonveg_token, ++g_nonveg_token);

    return 0;
}
