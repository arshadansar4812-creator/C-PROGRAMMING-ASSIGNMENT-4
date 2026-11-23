#include <stdio.h>
#include <string.h>

#define MAX_PATIENTS 100

typedef struct {
    int systolic;
    int diastolic;
} BloodPressure;

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char name[40];
    int age;
    char phone[16];       /* e.g., "+919876543210" or 10 digits + '\0' */
    BloodPressure bp;
    Date visit_date;
} Patient;

typedef struct {
    Patient records[MAX_PATIENTS];
    int count;
} PatientRegister;

int add_patient(PatientRegister *reg, const Patient *p) {
    if (reg == NULL || p == NULL) {
        return -1;
    }
    if (reg->count >= MAX_PATIENTS) {
        printf("Warning: Register full (%d max). Cannot add %s.\n", MAX_PATIENTS, p->name);
        return 0;
    }
    reg->records[reg->count] = *p;
    reg->count++;
    return 1;
}

void sort_by_age(PatientRegister *reg) {
    if (reg == NULL || reg->count < 2) {
        return;
    }
    for (int i = 0; i < reg->count - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < reg->count; j++) {
            if (reg->records[j].age < reg->records[min_idx].age) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Patient temp = reg->records[i];
            reg->records[i] = reg->records[min_idx];
            reg->records[min_idx] = temp;
        }
    }
}

void print_register(const PatientRegister *reg) {
    printf("Total patients: %d (sizeof(Patient) = %zu bytes)\n", reg->count, sizeof(Patient));
    for (int i = 0; i < reg->count; i++) {
        const Patient *p = &reg->records[i];
        printf("%d. %-16s | Age: %2d | Phone: %-12s | BP: %3d/%2d | Date: %02d/%02d/%04d\n",
               i + 1, p->name, p->age, p->phone,
               p->bp.systolic, p->bp.diastolic,
               p->visit_date.day, p->visit_date.month, p->visit_date.year);
    }
}

int main(void) {
    PatientRegister phc = { .count = 0 };

    Patient p1 = {"Anitha Raj", 46, "9840112233", {130, 85}, {12, 10, 2026}};
    Patient p2 = {"Karthik S", 29, "9840223344", {118, 76}, {12, 10, 2026}};
    Patient p3 = {"Meenakshi V", 62, "9840334455", {145, 92}, {13, 10, 2026}};

    add_patient(&phc, &p1);
    add_patient(&phc, &p2);
    add_patient(&phc, &p3);

    printf("--- Before Sorting by Age ---\n");
    print_register(&phc);

    sort_by_age(&phc);

    printf("\n--- After Sorting by Age (Ascending) ---\n");
    print_register(&phc);

    return 0;
}
