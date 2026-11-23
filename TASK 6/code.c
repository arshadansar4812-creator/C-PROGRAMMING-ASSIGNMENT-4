#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 20
#define MAX_COMBO_ITEMS 3

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int hour;
    int minute;
} Time;

typedef enum {
    ITEM_PACKAGED,
    ITEM_COOKED,
    ITEM_COMBO
} ItemKind;

typedef struct {
    int id;
    char name[32];
    float unit_price;
    int stock;
    int units_sold;
    float revenue;
    ItemKind kind;
    union {
        Date expiry_date;                 /* ITEM_PACKAGED */
        Time prep_time;                   /* ITEM_COOKED */
        int component_ids[MAX_COMBO_ITEMS]; /* ITEM_COMBO (0 if unused) */
    } spec;
} CanteenItem;

/* Converts dd/mm/yyyy to approximate day index for simple comparison */
static long to_day_number(Date d) {
    return (long)d.year * 365L + (long)d.month * 30L + (long)d.day;
}

static int find_item_index(const CanteenItem inv[], int n, int id) {
    for (int i = 0; i < n; i++) {
        if (inv[i].id == id) {
            return i;
        }
    }
    return -1;
}

int record_sale(CanteenItem inv[], int n, int item_id, int qty) {
    if (qty <= 0) {
        printf("[SALE REJECTED] Invalid quantity %d for item ID %d.\n", qty, item_id);
        return 0;
    }
    int idx = find_item_index(inv, n, item_id);
    if (idx < 0) {
        printf("[SALE REJECTED] Item ID %d not found.\n", item_id);
        return 0;
    }
    CanteenItem *item = &inv[idx];

    if (item->kind == ITEM_COMBO) {
        /* Verify stock for all component items first */
        for (int c = 0; c < MAX_COMBO_ITEMS; c++) {
            int cid = item->spec.component_ids[c];
            if (cid != 0) {
                int cidx = find_item_index(inv, n, cid);
                if (cidx < 0 || inv[cidx].stock < qty) {
                    printf("[SALE REJECTED] Insufficient stock for combo '%s' (qty %d).\n",
                           item->name, qty);
                    return 0;
                }
            }
        }
        for (int c = 0; c < MAX_COMBO_ITEMS; c++) {
            int cid = item->spec.component_ids[c];
            if (cid != 0) {
                int cidx = find_item_index(inv, n, cid);
                inv[cidx].stock -= qty;
            }
        }
    } else {
        if (item->stock < qty) {
            printf("[SALE REJECTED] '%s' has only %d in stock (requested %d).\n",
                   item->name, item->stock, qty);
            return 0;
        }
        item->stock -= qty;
    }

    item->units_sold += qty;
    item->revenue += item->unit_price * (float)qty;
    printf("[SALE OK] %d x '%s' -> Revenue +%.2f (Remaining stock: %d)\n",
           qty, item->name, item->unit_price * (float)qty, item->stock);
    return 1;
}

void warn_expiring_soon(const CanteenItem inv[], int n, Date today) {
    long today_num = to_day_number(today);
    printf("\n--- Expiry Warnings (Within 2 Days of %02d/%02d/%04d) ---\n",
           today.day, today.month, today.year);
    int warned = 0;
    for (int i = 0; i < n; i++) {
        if (inv[i].kind == ITEM_PACKAGED) {
            long exp_num = to_day_number(inv[i].spec.expiry_date);
            long diff = exp_num - today_num;
            if (diff <= 2) {
                printf("WARNING: '%s' (ID %d) expires on %02d/%02d/%04d (Stock: %d)\n",
                       inv[i].name, inv[i].id,
                       inv[i].spec.expiry_date.day,
                       inv[i].spec.expiry_date.month,
                       inv[i].spec.expiry_date.year,
                       inv[i].stock);
                warned = 1;
            }
        }
    }
    if (!warned) {
        printf("No packaged items expiring within 2 days.\n");
    }
}

void print_summary_by_revenue(CanteenItem inv[], int n) {
    CanteenItem sorted[MAX_ITEMS];
    for (int i = 0; i < n; i++) {
        sorted[i] = inv[i];
    }

    /* Descending selection sort by revenue */
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            if (sorted[j].revenue > sorted[best].revenue) {
                best = j;
            }
        }
        if (best != i) {
            CanteenItem tmp = sorted[i];
            sorted[i] = sorted[best];
            sorted[best] = tmp;
        }
    }

    float total_rev = 0.0f;
    printf("\n--- Day's Bill Summary (Sorted by Revenue) ---\n");
    for (int i = 0; i < n; i++) {
        printf("%d. ID %-3d | %-18s | Sold: %2d | Stock Left: %2d | Revenue: Rs. %.2f\n",
               i + 1, sorted[i].id, sorted[i].name,
               sorted[i].units_sold, sorted[i].stock, sorted[i].revenue);
        total_rev += sorted[i].revenue;
    }
    printf("Total Canteen Revenue: Rs. %.2f\n", total_rev);
}

int main(void) {
    Date today = {20, 11, 2026};
    CanteenItem menu[4] = {
        {1, "Veg Puff",       20.0f, 15, 0, 0.0f, ITEM_PACKAGED, {.expiry_date = {21, 11, 2026}}},
        {2, "Masala Dosa",    50.0f, 10, 0, 0.0f, ITEM_COOKED,   {.prep_time = {8, 30}}},
        {3, "Cold Coffee",    40.0f,  4, 0, 0.0f, ITEM_PACKAGED, {.expiry_date = {15, 12, 2026}}},
        {4, "Morning Combo",  60.0f,  0, 0, 0.0f, ITEM_COMBO,    {.component_ids = {1, 2, 0}}}
    };

    /* 1. Normal sales */
    record_sale(menu, 4, 2, 4);  /* 4 Masala Dosas */
    record_sale(menu, 4, 3, 2);  /* 2 Cold Coffees */

    /* 2. Sale exceeding stock */
    record_sale(menu, 4, 3, 5);  /* Only 2 Cold Coffees left */

    /* 3. Expiring item check */
    warn_expiring_soon(menu, 4, today);

    /* 4. Sorted revenue summary */
    print_summary_by_revenue(menu, 4);

    return 0;
}
