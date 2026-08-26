// Banking Management System
// Author: Skim
// Description: A comprehensive banking system with account management, transactions, and audit logging

// Standard library includes for I/O, string manipulation, memory allocation, time functions, and character handling
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Platform-specific directory creation
#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#endif

// Account structure - stores all essential account information
typedef struct {
    int accountNumber;
    char accountName[50];
    char pin[5];
    float balance;
    int status;
    char accountType[10];
    char idNumber[20];
} Account;

// Function prototypes
void clear_input_buffer(); // NEW: Safe buffer clearing
void displayAccount(Account *acc);
int saveAccount(Account *acc);
Account *getAccount(int num);
void welcome();
void showSession();
void logTransaction(char *action);
void mainMenu();
void createAccount();
void deleteAccount();
void deposit();
void withdraw();
void remittance();
void initDatabase();
int listAllAccountsAndSelect(int *selectedAccountNum);

// Safely clear the input buffer to prevent infinite loops on EOF
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Entry point: bootstrap storage, show intro, and start interactive menu
int main() {
    initDatabase();
    welcome();
    showSession();
    mainMenu();
    return 0;
}

// Ensures the backing directory/index file exist before any operations run
void initDatabase() {
#ifdef _WIN32
    mkdir("database");
#else
    mkdir("database", 0700);
#endif
    FILE *fp = fopen("database/index.txt", "a");
    if (fp != NULL) {
        fclose(fp);
    }
}

void welcome() {
    printf("\n");
    printf("██████   █████  ███    ██ ██   ██ ██ ███    ██  ██████      ███████ ██    ██ ███████ ████████ ███████ ███    ███ \n");
    printf("██   ██ ██   ██ ████   ██ ██  ██  ██ ████   ██ ██           ██       ██  ██  ██         ██    ██      ████  ████ \n");
    printf("██████  ███████ ██ ██  ██ █████   ██ ██ ██  ██ ██   ███     ███████   ████   ███████    ██    █████   ██ ████ ██ \n");
    printf("██   ██ ██   ██ ██  ██ ██ ██  ██  ██ ██  ██ ██ ██    ██          ██    ██         ██    ██    ██      ██  ██  ██ \n");
    printf("██████  ██   ██ ██   ████ ██   ██ ██ ██   ████  ██████      ███████    ██    ███████    ██    ███████ ██      ██ \n");
    printf("\n");
    printf("                                                                                          created by Skim\n");
    printf("\n");
}

// Displays current session time and total number of stored accounts
void showSession() {
    time_t now = time(NULL);
    int count = 0;
    int num;
    FILE *fp = fopen("database/index.txt", "r");
    printf("\n+==============================================+\n");
    printf("  Banking Management System - Session Info\n");
    printf("+==============================================+\n");
    printf("  Session Time: %s", ctime(&now));
    if (fp != NULL) {
        while (fscanf(fp, "%d", &num) == 1)
            count++;
        fclose(fp);
    }
    printf("  Total Accounts: %d\n", count);
    if (count == 0)
        printf("  Note: No accounts found. Create one to start.\n");
    printf("+==============================================+\n");
}

// Appends every significant action to a transaction log for auditing
void logTransaction(char *action) {
    FILE *fp = fopen("database/transaction.log", "a");
    if (fp != NULL) {
        time_t now = time(NULL);
        char timeStr[100];
        strcpy(timeStr, ctime(&now));
        timeStr[strlen(timeStr) - 1] = '\0'; /* Remove newline character from ctime output */
        fprintf(fp, "[%s] %s\n", timeStr, action);
        fclose(fp);
    }
}

// Pretty-print the current state of an account in tabular form
void displayAccount(Account *acc) {
    char *stat = (acc->status == 0) ? "Active" : "Closed";
    printf("\n+------------------------------------------------------------------+\n");
    printf("| Account No | Name      | PIN  | Balance    | Type     | Status   |\n");
    printf("|%11d |%10s |%5s |%11.2f |%8s |%8s |\n",
           acc->accountNumber, acc->accountName, acc->pin,
           acc->balance, acc->accountType, stat);
    printf("+------------------------------------------------------------------+\n");
}

// Lists up to 100 accounts and lets the operator choose one interactively
int listAllAccountsAndSelect(int *selectedAccountNum) {
    FILE *fp = fopen("database/index.txt", "r");
    int num, count = 0;
    int accountNumbers[100];
    Account *acc;
    int selection;

    if (fp == NULL) {
        printf("No accounts found!\n");
        return 0;
    }

    printf("\n+==================================================================+\n");
    printf("| No | Account No | Name       | Balance    | Type     | Status   |\n");
    printf("+----+------------+------------+------------+----------+----------+\n");
    while (fscanf(fp, "%d", &num) == 1 && count < 100) {
        acc = getAccount(num);
        if (acc->accountNumber != 0) {
            accountNumbers[count] = acc->accountNumber;
            count++;
            char *stat = (acc->status == 0) ? "Active" : "Closed";
            printf("| %2d |%11d |%-11s |%11.2f |%-9s |%-9s |\n",
                   count, acc->accountNumber, acc->accountName,
                   acc->balance, acc->accountType, stat);
        }
        free(acc);
    }
    printf("+==================================================================+\n");
    fclose(fp);

    if (count == 0) {
        printf("No accounts available.\n");
        return 0;
    }

    while (1) {
        printf("\nEnter account number (1-%d) or 0 to enter account number directly: ", count);
        if (scanf("%d", &selection) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clear_input_buffer(); // FIXED
            continue;
        }
        clear_input_buffer(); // FIXED

        if (selection == 0) {
            printf("Enter account number: ");
            if (scanf("%d", selectedAccountNum) != 1) {
                printf("Invalid account number!\n");
                clear_input_buffer(); // FIXED
                continue;
            }
            clear_input_buffer(); // FIXED
            return 1;
        } else if (selection >= 1 && selection <= count) {
            *selectedAccountNum = accountNumbers[selection - 1];
            return 1;
        } else {
            printf("Invalid selection! Please try again.\n");
        }
    }
}

// Serializes the in-memory Account struct into a flat text file
int saveAccount(Account *acc) {
    char filename[100];
    sprintf(filename, "database/%d.txt", acc->accountNumber);
    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "Account No: %d\n", acc->accountNumber);
        fprintf(fp, "Account Name: %s\n", acc->accountName);
        fprintf(fp, "PIN: %s\n", acc->pin);
        fprintf(fp, "Balance: %.2f\n", acc->balance);
        fprintf(fp, "Status: %d\n", acc->status);
        fprintf(fp, "Account Type: %s\n", acc->accountType);
        fprintf(fp, "ID Number: %s\n", acc->idNumber);
        fclose(fp);
        return 1;
    }
    return 0;
}

// Loads an account from disk into heap memory; caller must free the result
Account *getAccount(int num) {
    Account *acc = (Account *)malloc(sizeof(Account));
    char filename[100];
    char label[50];
    sprintf(filename, "database/%d.txt", num);
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fscanf(fp, "%s %s %d", label, label, &acc->accountNumber);
        fscanf(fp, "%s %s %s", label, label, acc->accountName);
        fscanf(fp, "%s %s", label, acc->pin);
        fscanf(fp, "%s %f", label, &acc->balance);
        fscanf(fp, "%s %d", label, &acc->status);
        fscanf(fp, "%s %s %s", label, label, acc->accountType);
        fscanf(fp, "%s %s %s", label, label, acc->idNumber);
        fclose(fp);
    } else {
        acc->accountNumber = 0;
    }
    return acc;
}

// Creates a brand new account with validated fields and persists it
void createAccount() {
    Account acc;
    int num, exists;
    int digits;
    FILE *fp;
    char logMsg[100];
    srand(time(NULL));
    digits = 7 + rand() % 3;
    if (digits == 7)
        num = 1000000 + rand() % 9000000;
    else if (digits == 8)
        num = 10000000 + rand() % 90000000;
    else
        num = 100000000 + rand() % 900000000;

    fp = fopen("database/index.txt", "r");
    if (fp != NULL) {
        while (fscanf(fp, "%d", &exists) == 1) {
            if (exists == num) {
                num++;
                break;
            }
        }
        fclose(fp);
    }
    acc.accountNumber = num;

    printf("Enter name (max 49 chars): ");
    scanf("%49s", acc.accountName);

    while (1) {
        printf("Enter ID number (min 4 chars, max 19 chars): ");
        scanf("%19s", acc.idNumber);
        if (strlen(acc.idNumber) >= 4) {
            break;
        }
        printf("ID number must be at least 4 characters!\n");
    }

    while (1) {
        int typeChoice;
        printf("Account type:\n");
        printf(" 1. Savings\n");
        printf(" 2. Current\n");
        printf("Please select (1 or 2): ");
        if (scanf("%d", &typeChoice) != 1) {
            printf("Invalid input! Please enter 1 or 2.\n");
            clear_input_buffer(); // FIXED
            continue;
        }
        clear_input_buffer(); // FIXED

        if (typeChoice == 1) {
            strcpy(acc.accountType, "Savings");
            break;
        } else if (typeChoice == 2) {
            strcpy(acc.accountType, "Current");
            break;
        } else {
            printf("Invalid choice! Please enter 1 for Savings or 2 for Current.\n");
        }
    }

    while (1) {
        printf("Enter 4-digit PIN: ");
        scanf("%4s", acc.pin);
        if (strlen(acc.pin) == 4) {
            int valid = 1;
            for (int i = 0; i < 4; i++) {
                if (!isdigit(acc.pin[i])) {
                    valid = 0;
                    break;
                }
            }
            if (valid) break;
        }
        printf("PIN must be exactly 4 digits!\n");
    }
    clear_input_buffer(); // FIXED

    acc.balance = 0.00;
    acc.status = 0;

    if (saveAccount(&acc)) {
        fp = fopen("database/index.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%d\n", acc.accountNumber);
            fclose(fp);
        }
        displayAccount(&acc);
        printf("Account created successfully!\n");
        sprintf(logMsg, "create account - Account: %d", acc.accountNumber);
        logTransaction(logMsg);
    } else {
        printf("Failed to create account!\n");
    }
}

// Removes an existing account after verifying ID and PIN
void deleteAccount() {
    int num, confirm, i;
    char pin[5], id[5], filename[100];
    char logMsg[100];
    Account *acc;

    if (!listAllAccountsAndSelect(&num)) {
        return;
    }

    acc = getAccount(num);
    if (acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }

    printf("Last 4 digits of ID: ");
    scanf("%s", id);
    int len = strlen(acc->idNumber);
    if (len < 4 || strcmp(&acc->idNumber[len - 4], id) != 0) {
        printf("ID verification failed!\n");
        free(acc);
        return;
    }

    for (i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%4s", pin);
        clear_input_buffer(); // FIXED

        if (strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            if (acc->balance > 0)
                printf("Warning: Balance is RM%.2f\n", acc->balance);
            printf("Confirm delete? (1=Yes/0=No): ");
            scanf("%d", &confirm);
            clear_input_buffer(); // FIXED

            if (confirm == 1) {
                sprintf(filename, "database/%d.txt", acc->accountNumber);
                remove(filename);
                FILE *fp1 = fopen("database/index.txt", "r");
                FILE *fp2 = fopen("database/temp.txt", "w");
                int tmp;
                if (fp1 != NULL && fp2 != NULL) {
                    while (fscanf(fp1, "%d", &tmp) == 1) {
                        if (tmp != num)
                            fprintf(fp2, "%d\n", tmp);
                    }
                    fclose(fp1);
                    fclose(fp2);
                    remove("database/index.txt");
                    rename("database/temp.txt", "database/index.txt");
                    printf("Account deleted successfully!\n");
                    sprintf(logMsg, "delete account - Account: %d", num);
                    logTransaction(logMsg);
                } else {
                    printf("Error updating index file!\n");
                    if (fp1) fclose(fp1);
                    if (fp2) fclose(fp2);
                }
            } else {
                printf("Cancelled.\n");
            }
            free(acc);
            return;
        }
        if (i < 2)
            printf("Wrong PIN! %d tries left.\n", 2 - i);
    }
    printf("Max attempts exceeded.\n");
    free(acc);
}

// Adds funds to an active account after authenticating via PIN
void deposit() {
    int num, i;
    char pin[5];
    float amount;
    char logMsg[100];
    Account *acc;

    if (!listAllAccountsAndSelect(&num)) {
        return;
    }

    acc = getAccount(num);
    if (acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }

    if (acc->status == 1) {
        printf("Account closed!\n");
        free(acc);
        return;
    }

    for (i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%4s", pin);
        clear_input_buffer(); // FIXED

        if (strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            while (1) {
                printf("Deposit amount (Max RM50,000): RM");
                if (scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    clear_input_buffer(); // FIXED
                    continue;
                }
                clear_input_buffer(); // FIXED

                if (amount <= 0) {
                    printf("Amount must be greater than RM0!\n");
                    continue;
                }
                if (amount > 50000) {
                    printf("Amount exceeds maximum limit of RM50,000!\n");
                    continue;
                }
                break;
            }

            acc->balance += amount;
            if (!saveAccount(acc)) {
                printf("Error: Failed to update account!\n");
                free(acc);
                return;
            }
            displayAccount(acc);
            printf("Deposit successful!\n");
            sprintf(logMsg, "deposit - Account: %d, Amount: RM%.2f", num, amount);
            logTransaction(logMsg);
            free(acc);
            return;
        }
        if (i < 2)
            printf("Wrong PIN! %d tries left.\n", 2 - i);
    }
    printf("Max attempts exceeded.\n");
    free(acc);
}

// Deducts funds from an active account while preventing overdrafts
void withdraw() {
    int num, i;
    char pin[5];
    float amount;
    char logMsg[100];
    Account *acc;

    if (!listAllAccountsAndSelect(&num)) {
        return;
    }

    acc = getAccount(num);
    if (acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }

    if (acc->status == 1) {
        printf("Account closed!\n");
        free(acc);
        return;
    }

    for (i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%4s", pin);
        clear_input_buffer(); // FIXED

        if (strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            printf("Available balance: RM%.2f\n", acc->balance);
            while (1) {
                printf("Withdraw amount: RM");
                if (scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    clear_input_buffer(); // FIXED
                    continue;
                }
                clear_input_buffer(); // FIXED

                if (amount <= 0) {
                    printf("Invalid amount! Must be greater than RM0.\n");
                    continue;
                }
                if (amount > acc->balance) {
                    printf("Insufficient funds! Available: RM%.2f\n", acc->balance);
                    continue;
                }
                break;
            }

            acc->balance -= amount;
            if (!saveAccount(acc)) {
                printf("Error: Failed to update account!\n");
                free(acc);
                return;
            }
            displayAccount(acc);
            printf("Withdrawal successful!\n");
            sprintf(logMsg, "withdrawal - Account: %d, Amount: RM%.2f", num, amount);
            logTransaction(logMsg);
            free(acc);
            return;
        }
        if (i < 2)
            printf("Wrong PIN! %d tries left.\n", 2 - i);
    }
    printf("Max attempts exceeded.\n");
    free(acc);
}

// Transfers funds between two accounts and applies conditional fees
void remittance() {
    int sender, receiver, i;
    char pin[5];
    float amount, fee = 0;
    char logMsg[200];
    Account *acc1, *acc2;

    printf("=== Select Sender Account ===\n");
    if (!listAllAccountsAndSelect(&sender)) {
        return;
    }

    printf("\n=== Select Receiver Account ===\n");
    if (!listAllAccountsAndSelect(&receiver)) {
        return;
    }

    if (sender == receiver) {
        printf("Sender and receiver must be different!\n");
        return;
    }

    acc1 = getAccount(sender);
    acc2 = getAccount(receiver);

    if (acc1->accountNumber == 0) {
        printf("Sender account not found!\n");
        free(acc1);
        free(acc2);
        return;
    }
    if (acc2->accountNumber == 0) {
        printf("Receiver account not found!\n");
        free(acc1);
        free(acc2);
        return;
    }
    if (acc1->status == 1) {
        printf("Sender account is closed!\n");
        free(acc1);
        free(acc2);
        return;
    }
    if (acc2->status == 1) {
        printf("Receiver account is closed!\n");
        free(acc1);
        free(acc2);
        return;
    }

    for (i = 0; i < 3; i++) {
        printf("Enter sender PIN: ");
        scanf("%4s", pin);
        clear_input_buffer(); // FIXED

        if (strcmp(acc1->pin, pin) == 0) {
            displayAccount(acc1);
            while (1) {
                printf("\nEnter transfer amount: RM");
                if (scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    clear_input_buffer(); // FIXED
                    continue;
                }
                clear_input_buffer(); // FIXED

                if (amount <= 0) {
                    printf("Invalid amount! Must be greater than RM0.\n");
                    continue;
                }

                if (strcmp(acc1->accountType, "Savings") == 0 && strcmp(acc2->accountType, "Current") == 0) {
                    fee = amount * 0.02;
                    printf("Remittance fee (2%%): RM%.2f\n", fee);
                } else if (strcmp(acc1->accountType, "Current") == 0 && strcmp(acc2->accountType, "Savings") == 0) {
                    fee = amount * 0.03;
                    printf("Remittance fee (3%%): RM%.2f\n", fee);
                } else {
                    printf("No remittance fee applied.\n");
                }

                if (acc1->balance < amount + fee) {
                    printf("Insufficient funds! Need: RM%.2f (including fee)\n", amount + fee);
                    printf("Available: RM%.2f\n", acc1->balance);
                    char retry;
                    printf("Try different amount? (y/n): ");
                    scanf(" %c", &retry);
                    clear_input_buffer(); // FIXED
                    if (retry == 'y' || retry == 'Y') {
                        continue;
                    } else {
                        free(acc1);
                        free(acc2);
                        return;
                    }
                }
                break;
            }

            acc1->balance -= (amount + fee);
            acc2->balance += amount;
            if (!saveAccount(acc1) || !saveAccount(acc2)) {
                printf("Error: Failed to update accounts!\n");
                free(acc1);
                free(acc2);
                return;
            }
            printf("\n--- Sender Account ---\n");
            displayAccount(acc1);
            printf("\n--- Receiver Account ---\n");
            displayAccount(acc2);
            printf("\nRemittance successful!\n");
            sprintf(logMsg, "remittance - From: %d to %d, Amount: RM%.2f, Fee: RM%.2f", sender, receiver, amount, fee);
            logTransaction(logMsg);
            free(acc1);
            free(acc2);
            return;
        }
        if (i < 2)
            printf("Wrong PIN! %d tries left.\n", 2 - i);
    }
    printf("Max attempts exceeded.\n");
    free(acc1);
    free(acc2);
}

// User input to the right operation based on menu selection
void mainMenu() {
    char input[50];
    while (1) {
        printf("\n+========================================+\n");
        printf("| 1. Deposit    | 4. Create  Account     |\n");
        printf("| 2. Withdraw   | 5. Delete  Account     |\n");
        printf("| 3. Remittance | 0. Exit  System        |\n");
        printf("+========================================+\n");
        printf("Please select (number or keyword): ");

        // FIXED: Use fgets to safely read a line, preventing EOF infinite loops
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n==============================================\n");
            printf("End of input detected. Exiting system.\n");
            logTransaction("exit system (EOF)");
            exit(0);
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // If user just pressed Enter, redisplay menu
        if (strlen(input) == 0) {
            continue;
        }

        // Convert to lowercase for case-insensitive comparison
        for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }

        if (strcmp(input, "1") == 0 || strcmp(input, "deposit") == 0)
            deposit();
        else if (strcmp(input, "2") == 0 || strcmp(input, "withdraw") == 0 || strcmp(input, "withdrawal") == 0)
            withdraw();
        else if (strcmp(input, "3") == 0 || strcmp(input, "remittance") == 0 || strcmp(input, "transfer") == 0)
            remittance();
        else if (strcmp(input, "4") == 0 || strcmp(input, "create") == 0 || strcmp(input, "new") == 0)
            createAccount();
        else if (strcmp(input, "5") == 0 || strcmp(input, "delete") == 0 || strcmp(input, "remove") == 0)
            deleteAccount();
        else if (strcmp(input, "0") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("==============================================\n");
            printf("Thank you for using Banking System. Goodbye!\n");
            logTransaction("exit system");
            exit(0);
        } else {
            printf("==============================================\n");
            printf("Invalid option! Please try again.\n");
        }
    }
}

// PrayForSuccess (º̩̩́⌣º̩̩̀ʃƪ)
