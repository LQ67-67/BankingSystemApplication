// Banking Management System
// Author: Skim
// Description: A comprehensive banking system with account management, transactions, and audit logging

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)  
#else
    #include <sys/stat.h>
#endif

typedef struct {
    int accountNumber;
    char accountName[50];
    char pin[5];
    float balance;
    int status;
    char accountType[10];
    char idNumber[20];
} Account;

void displayAccount(Account *acc);
int saveAccount(Account* acc);
Account* getAccount(int num);
void welcome();
void showSession();
void logTransaction(char* action);
void mainMenu();
void createAccount();
void deleteAccount();
void deposit();
void withdraw();
void remittance();
void initDatabase();
int listAllAccountsAndSelect(int *selectedAccountNum);

// ================= VALIDATION HELPER FUNCTIONS =================

int isValidName(const char* name) {
    if(strlen(name) < 2 || strlen(name) > 49) return 0;
    int has_alpha = 0;
    for(int i = 0; name[i] != '\0'; i++) {
        if(isalpha((unsigned char)name[i])) {
            has_alpha = 1;
        } else if(name[i] != ' ') {
            return 0; // Only letters and spaces allowed (no symbols)
        }
    }
    return has_alpha;
}

int isValidID(const char* id) {
    int len = strlen(id);
    if(len < 4 || len > 19) return 0;
    for(int i = 0; id[i] != '\0'; i++) {
        if(!isalnum((unsigned char)id[i])) return 0; // Alphanumeric only, no symbols
    }
    return 1;
}

int isValidPIN(const char* pin) {
    if(strlen(pin) != 4) return 0;
    for(int i = 0; i < 4; i++) {
        if(!isdigit((unsigned char)pin[i])) return 0;
    }
    return 1;
}

int validateAmount(const char* str, float* out_amount) {
    int dot_count = 0;
    int decimal_digits = 0;
    int has_digit = 0;
    
    for(int i = 0; str[i] != '\0'; i++) {
        if(str[i] == '.') {
            dot_count++;
            if(dot_count > 1) return 0;
            decimal_digits = 0;
        } else if(isdigit((unsigned char)str[i])) {
            has_digit = 1;
            if(dot_count == 1) {
                decimal_digits++;
                if(decimal_digits > 2) return 0; // Max 2 decimal places
            }
        } else {
            return 0; // Symbol or invalid character rejected
        }
    }
    
    if(!has_digit) return 0;
    
    *out_amount = atof(str);
    return 1;
}

// ================= END VALIDATION HELPERS =================

int main() {
    initDatabase();
    welcome();
    showSession();
    mainMenu();
    return 0;
}

void initDatabase() {
    #ifdef _WIN32
        mkdir("database");
    #else
        mkdir("database", 0700);
    #endif
    
    FILE *fp = fopen("database/index.txt", "a");
    if(fp != NULL) fclose(fp);
}

void welcome() {
    printf("\n");
    printf("██████   █████  ███    ██ ██   ██ ██ ███    ██  ██████      ███████ ██    ██ ███████ ████████ ███████ ███    ███ \n");
    printf("██   ██ ██   ██ ████   ██ ██  ██  ██ ████   ██ ██           ██       ██  ██  ██         ██    ██      ████  ████ \n");
    printf("██████  ███████ ██ ██  ██ █████   ██ ██ ██  ██ ██   ███     ███████   ████   ███████    ██    █████   ██ ████ ██ \n");
    printf("██   ██ ██   ██ ██  ██ ██ ██  ██  ██ ██  ██ ██ ██    ██          ██    ██         ██    ██    ██      ██  ██  ██ \n");
    printf("██████  ██   ██ ██   ████ ██   ██ ██ ██   ████  ██████      ███████    ██    ███████    ██    ███████ ██      ██ \n");
    printf("\n                                                                                                  created by Skim\n\n");
}

void showSession() {
    time_t now = time(NULL);
    int count = 0, num;
    FILE *fp = fopen("database/index.txt", "r");
    
    printf("\n+==============================================+\n");
    printf("  Banking Management System - Session Info\n");
    printf("+==============================================+\n");
    printf("  Session Time: %s", ctime(&now));
    
    if(fp != NULL) {
        while(fscanf(fp, "%d", &num) == 1) count++;
        fclose(fp);
    }
    
    printf("  Total Accounts: %d\n", count);
    if(count == 0) printf("  Note: No accounts found. Create one to start.\n");
    printf("+==============================================+\n");
}

void logTransaction(char* action) {
    FILE *fp = fopen("database/transaction.log", "a");
    if(fp != NULL) {
        time_t now = time(NULL);
        char timeStr[100];
        strcpy(timeStr, ctime(&now));
        timeStr[strlen(timeStr)-1] = '\0';
        fprintf(fp, "[%s] %s\n", timeStr, action);
        fclose(fp);
    }
}

void displayAccount(Account *acc) {
    char *stat = (acc->status == 0) ? "Active" : "Closed";
    printf("\n+------------------------------------------------------------------+\n");
    printf("| Account No | Name      | PIN  | Balance    | Type     | Status   |\n");
    printf("|%11d |%10s |%5s |%11.2f |%8s  |%8s  |\n",
           acc->accountNumber, acc->accountName, acc->pin, 
           acc->balance, acc->accountType, stat);
    printf("+------------------------------------------------------------------+\n");
}

int listAllAccountsAndSelect(int *selectedAccountNum) {
    FILE *fp = fopen("database/index.txt", "r");
    int num, count = 0, accountNumbers[100], selection;
    Account *acc;
    
    if(fp == NULL) { printf("No accounts found!\n"); return 0; }
    
    printf("\n+==================================================================+\n");
    printf("| No | Account No | Name       | Balance    | Type     | Status   |\n");
    printf("+----+------------+------------+------------+----------+----------+\n");
    
    while(fscanf(fp, "%d", &num) == 1 && count < 100) {
        acc = getAccount(num);
        if(acc->accountNumber != 0) {
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
    
    if(count == 0) { printf("No accounts available.\n"); return 0; }
    
    while(1) {
        printf("\nEnter account number (1-%d) or 0 to enter account number directly: ", count);
        if(scanf("%d", &selection) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while(getchar() != '\n');
            continue;
        }
        getchar();
        
        if(selection == 0) {
            printf("Enter account number: ");
            if(scanf("%d", selectedAccountNum) != 1) {
                printf("Invalid account number!\n");
                while(getchar() != '\n');
                continue;
            }
            getchar();
            return 1;
        } else if(selection >= 1 && selection <= count) {
            *selectedAccountNum = accountNumbers[selection - 1];
            return 1;
        } else {
            printf("Invalid selection! Please try again.\n");
        }
    }
}

int saveAccount(Account* acc) {
    char filename[100];
    sprintf(filename, "database/%d.txt", acc->accountNumber);
    FILE *fp = fopen(filename, "w");
    if(fp != NULL) {
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

Account* getAccount(int num) {
    Account *acc = (Account*)malloc(sizeof(Account));
    char filename[100], label[50];
    sprintf(filename, "database/%d.txt", num);
    FILE *fp = fopen(filename, "r");
    
    if(fp != NULL) {
        fscanf(fp, "%s %s %d\n", label, label, &acc->accountNumber);
        fscanf(fp, "%s %s %s\n", label, label, acc->accountName);
        fscanf(fp, "%s %s\n", label, acc->pin);
        fscanf(fp, "%s %f\n", label, &acc->balance);
        fscanf(fp, "%s %d\n", label, &acc->status);
        fscanf(fp, "%s %s %s\n", label, label, acc->accountType);
        fscanf(fp, "%s %s %s\n", label, label, acc->idNumber);
        fclose(fp);
    } else {
        acc->accountNumber = 0;
    }
    return acc;
}

void createAccount() {
    Account acc;
    int num, exists, digits;
    FILE *fp;
    char logMsg[100];
    
    srand(time(NULL));
    digits = 7 + rand() % 3;
    if(digits == 7) num = 1000000 + rand() % 9000000;
    else if(digits == 8) num = 10000000 + rand() % 90000000;
    else num = 100000000 + rand() % 900000000;
    
    fp = fopen("database/index.txt", "r");
    if(fp != NULL) {
        while(fscanf(fp, "%d", &exists) == 1) {
            if(exists == num) { num++; break; }
        }
        fclose(fp);
    }
    acc.accountNumber = num;
    
    // FIX 1: Name validation (no symbols only, letters and spaces only)
    while(1) {
        printf("Enter name (max 49 chars, letters and spaces only): ");
        int c; while((c = getchar()) != '\n' && c != EOF); // Clear buffer
        fgets(acc.accountName, 50, stdin);
        acc.accountName[strcspn(acc.accountName, "\n")] = '\0';
        if(isValidName(acc.accountName)) break;
        printf("Invalid name! Please use only letters and spaces (min 2 characters).\n");
    }

    // FIX 2: ID validation (alphanumeric only, no symbols)
    while(1) {
        printf("Enter ID number (min 4 chars, max 19 chars, alphanumeric only): ");
        scanf("%19s", acc.idNumber);
        if(isValidID(acc.idNumber)) break;
        printf("Invalid ID! Must be 4-19 alphanumeric characters without symbols.\n");
    }
    
    while(1) {
        int typeChoice;
        printf("Account type:\n  1. Savings\n  2. Current\nPlease select (1 or 2): ");
        if(scanf("%d", &typeChoice) != 1) {
            printf("Invalid input! Please enter 1 or 2.\n");
            while(getchar() != '\n');
            continue;
        }
        getchar();
        if(typeChoice == 1) { strcpy(acc.accountType, "Savings"); break; }
        else if(typeChoice == 2) { strcpy(acc.accountType, "Current"); break; }
        else { printf("Invalid choice! Please enter 1 for Savings or 2 for Current.\n"); }
    }
    
    // FIX 3: PIN validation (exactly 4 digits, reject >4 chars)
    while(1) {
        char pin_input[20];
        printf("Enter 4-digit PIN: ");
        scanf("%19s", pin_input);
        if(isValidPIN(pin_input)) { strcpy(acc.pin, pin_input); break; }
        printf("PIN must be exactly 4 digits!\n");
    }
    
    int c; while((c = getchar()) != '\n' && c != EOF); // Clear buffer

    acc.balance = 0.00;
    acc.status = 0;
    
    if(saveAccount(&acc)) {
        fp = fopen("database/index.txt", "a");
        if(fp != NULL) { fprintf(fp, "%d\n", acc.accountNumber); fclose(fp); }
        displayAccount(&acc);
        printf("Account created successfully!\n");
        sprintf(logMsg, "create account - Account: %d", acc.accountNumber);
        logTransaction(logMsg);
    } else {
        printf("Failed to create account!\n");
    }
}

void deleteAccount() {
    int num, confirm, i;
    char pin_input[20], id[20], filename[100];
    char logMsg[100];
    Account *acc;
    
    if(!listAllAccountsAndSelect(&num)) return;
    acc = getAccount(num);
    if(acc->accountNumber == 0) { printf("Account not found!\n"); free(acc); return; }
    
    printf("Last 4 digits of ID: ");
    scanf("%19s", id);
    int len = strlen(acc->idNumber);
    if(len < 4 || strcmp(&acc->idNumber[len-4], id) != 0) {
        printf("ID verification failed!\n"); free(acc); return;
    }
    
    for(i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%19s", pin_input);
        int c; while((c = getchar()) != '\n' && c != EOF);
        
        if(isValidPIN(pin_input) && strcmp(acc->pin, pin_input) == 0) {
            displayAccount(acc);
            if(acc->balance > 0) printf("Warning: Balance is RM%.2f\n", acc->balance);
            printf("Confirm delete? (1=Yes/0=No): ");
            scanf("%d", &confirm); getchar();
            
            if(confirm == 1) {
                sprintf(filename, "database/%d.txt", acc->accountNumber);
                remove(filename);
                FILE *fp1 = fopen("database/index.txt", "r");
                FILE *fp2 = fopen("database/temp.txt", "w");
                int tmp;
                if(fp1 != NULL && fp2 != NULL) {
                    while(fscanf(fp1, "%d", &tmp) == 1) {
                        if(tmp != num) fprintf(fp2, "%d\n", tmp);
                    }
                    fclose(fp1); fclose(fp2);
                    remove("database/index.txt");
                    rename("database/temp.txt", "database/index.txt");
                    printf("Account deleted successfully!\n");
                    sprintf(logMsg, "delete account - Account: %d", num);
                    logTransaction(logMsg);
                } else {
                    printf("Error updating index file!\n");
                    if(fp1) fclose(fp1); if(fp2) fclose(fp2);
                }
            } else { printf("Cancelled.\n"); }
            free(acc); return;
        }
        if(i < 2) printf("Wrong PIN! %d tries left.\n", 2-i);
    }
    printf("Max attempts exceeded.\n"); free(acc);
}

void deposit() {
    int num, i;
    float amount;
    char logMsg[100];
    Account *acc;
    
    if(!listAllAccountsAndSelect(&num)) return;
    acc = getAccount(num);
    if(acc->accountNumber == 0) { printf("Account not found!\n"); free(acc); return; }
    if(acc->status == 1) { printf("Account closed!\n"); free(acc); return; }
    
    for(i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        char pin_input[20];
        scanf("%19s", pin_input);
        int c; while((c = getchar()) != '\n' && c != EOF);
        
        if(isValidPIN(pin_input) && strcmp(acc->pin, pin_input) == 0) {
            displayAccount(acc);
            while(1) {
                char amount_str[50];
                printf("Deposit amount (Max RM50,000): RM");
                if(scanf("%49s", amount_str) != 1) {
                    printf("Invalid input!\n"); while(getchar() != '\n'); continue;
                }
                // FIX 4 & 5: Validate no symbols and max 2 decimal places
                if(!validateAmount(amount_str, &amount)) {
                    printf("Invalid input! Please enter a valid number without symbols (max 2 decimal places).\n");
                    continue;
                }
                if(amount <= 0) { printf("Amount must be greater than RM0!\n"); continue; }
                if(amount > 50000) { printf("Amount exceeds maximum limit of RM50,000!\n"); continue; }
                break;
            }
            acc->balance += amount;
            if(!saveAccount(acc)) { printf("Error: Failed to update account!\n"); free(acc); return; }
            displayAccount(acc);
            printf("Deposit successful!\n");
            sprintf(logMsg, "deposit - Account: %d, Amount: RM%.2f", num, amount);
            logTransaction(logMsg);
            free(acc); return;
        }
        if(i < 2) printf("Wrong PIN! %d tries left.\n", 2-i);
    }
    printf("Max attempts exceeded.\n"); free(acc);
}

void withdraw() {
    int num, i;
    float amount;
    char logMsg[100];
    Account *acc;
    
    if(!listAllAccountsAndSelect(&num)) return;
    acc = getAccount(num);
    if(acc->accountNumber == 0) { printf("Account not found!\n"); free(acc); return; }
    if(acc->status == 1) { printf("Account closed!\n"); free(acc); return; }
    
    for(i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        char pin_input[20];
        scanf("%19s", pin_input);
        int c; while((c = getchar()) != '\n' && c != EOF);
        
        if(isValidPIN(pin_input) && strcmp(acc->pin, pin_input) == 0) {
            displayAccount(acc);
            printf("Available balance: RM%.2f\n", acc->balance);
            while(1) {
                char amount_str[50];
                printf("Withdraw amount: RM");
                if(scanf("%49s", amount_str) != 1) {
                    printf("Invalid input!\n"); while(getchar() != '\n'); continue;
                }
                if(!validateAmount(amount_str, &amount)) {
                    printf("Invalid input! Please enter a valid number without symbols (max 2 decimal places).\n");
                    continue;
                }
                if(amount <= 0) { printf("Invalid amount! Must be greater than RM0.\n"); continue; }
                if(amount > acc->balance) { printf("Insufficient funds! Available: RM%.2f\n", acc->balance); continue; }
                break;
            }
            acc->balance -= amount;
            if(!saveAccount(acc)) { printf("Error: Failed to update account!\n"); free(acc); return; }
            displayAccount(acc);
            printf("Withdrawal successful!\n");
            sprintf(logMsg, "withdrawal - Account: %d, Amount: RM%.2f", num, amount);
            logTransaction(logMsg);
            free(acc); return;
        }
        if(i < 2) printf("Wrong PIN! %d tries left.\n", 2-i);
    }
    printf("Max attempts exceeded.\n"); free(acc);
}

void remittance() {
    int sender, receiver, i;
    float amount, fee = 0;
    char logMsg[200];
    Account *acc1, *acc2;
    
    printf("=== Select Sender Account ===\n");
    if(!listAllAccountsAndSelect(&sender)) return;
    printf("\n=== Select Receiver Account ===\n");
    if(!listAllAccountsAndSelect(&receiver)) return;
    if(sender == receiver) { printf("Sender and receiver must be different!\n"); return; }
    
    acc1 = getAccount(sender);
    acc2 = getAccount(receiver);
    if(acc1->accountNumber == 0 || acc2->accountNumber == 0) {
        printf("Account not found!\n"); free(acc1); free(acc2); return;
    }
    if(acc1->status == 1 || acc2->status == 1) {
        printf("One or both accounts are closed!\n"); free(acc1); free(acc2); return;
    }
    
    for(i = 0; i < 3; i++) {
        printf("Enter sender PIN: ");
        char pin_input[20];
        scanf("%19s", pin_input);
        int c; while((c = getchar()) != '\n' && c != EOF);
        
        if(isValidPIN(pin_input) && strcmp(acc1->pin, pin_input) == 0) {
            displayAccount(acc1);
            while(1) {
                char amount_str[50];
                printf("\nEnter transfer amount: RM");
                if(scanf("%49s", amount_str) != 1) {
                    printf("Invalid input!\n"); while(getchar() != '\n'); continue;
                }
                if(!validateAmount(amount_str, &amount)) {
                    printf("Invalid input! Please enter a valid number without symbols (max 2 decimal places).\n");
                    continue;
                }
                if(amount <= 0) { printf("Invalid amount! Must be greater than RM0.\n"); continue; }
                
                if(strcmp(acc1->accountType, "Savings") == 0 && strcmp(acc2->accountType, "Current") == 0) {
                    fee = amount * 0.02; printf("Remittance fee (2%%): RM%.2f\n", fee);
                } else if(strcmp(acc1->accountType, "Current") == 0 && strcmp(acc2->accountType, "Savings") == 0) {
                    fee = amount * 0.03; printf("Remittance fee (3%%): RM%.2f\n", fee);
                } else {
                    printf("No remittance fee applied.\n");
                }
                
                if(acc1->balance < amount + fee) {
                    printf("Insufficient funds! Need: RM%.2f (including fee)\n", amount + fee);
                    printf("Available: RM%.2f\n", acc1->balance); // FIX 6: Was incorrectly using undefined 'acc'
                    char retry;
                    printf("Try different amount? (y/n): ");
                    scanf(" %c", &retry);
                    while(getchar() != '\n');
                    if(retry == 'y' || retry == 'Y') continue;
                    else { free(acc1); free(acc2); return; }
                }
                break;
            }
            acc1->balance -= (amount + fee);
            acc2->balance += amount;
            if(!saveAccount(acc1) || !saveAccount(acc2)) {
                printf("Error: Failed to update accounts!\n"); free(acc1); free(acc2); return;
            }
            printf("\n--- Sender Account ---\n"); displayAccount(acc1);
            printf("\n--- Receiver Account ---\n"); displayAccount(acc2);
            printf("\nRemittance successful!\n");
            sprintf(logMsg, "remittance - From: %d to %d, Amount: RM%.2f, Fee: RM%.2f", sender, receiver, amount, fee);
            logTransaction(logMsg);
            free(acc1); free(acc2); return;
        }
        if(i < 2) printf("Wrong PIN! %d tries left.\n", 2-i);
    }
    printf("Max attempts exceeded.\n"); free(acc1); free(acc2);
}

void mainMenu() {
    char input[20];
    int c;
    while(1) {
        printf("\n+========================================+\n");
        printf("| 1. Deposit    | 4. Create  Account     |\n");
        printf("| 2. Withdraw   | 5. Delete  Account     |\n");
        printf("| 3. Remittance | 0. Exit  System        |\n");
        printf("+========================================+\n");
        printf("Please select (number or keyword): ");
        
        if(scanf("%s", input) != 1) {
            while((c = getchar()) != '\n' && c != EOF);
            printf("==============================================\nInvalid input!\n");
            continue;
        }
        while((c = getchar()) != '\n' && c != EOF);
        for(int i = 0; input[i]; i++) input[i] = tolower(input[i]);
        
        if(strcmp(input, "1") == 0 || strcmp(input, "deposit") == 0) deposit();
        else if(strcmp(input, "2") == 0 || strcmp(input, "withdraw") == 0 || strcmp(input, "withdrawal") == 0) withdraw();
        else if(strcmp(input, "3") == 0 || strcmp(input, "remittance") == 0 || strcmp(input, "transfer") == 0) remittance();
        else if(strcmp(input, "4") == 0 || strcmp(input, "create") == 0 || strcmp(input, "new") == 0) createAccount();
        else if(strcmp(input, "5") == 0 || strcmp(input, "delete") == 0 || strcmp(input, "remove") == 0) deleteAccount();
        else if(strcmp(input, "0") == 0 || strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("==============================================\nThank you for using Banking System. Goodbye!\n");
            logTransaction("exit system");
            exit(0);
        } else {
            printf("==============================================\nInvalid option! Please try again.\n");
        }
    }
}
