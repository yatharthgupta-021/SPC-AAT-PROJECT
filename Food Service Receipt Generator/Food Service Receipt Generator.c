#include <stdio.h>
#include <time.h>
#include <string.h>

#define GST_RATE 0.05
#define MAX_ITEMS 20

/* ---------- FUNCTION DECLARATIONS ---------- */
void getDateTime(char dateStr[], char timeStr[]);
int getBillNumber();
void printMenu();
void takeOrder(char itemName[][30], int itemQty[], float itemPrice[],
               float itemTotal[], int *itemCount, float *subtotal);
void getCustomerDetails(char customerName[], char orderTypeStr[]);
int getPaymentChoice();
void printReceipt(int billNo, char dateStr[], char timeStr[],
                  char customerName[], char orderTypeStr[],
                  char itemName[][30], int itemQty[],
                  char paymentTypeStr[],
                  float itemPrice[], float itemTotal[],
                  int itemCount, float subtotal, float gst, float total,
                  FILE *receiptFile);
void handlePayment(int paymentChoice, float total, FILE *receiptFile);
void printStoreAddress(FILE *receiptFile);

/* ---------- MAIN ---------- */
int main() {
    char dateStr[20], timeStr[20];
    char customerName[50], orderTypeStr[20];
    char itemName[MAX_ITEMS][30];
    int itemQty[MAX_ITEMS], itemCount = 0;
    float itemPrice[MAX_ITEMS], itemTotal[MAX_ITEMS];
    float subtotal = 0.0, gst, total;

    getDateTime(dateStr, timeStr);
    int billNo = getBillNumber();

    char fileName[30];
    sprintf(fileName, "bill_%d.txt", billNo);
    FILE *receiptFile = fopen(fileName, "w");

    printMenu();
    takeOrder(itemName, itemQty, itemPrice, itemTotal, &itemCount, &subtotal);
    getCustomerDetails(customerName, orderTypeStr);

    gst = subtotal * GST_RATE;
    total = subtotal + gst;

    int paymentChoice = getPaymentChoice();

    char paymentTypeStr[20];

    if (paymentChoice == 1)
        strcpy(paymentTypeStr, "Cash");
    else if (paymentChoice == 2)
        strcpy(paymentTypeStr, "Card");
    else
        strcpy(paymentTypeStr, "UPI");

    printReceipt(billNo, dateStr, timeStr, customerName, orderTypeStr,
             itemName, itemQty,
             paymentTypeStr, itemPrice, itemTotal,
             itemCount, subtotal, gst, total, receiptFile);


    handlePayment(paymentChoice, total, receiptFile);
    printStoreAddress(receiptFile);

    fclose(receiptFile);
    printf("\nReceipt saved as %s\n", fileName);

    return 0;
}

/* ---------- FUNCTION DEFINITIONS ---------- */

void getDateTime(char dateStr[], char timeStr[]) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(dateStr, 20, "%d-%m-%Y", tm_info);
    strftime(timeStr, 20, "%I:%M %p", tm_info);
}

int getBillNumber() {
    FILE *file = fopen("bill_count.txt", "r");
    int billNo = 1;

    if (file != NULL) {
        fscanf(file, "%d", &billNo);
        billNo++;
        fclose(file);
    }

    file = fopen("bill_count.txt", "w");
    fprintf(file, "%d", billNo);
    fclose(file);

    return billNo;
}

void printMenu() {
    printf("=================================\n");
    printf("        WELCOME TO FOOD HUB      \n");
    printf("=================================\n");
    printf("\n1. Burger        - Rs 120\n");
    printf("2. Pizza         - Rs 250\n");
    printf("3. Sandwich      - Rs 100\n");
    printf("4. French Fries  - Rs 80\n");
    printf("5. Soft Drink    - Rs 50\n");
    printf("---------------------------------\n");
}

void takeOrder(char itemName[][30], int itemQty[], float itemPrice[],
               float itemTotal[], int *itemCount, float *subtotal) {

    int choice, quantity;
    float price;
    int found, i;

    while (1) {

        /* Validate item choice */
        while (1) {
            printf("Enter item number (1 - 5) (0 if done): ");

            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n');   // clear invalid input
                printf("\nInvalid input! Please enter a number between 0 and 5.\n\n");
                continue;
            }

            if (choice < 0 || choice > 5) {
                printf("\nInvalid choice! Please enter a number between 0 and 5.\n\n");
                continue;
            }

            break;  // valid input
        }

        if (choice == 0) {
            break;   // finish ordering
        }

        /* Validate quantity */
        while (1) {
            printf("Enter quantity: ");

            if (scanf("%d", &quantity) != 1) {
                while (getchar() != '\n');
                printf("\nInvalid input! Please enter a valid quantity.\n\n");
                continue;
            }

            if (quantity <= 0) {
                printf("\nQuantity must be greater than 0.\n\n");
                continue;
            }

            break;  // valid quantity
        }

        /* Set item price and name */
        switch (choice) {
            case 1: price = 120; strcpy(itemName[*itemCount], "Burger"); break;
            case 2: price = 250; strcpy(itemName[*itemCount], "Pizza"); break;
            case 3: price = 100; strcpy(itemName[*itemCount], "Sandwich"); break;
            case 4: price = 80;  strcpy(itemName[*itemCount], "French Fries"); break;
            case 5: price = 50;  strcpy(itemName[*itemCount], "Soft Drink"); break;
        }

        /* Check if item already exists */
        found = 0;
        for (i = 0; i < *itemCount; i++) {
            if (strcmp(itemName[i], itemName[*itemCount]) == 0) {
                itemQty[i] += quantity;
                itemTotal[i] += price * quantity;
                *subtotal += price * quantity;
                found = 1;
                break;
            }
        }

        /* If new item, add it */
        if (!found) {
            itemQty[*itemCount]   = quantity;
            itemPrice[*itemCount] = price;
            itemTotal[*itemCount] = price * quantity;
            *subtotal += itemTotal[*itemCount];
            (*itemCount)++;
        }

        printf("Item added successfully!\n\n");
    }
}

void getCustomerDetails(char customerName[], char orderTypeStr[]) {
    int orderType;
    char orderChar;

    printf("\nEnter Customer Name: ");
    scanf(" %[^\n]", customerName);

    printf("\nOrder Type:\n");
    printf("1. Dine In\n");
    printf("2. Online\n");

    do {
        printf("Choose order type (1/2): ");
        scanf(" %c", &orderChar);

        if (orderChar != '1' && orderChar != '2') {
            printf("\nInvalid choice! Please enter 1 or 2.\n\n");
        }

    } while (orderChar != '1' && orderChar != '2');

    /* Convert char to int */
    orderType = orderChar - '0';
    if (orderType == 1)
        strcpy(orderTypeStr, "Dine In");
    else
        strcpy(orderTypeStr, "Home Delivery");
}

int getPaymentChoice() {
    char choice;

    printf("\nPayment Options:\n");
    printf("1. Cash\n");
    printf("2. Card\n");
    printf("3. UPI\n");

    do {
        printf("Choose payment method (1/2/3): ");
        scanf(" %c", &choice);

        if (choice != '1' && choice != '2' && choice != '3') {
            printf("\nInvalid choice! Please enter 1, 2, or 3.\n\n");
        }

    } while (choice != '1' && choice != '2' && choice != '3');

    return choice - '0';   // converts '1' → 1, '2' → 2, '3' → 3
}

void printReceipt(int billNo, char dateStr[], char timeStr[],
                  char customerName[], char orderTypeStr[],
                  char itemName[][30], int itemQty[],
                  char paymentTypeStr[],
                  float itemPrice[], float itemTotal[],
                  int itemCount, float subtotal, float gst, float total,
                  FILE *receiptFile) {

    printf("\n========================================\n");
    printf("             FOOD HUB RECEIPT            \n");
    printf("              Bill No : %d               \n", billNo);
    printf("========================================\n");
    printf("Date : %s   Time : %s\n", dateStr, timeStr);
    printf("Customer   : %s\nOrder Type : %s\n", customerName, orderTypeStr);
    printf("Payment    : %s\n", paymentTypeStr);
    fprintf(receiptFile, "Payment    : %s\n", paymentTypeStr);
    printf("----------------------------------------\n");

    //for saving
    fprintf(receiptFile, "========================================\n");
    fprintf(receiptFile, "             FOOD HUB RECEIPT            \n");
    fprintf(receiptFile, "              Bill No : %d               \n", billNo);
    fprintf(receiptFile, "========================================\n");
    fprintf(receiptFile, "Date : %s   Time : %s\n", dateStr, timeStr);
    fprintf(receiptFile, "Customer   : %s\nOrder Type : %s\n", customerName, orderTypeStr);
    fprintf(receiptFile, "----------------------------------------\n");


    printf("%-15s %-6s %-10s %-10s\n", "Item", "Qty", "Price", "Total");
    fprintf(receiptFile, "%-15s %-6s %-10s %-10s\n", "Item", "Qty", "Price", "Total");

    for (int i = 0; i < itemCount; i++){
        printf("%-15s %-6d Rs %-7.2f Rs %-7.2f\n",
               itemName[i], itemQty[i], itemPrice[i], itemTotal[i]);
        fprintf(receiptFile, "%-15s %-6d Rs %-7.2f Rs %-7.2f\n",
               itemName[i], itemQty[i], itemPrice[i], itemTotal[i]);
    }

    printf("----------------------------------------\n");
    printf("Subtotal :                       Rs %.2f\nGST (5%%) :                       Rs %.2f\nTotal    :                       Rs %.2f\n",
           subtotal, gst, total);
    fprintf(receiptFile, "----------------------------------------\n");
    fprintf(receiptFile, "Subtotal :                       Rs %.2f\n", subtotal);
    fprintf(receiptFile, "GST (5%%) :                       Rs %.2f\n", gst);
    fprintf(receiptFile, "Total    :                       Rs %.2f\n", total);
}

void handlePayment(int paymentChoice, float total, FILE *receiptFile) {
    float cashPaid, balance;

    if (paymentChoice == 1) {
        
        float totalPaid = 0.0;

        while (totalPaid < total) {
            printf("Enter Cash Amount: Rs ");
            scanf("%f", &cashPaid);

            if (cashPaid <= 0) {
                printf("Invalid amount! Please enter a valid amount.\n");
                continue;
            }

            totalPaid += cashPaid;

            if (totalPaid < total) {
                printf("Insufficient amount!\n");
                printf("Remaining amount to be paid : Rs %.2f\n\n", total - totalPaid);
            }
        }

        balance = totalPaid - total;
        printf("\nPayment Successful\n");
        printf("Balance Returned: Rs %.2f\n", balance);

        fprintf(receiptFile, "Payment Method: Cash\n");
        fprintf(receiptFile, "Total Paid: Rs %.2f\n", totalPaid);
        fprintf(receiptFile, "Balance Returned: Rs %.2f\n", balance);
    }
    else {
        printf("\nPayment Successful\n");
        fprintf(receiptFile, "Payment Successful via %s\n", 
                paymentChoice == 2 ? "Card" : "UPI");
    }

}

void printStoreAddress(FILE *receiptFile) {
    printf("\n========================================\n");
    printf("               STORE ADDRESS             \n");
    printf("                FOOD HUB                \n");
    printf("             123, MG Road               \n");
    printf("           Bengaluru - 560001            \n");
    printf("            Karnataka, India             \n");
    printf("========================================\n");

    fprintf(receiptFile, "\nSTORE ADDRESS\nFOOD HUB\n123, MG Road\nBengaluru - 560001\nKarnataka, India\n");
}