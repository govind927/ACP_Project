/* Advanced_HMS_fixed.c
   Single-file console Hospital Management System (C)
   Compile: gcc Advanced_HMS_fixed.c -o Advanced_HMS
   Run: ./Advanced_HMS
   Admin password (hardcoded): admin123
   Note: Educational/demo. Not production.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE 1024

const char *PATIENT_FILE = "patients.csv";
const char *DOCTOR_FILE  = "doctors.csv";
const char *APPT_FILE    = "appointments.csv";
const char *PHARM_FILE   = "pharmacy.csv";
const char *BILL_FILE    = "bills.csv";

static void safe_prompt(const char *msg, char *out, size_t outlen) {
    printf("%s", msg);
    if (!fgets(out, (int)outlen, stdin)) {
        /* if EOF or error, clear and set empty */
        clearerr(stdin);
        out[0] = '\0';
        return;
    }
    out[strcspn(out, "\n")] = '\0';
}

static long parse_long(const char *s, long def) {
    char *end;
    errno = 0;
    long v = strtol(s, &end, 10);
    if (errno != 0 || end == s) return def;
    return v;
}

static int ensure_file_exists(const char *fn) {
    FILE *f = fopen(fn, "a");
    if (!f) return 0;
    fclose(f);
    return 1;
}

/* Return next id (1 if file missing or empty) */
static int next_id(const char *filename) {
    ensure_file_exists(filename);
    FILE *fp = fopen(filename, "r");
    if (!fp) return 1;
    char line[MAX_LINE];
    int maxid = 0;
    while (fgets(line, sizeof(line), fp)) {
        int id = 0;
        /* read up to first comma */
        if (sscanf(line, "%d", &id) == 1) {
            if (id > maxid) maxid = id;
        }
    }
    fclose(fp);
    return maxid + 1;
}

static void press_enter(void) {
    printf("\nPress Enter to continue...");
    char tmp[8];
    fgets(tmp, sizeof(tmp), stdin);
}

/* PATIENT MODULE */
static void add_patient(void) {
    char name[200], age_s[32], gender[32], contact[64], address[300], history[512];
    int id = next_id(PATIENT_FILE);

    printf("\n--- Add Patient (ID %d) ---\n", id);
    safe_prompt("Name: ", name, sizeof(name));
    safe_prompt("Age: ", age_s, sizeof(age_s));
    safe_prompt("Gender: ", gender, sizeof(gender));
    safe_prompt("Contact: ", contact, sizeof(contact));
    safe_prompt("Address: ", address, sizeof(address));
    safe_prompt("Medical history: ", history, sizeof(history));

    long age = parse_long(age_s, -1);
    if (age < 0) age = 0;

    FILE *fp = fopen(PATIENT_FILE, "a");
    if (!fp) { printf("Error opening %s\n", PATIENT_FILE); return; }
    fprintf(fp, "%d,%s,%ld,%s,%s,%s,%s\n", id, name, age, gender, contact, address, history);
    fclose(fp);
    printf("Patient added with ID %d\n", id);
    press_enter();
}

static void list_file(const char *filename, const char *header) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    printf("\n--- %s ---\n", header);
    if (!fp) { printf("No records found.\n"); press_enter(); return; }
    while (fgets(line, sizeof(line), fp)) {
        /* print raw CSV line */
        printf("%s", line);
    }
    fclose(fp);
    press_enter();
}

static int find_and_print_in_file(const char *filename, int id) {
    FILE *fp = fopen(filename, "r");
    char line[MAX_LINE];
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        int rid = 0;
        if (sscanf(line, "%d", &rid) == 1 && rid == id) {
            printf("%s\n", line);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

static void search_patient(void) {
    char id_s[64];
    safe_prompt("Enter Patient ID to search: ", id_s, sizeof(id_s));
    int id = (int)parse_long(id_s, -1);
    if (id < 0) { printf("Invalid ID\n"); press_enter(); return; }
    printf("\n--- Search Result ---\n");
    if (!find_and_print_in_file(PATIENT_FILE, id)) printf("Patient with ID %d not found.\n", id);
    press_enter();
}

static void delete_record(const char *filename, int id) {
    ensure_file_exists(filename);
    FILE *fp = fopen(filename, "r");
    FILE *tmp = fopen("tmp.csv", "w");
    if (!fp || !tmp) { if (fp) fclose(fp); if (tmp) fclose(tmp); printf("Error opening files\n"); return; }
    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        int rid = 0;
        if (sscanf(line, "%d", &rid) == 1 && rid == id) { found = 1; continue; }
        fputs(line, tmp);
    }
    fclose(fp); fclose(tmp);
    remove(filename);
    rename("tmp.csv", filename);
    if (found) printf("Record %d deleted.\n", id); else printf("Record %d not found.\n", id);
}

static void delete_patient(void) {
    char id_s[64];
    safe_prompt("Enter Patient ID to delete: ", id_s, sizeof(id_s));
    int id = (int)parse_long(id_s, -1);
    if (id < 0) { printf("Invalid ID\n"); press_enter(); return; }
    delete_record(PATIENT_FILE, id);
    press_enter();
}

/* DOCTOR MODULE */
static void add_doctor(void) {
    char name[200], spec[200], contact[64];
    int id = next_id(DOCTOR_FILE);
    printf("\n--- Add Doctor (ID %d) ---\n", id);
    safe_prompt("Name: ", name, sizeof(name));
    safe_prompt("Specialization: ", spec, sizeof(spec));
    safe_prompt("Contact: ", contact, sizeof(contact));
    FILE *fp = fopen(DOCTOR_FILE, "a");
    if (!fp) { printf("Error opening %s\n", DOCTOR_FILE); return; }
    fprintf(fp, "%d,%s,%s,%s\n", id, name, spec, contact);
    fclose(fp);
    printf("Doctor added with ID %d\n", id);
    press_enter();
}

static void search_doctor(void) {
    char id_s[64];
    safe_prompt("Enter Doctor ID to search: ", id_s, sizeof(id_s));
    int id = (int)parse_long(id_s, -1);
    if (id < 0) { printf("Invalid ID\n"); press_enter(); return; }
    printf("\n--- Search Result ---\n");
    if (!find_and_print_in_file(DOCTOR_FILE, id)) printf("Doctor with ID %d not found.\n", id);
    press_enter();
}

/* APPOINTMENT MODULE */
static void add_appointment(void) {
    char pid_s[64], did_s[64], date[64], time_s[64];
    int id = next_id(APPT_FILE);
    printf("\n--- Add Appointment (ID %d) ---\n", id);
    safe_prompt("Patient ID: ", pid_s, sizeof(pid_s));
    safe_prompt("Doctor ID: ", did_s, sizeof(did_s));
    safe_prompt("Date (YYYY-MM-DD): ", date, sizeof(date));
    safe_prompt("Time (HH:MM): ", time_s, sizeof(time_s));
    int pid = (int)parse_long(pid_s, -1);
    int did = (int)parse_long(did_s, -1);
    if (pid < 0 || did < 0) { printf("Invalid IDs\n"); press_enter(); return; }
    FILE *fp = fopen(APPT_FILE, "a");
    if (!fp) { printf("Error opening %s\n", APPT_FILE); return; }
    fprintf(fp, "%d,%d,%d,%s,%s\n", id, pid, did, date, time_s);
    fclose(fp);
    printf("Appointment added with ID %d\n", id);
    press_enter();
}

/* PHARMACY */
static void add_medicine(void) {
    char name[200], qty_s[64], price_s[64];
    int id = next_id(PHARM_FILE);
    printf("\n--- Add Medicine (ID %d) ---\n", id);
    safe_prompt("Name: ", name, sizeof(name));
    safe_prompt("Quantity: ", qty_s, sizeof(qty_s));
    safe_prompt("Price per unit (e.g. 12.50): ", price_s, sizeof(price_s));
    int qty = (int)parse_long(qty_s, 0);
    double price = strtod(price_s, NULL);
    FILE *fp = fopen(PHARM_FILE, "a");
    if (!fp) { printf("Error opening %s\n", PHARM_FILE); return; }
    fprintf(fp, "%d,%s,%d,%.2f\n", id, name, qty, price);
    fclose(fp);
    printf("Medicine added with ID %d\n", id);
    press_enter();
}

/* Get price in cents to avoid floating issues; also returns available qty */
static int get_medicine_price_and_qty(int mid, int *out_qty, double *out_price) {
    ensure_file_exists(PHARM_FILE);
    FILE *fp = fopen(PHARM_FILE, "r");
    char line[MAX_LINE];
    if (!fp) return -1;
    while (fgets(line, sizeof(line), fp)) {
        int id = 0, qty = 0;
        double price = 0.0;
        char name[256];
        if (sscanf(line, "%d,%255[^,],%d,%lf", &id, name, &qty, &price) == 4) {
            if (id == mid) {
                if (out_qty) *out_qty = qty;
                if (out_price) *out_price = price;
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

static void update_pharmacy_qty(int mid, int sold_qty) {
    ensure_file_exists(PHARM_FILE);
    FILE *fp = fopen(PHARM_FILE, "r");
    FILE *tmp = fopen("tmp.csv", "w");
    if (!fp || !tmp) { if(fp) fclose(fp); if(tmp) fclose(tmp); return; }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        int id = 0, qty = 0;
        double price = 0.0;
        char name[256];
        if (sscanf(line, "%d,%255[^,],%d,%lf", &id, name, &qty, &price) == 4) {
            if (id == mid) qty -= sold_qty;
            if (qty < 0) qty = 0;
            fprintf(tmp, "%d,%s,%d,%.2f\n", id, name, qty, price);
        } else {
            fputs(line, tmp);
        }
    }
    fclose(fp); fclose(tmp);
    remove(PHARM_FILE);
    rename("tmp.csv", PHARM_FILE);
}

/* BILLING */
static void generate_bill(void) {
    char pid_s[64], item[128];
    safe_prompt("Enter Patient ID for billing: ", pid_s, sizeof(pid_s));
    int pid = (int)parse_long(pid_s, -1);
    if (pid < 0) { printf("Invalid patient ID\n"); press_enter(); return; }
    double total = 0.0;
    char items_acc[1024] = "";
    printf("Enter medicines in format medID:qty (one per line). Empty line to finish.\n");
    while (1) {
        safe_prompt("Item: ", item, sizeof(item));
        if (strlen(item) == 0) break;
        int mid = 0, qty = 0;
        if (sscanf(item, "%d:%d", &mid, &qty) != 2) { printf("Invalid format\n"); continue; }
        double price = 0.0; int available = 0;
        if (!get_medicine_price_and_qty(mid, &available, &price)) { printf("Medicine %d not found\n", mid); continue; }
        if (qty > available) { printf("Only %d available, selling %d\n", available, available); qty = available; }
        double line_total = price * qty;
        char tmp[256]; snprintf(tmp, sizeof(tmp), "%d:%d:%.2f;", mid, qty, line_total);
        strncat(items_acc, tmp, sizeof(items_acc) - strlen(items_acc) - 1);
        total += line_total;
        update_pharmacy_qty(mid, qty);
    }
    int bill_id = next_id(BILL_FILE);
    FILE *fp = fopen(BILL_FILE, "a");
    if (fp) { fprintf(fp, "%d,%d,%.2f,%s\n", bill_id, pid, total, items_acc); fclose(fp); }
    printf("Bill ID %d generated for Patient %d. Total = %.2f\n", bill_id, pid, total);
    press_enter();
}

/* Simple auth */
static int authenticate(void) {
    char pass[128];
    safe_prompt("Enter admin password: ", pass, sizeof(pass));
    return strcmp(pass, "govind123") == 0;
}

int main(void) {
    if (!authenticate()) { printf("Authentication failed. Exiting.\n"); return 0; }
    while (1) {
        char choice_s[16];
        printf("\n=== Advanced Hospital Management System (Console) ===\n");
        printf("1. Patient Module\n2. Doctor Module\n3. Appointment Module\n4. Pharmacy Module\n5. Billing\n6. Reports\n7. Exit\n");
        safe_prompt("Choice: ", choice_s, sizeof(choice_s));
        int choice = (int)parse_long(choice_s, -1);
        if (choice == 1) {
            char c_s[16];
            printf("\nPatient Module: 1-Add 2-List 3-Search 4-Delete 5-Back\n");
            safe_prompt("Choice: ", c_s, sizeof(c_s));
            int c = (int)parse_long(c_s, -1);
            if (c == 1) add_patient();
            else if (c == 2) list_file(PATIENT_FILE, "Patients");
            else if (c == 3) search_patient();
            else if (c == 4) delete_patient();
            else continue;
        } else if (choice == 2) {
            char c_s[16];
            printf("\nDoctor Module: 1-Add 2-List 3-Search 4-Delete 5-Back\n");
            safe_prompt("Choice: ", c_s, sizeof(c_s));
            int c = (int)parse_long(c_s, -1);
            if (c == 1) add_doctor();
            else if (c == 2) list_file(DOCTOR_FILE, "Doctors");
            else if (c == 3) search_doctor();
            else if (c == 4) { char id_s[64]; safe_prompt("Doctor ID to delete: ", id_s, sizeof(id_s)); int id=(int)parse_long(id_s,-1); if(id>=0) delete_record(DOCTOR_FILE,id); press_enter(); }
            else continue;
        } else if (choice == 3) {
            char c_s[16];
            printf("\nAppointment Module: 1-Add 2-List 3-Delete 4-Back\n");
            safe_prompt("Choice: ", c_s, sizeof(c_s));
            int c = (int)parse_long(c_s, -1);
            if (c == 1) add_appointment();
            else if (c == 2) list_file(APPT_FILE, "Appointments");
            else if (c == 3) { char id_s[64]; safe_prompt("Appointment ID to delete: ", id_s, sizeof(id_s)); int id=(int)parse_long(id_s,-1); if(id>=0) delete_record(APPT_FILE,id); press_enter(); }
            else continue;
        } else if (choice == 4) {
            char c_s[16];
            printf("\nPharmacy Module: 1-Add Medicine 2-List Medicines 3-Back\n");
            safe_prompt("Choice: ", c_s, sizeof(c_s));
            int c = (int)parse_long(c_s, -1);
            if (c == 1) add_medicine();
            else if (c == 2) list_file(PHARM_FILE, "Pharmacy Items");
            else continue;
        } else if (choice == 5) {
            char c_s[16];
            printf("\nBilling: 1-Generate Bill 2-List Bills 3-Back\n");
            safe_prompt("Choice: ", c_s, sizeof(c_s));
            int c = (int)parse_long(c_s, -1);
            if (c == 1) generate_bill();
            else if (c == 2) list_file(BILL_FILE, "Bills");
            else continue;
        } else if (choice == 6) {
            printf("\nReports: \n1. Patients 2. Doctors 3. Appointments 4. Medicines 5. Bills 6. Back\n");
            char r_s[16];
            safe_prompt("Choice: ", r_s, sizeof(r_s));
            int r = (int)parse_long(r_s, -1);
            if (r == 1) list_file(PATIENT_FILE, "Patients");
            else if (r == 2) list_file(DOCTOR_FILE, "Doctors");
            else if (r == 3) list_file(APPT_FILE, "Appointments");
            else if (r == 4) list_file(PHARM_FILE, "Pharmacy Items");
            else if (r == 5) list_file(BILL_FILE, "Bills");
            else continue;
        } else if (choice == 7) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}
