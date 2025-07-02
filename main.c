#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_ORD 50
#define MAX_MENU 100

struct Item {
    char name[50];
    float price;
};

struct Order {
    struct Item item;
    int quantity;
    float amount;
};

struct Customer {
    char name[50];
    char type[20];
    struct Order orders[MAX_ORD];
    int orderCount;
    int tableNo;
};

void initializeMenuFile();
void displayMenu();
void takeOrder(struct Customer *customer);
void billing(struct Customer *customer);
float calculateDiscount(float total, char type[], int month);
void generateDailySalesReport();
void addNewRecord();
void updateRecord();
void displayUpdatedRecord();

int main() {
    initializeMenuFile();

    char repeat;
    struct Customer customer;
    int opt;

    printf("\n\t\tJolly - Jip Restaurant\n");
    printf("\t\t Financial Info System\n");

    do {
        printf("\n ========== MAIN MENU ============\n");
        printf(" BILLING SUB MODULE:\n");
        printf("\t[1] Ordering and Receipt\n");
        printf("\t[2] Generate Daily Total Sales\n");

        printf("\n FILE MAINTENANCE:\n");
        printf("\t[3] Add New Record\n");
        printf("\t[4] Update Record\n");
        printf("\t[5] Display Updated Record\n");
        printf("\t[6] Exit\n");

        printf("\nChoose Transaction: ");
        scanf("%d", &opt);
        getchar();

        switch (opt) {
            case 1:
                do {
                    customer.orderCount = 0;
                    printf("\n ========== ORDERING ==========\n");
                    printf("Welcome!!\n");

                    // Table Number Validation
                    char input[20];
                    int valid = 0;
                    while (!valid) {
                        printf("Table No.: ");
                        fgets(input, sizeof(input), stdin);
                        if (sscanf(input, "%d", &customer.tableNo) == 1 && customer.tableNo > 0) {
                            valid = 1;
                        } else {
                            printf("Invalid input. Please enter a numeric table number greater than 0.\n");
                        }
                    }

                    // Customer Name
                    printf("Enter customer name: ");
                    fgets(customer.name, sizeof(customer.name), stdin);
                    customer.name[strcspn(customer.name, "\n")] = '\0';

                    // Customer Type Validation
                    while (1) {
                        printf("Enter customer type (Regular/Senior/PWD): ");
                        fgets(customer.type, sizeof(customer.type), stdin);
                        customer.type[strcspn(customer.type, "\n")] = '\0';

                        if (strcasecmp(customer.type, "Regular") == 0 ||
                            strcasecmp(customer.type, "Senior") == 0 ||
                            strcasecmp(customer.type, "PWD") == 0) {
                            break;
                        } else {
                            printf("Invalid type. Please enter only 'Regular', 'Senior', or 'PWD'.\n");
                        }
                    }

                    takeOrder(&customer);
                    billing(&customer);

                    printf("\nWould you like to process another customer? (Y/N): ");
                    scanf(" %c", &repeat);
                    getchar();
                } while (repeat == 'Y' || repeat == 'y');
                break;
            case 2:
                generateDailySalesReport();
                break;
            case 3:
                addNewRecord();
                break;
            case 4:
                updateRecord();
                break;
            case 5:
                displayUpdatedRecord();
                break;
            case 6:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid input. Try again.\n");
        }
    } while (opt != 6);

    return 0;
}

void initializeMenuFile() {
    FILE *fp = fopen("menu.txt", "r");
    if (fp != NULL) {
        fclose(fp);
        return;
    }

    fp = fopen("menu.txt", "w");
    if (fp == NULL) {
        printf("Failed to create menu.txt\n");
        return;
    }

    fprintf(fp,
        "#Solo Packages\n"
        "Solo Pack #1|150.00\n"
        "Solo Pack #2|170.00\n\n"
        "#Barkada Packages\n"
        "Barkada Pack #1|350.00\n"
        "Barkada Pack #2|500.00\n\n"
        "#Family Packages\n"
        "Family Pack #1|1200.00\n"
        "Family Pack #2|1350.00\n\n"
        "#Individual Items\n"
        "X-tra Rice|20.00\n"
        "Soda 1.5L|70.00\n"
        "Iced Tea 1L|60.00\n"
        "Condiments|20.00\n"
    );
    fclose(fp);
}

void displayMenu() {
    FILE *fp = fopen("menu.txt", "r");
    if (fp == NULL) {
        printf("Error: menu.txt not found.\n");
        return;
    }

    char line[100];
    int itemNum = 1;

    printf("\n======== MENU =========\n");

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '#') {
            printf("\n\t%s\n", line + 1);
        } else {
            char name[50];
            float price;
            if (sscanf(line, " %[^|]|%f", name, &price) == 2) {
                printf("\t%d. %s - %.2f\n", itemNum++, name, price);
            }
        }
    }

    fclose(fp);
}

void takeOrder(struct Customer *customer) {
    struct Item menu[MAX_MENU];
    int menuSize = 0;

    FILE *fp = fopen("menu.txt", "r");
    if (fp == NULL) {
        printf("Error loading menu file.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#') continue;
        char name[50];
        float price;
        if (sscanf(line, " %[^|]|%f", name, &price) == 2) {
            strcpy(menu[menuSize].name, name);
            menu[menuSize].price = price;
            menuSize++;
        }
    }
    fclose(fp);

    int choice, quantity;
    char another;
    char choiceInput[10];

    do {
        displayMenu();
        int validChoice = 0;
        while (!validChoice) {
            printf("\nEnter your choice (1-%d): ", menuSize);
            fgets(choiceInput, sizeof(choiceInput), stdin);
            if (sscanf(choiceInput, "%d", &choice) == 1 && choice >= 1 && choice <= menuSize) {
                validChoice = 1;
            } else {
                printf("Invalid menu choice. Please enter a number from 1 to %d.\n", menuSize);
            }
        }

        printf("Enter quantity: ");
        while (scanf("%d", &quantity) != 1 || quantity <= 0) {
            while (getchar() != '\n');
            printf("Invalid quantity. Enter a positive number: ");
        }
        getchar();

        struct Order o;
        o.item = menu[choice - 1];
        o.quantity = quantity;
        o.amount = o.item.price * quantity;
        customer->orders[customer->orderCount++] = o;

        printf("Do you want to order more? (Y/N): ");
        scanf(" %c", &another);
        getchar();
    } while (another == 'Y' || another == 'y');
}

float calculateDiscount(float total, char type[], int month) {
    float discount = 0;

    if ((month == 5 || month == 6) && total >= 500 && total <= 2000) {
        discount += total * 0.02;
    }

    if (strcasecmp(type, "Senior") == 0) {
        discount += total * 0.20;
    } else if (strcasecmp(type, "PWD") == 0) {
        discount += total * 0.05;
    }

    return discount;
}

void billing(struct Customer *customer) {
    float total = 0, discount, netBill, amountTendered, change;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("\n ======== BILLING STATEMENT ========\n");
    printf("Date: %02d/%02d/%04d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Table No.: %d\n", customer->tableNo);
    printf("Customer Name: %s\n", customer->name);
    printf("Customer Type: %s\n", customer->type);
    printf("----------------------------------------\n");
    printf("Item\t\tQty\tPrice\tAmount\n");

    for (int i = 0; i < customer->orderCount; i++) {
        struct Order o = customer->orders[i];
        printf("%-15s %d\t%.2f\t%.2f\n", o.item.name, o.quantity, o.item.price, o.amount);
        total += o.amount;
    }

    printf("----------------------------------------\n");
    printf("TOTAL BILL:\t\t\t%.2f\n", total);

    discount = calculateDiscount(total, customer->type, tm.tm_mon + 1);
    netBill = total - discount;
    printf("DISCOUNT:\t\t\t%.2f\n", discount);
    printf("NET BILL:\t\t\t%.2f\n", netBill);

    // FIXED SECTION: safer amountTendered input
    char amtInput[50];
    int validAmount = 0;

    do {
        printf("Enter amount tendered: ");
        fgets(amtInput, sizeof(amtInput), stdin);
        amtInput[strcspn(amtInput, "\n")] = '\0'; // Remove newline

        if (sscanf(amtInput, "%f", &amountTendered) != 1) {
            printf("Invalid input. Please enter a valid decimal number.\n");
            continue;
        }

        if (amountTendered < netBill) {
            printf("Insufficient Fund. Please enter again.\n");
        } else {
            validAmount = 1;
        }
    } while (!validAmount);

    change = amountTendered - netBill;
    printf("CHANGE:\t\t\t\t%.2f\n", change);
    printf("******* THANK YOU! *********\n");

    FILE *fp = fopen("sales.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%.2f %.2f %.2f\n", total, discount, netBill);
        fclose(fp);
    } else {
        printf("Error writing to sales file.\n");
    }
}


void generateDailySalesReport() {
    time_t tme = time(NULL);
    struct tm tm = *localtime(&tme);

    FILE *fp = fopen("sales.txt", "r");
    float total = 0, totalDiscount = 0, totalNet = 0, t, d, n;

    if (fp == NULL) {
        printf("No sales data found.\n");
        return;
    }

    while (fscanf(fp, "%f %f %f", &t, &d, &n) == 3) {
        total += t;
        totalDiscount += d;
        totalNet += n;
    }

    fclose(fp);
    printf("\n ======= DAILY TOTAL SALES REPORT =======\n");
    printf("\t as of  %02d/%02d/%04d\n\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Total Billing: $%.2f\n", total);
    printf("Total Discounts: $%.2f\n", totalDiscount);
    printf("Total Net: $%.2f\n", totalNet);
}

void addNewRecord() {
    FILE *fp = fopen("menu.txt", "a");
    if (fp == NULL) {
        printf("Error opening menu file.\n");
        return;
    }

    struct Item newItem;

    getchar();
    printf("Enter item name: ");
    fgets(newItem.name, sizeof(newItem.name), stdin);
    newItem.name[strcspn(newItem.name, "\n")] = '\0';

    printf("Enter item price: ");
    scanf("%f", &newItem.price);

    fprintf(fp, "%s|%.2f\n", newItem.name, newItem.price);
    fclose(fp);

    printf("New item added successfully.\n");
}

void updateRecord() {
    FILE *fp = fopen("menu.txt", "r");
    if (fp == NULL) {
        printf("Menu file not found.\n");
        return;
    }

    struct Item items[100];
    char headers[100][100];
    char rawLines[200][100];
    int count = 0;
    int rawCount = 0;


    while (fgets(rawLines[rawCount], sizeof(rawLines[rawCount]), fp)) {

        if (strlen(rawLines[rawCount]) < 3 || strchr(rawLines[rawCount], '|') == NULL) {
            rawCount++;
            continue;
        }

        if (rawLines[rawCount][0] == '#') {
            strcpy(headers[rawCount], rawLines[rawCount]);
            rawCount++;
            continue;
        }


        sscanf(rawLines[rawCount], " %[^|]|%f", items[count].name, &items[count].price);
        count++;
        rawCount++;
    }
    fclose(fp);

    printf("\nCurrent Menu:\n");
    for (int i = 0; i < count; i++) {
        printf("[%d] %s - %.2f\n", i + 1, items[i].name, items[i].price);
    }

    int choice;
    printf("Enter item number to update or delete: ");
    scanf("%d", &choice);
    choice--;

    if (choice < 0 || choice >= count) {
        printf("Invalid choice.\n");
        return;
    }

    getchar();
    printf("Enter new name (or leave blank to keep current): ");
    char tempName[50];
    fgets(tempName, sizeof(tempName), stdin);
    tempName[strcspn(tempName, "\n")] = '\0';

    if (strlen(tempName) > 0) {
        strcpy(items[choice].name, tempName);
    }

    printf("Enter new price (0 to delete this item): ");
    float newPrice;
    scanf("%f", &newPrice);

    if (newPrice == 0) {

        fp = fopen("menu.txt", "w");
        int itemIndex = 0;
        for (int i = 0; i < rawCount; i++) {
            if (rawLines[i][0] == '#') {
                fprintf(fp, "%s", rawLines[i]);
            } else if (strlen(rawLines[i]) < 3 || strchr(rawLines[i], '|') == NULL) {
                continue;
            } else {
                if (itemIndex != choice) {
                    fprintf(fp, "%s", rawLines[i]);
                }
                itemIndex++;
            }
        }
        fclose(fp);
        printf("Item deleted successfully.\n");
        return;
    }

    items[choice].price = newPrice;

    fp = fopen("menu.txt", "w");
    int itemIndex = 0;
    for (int i = 0; i < rawCount; i++) {
        if (rawLines[i][0] == '#') {
            fprintf(fp, "%s", rawLines[i]);
        } else if (strlen(rawLines[i]) < 3 || strchr(rawLines[i], '|') == NULL) {
            continue;
        } else {
            fprintf(fp, "%s|%.2f\n", items[itemIndex].name, items[itemIndex].price);
            itemIndex++;
        }
    }
    fclose(fp);
    printf("Item updated successfully.\n");
}


void displayUpdatedRecord() {
    displayMenu();
}
