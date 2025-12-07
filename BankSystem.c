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
// This structure represents a single bank account with all required fields
typedef struct {
    int accountNumber;    // Unique identifier for the account (7-9 digits)
    char accountName[50]; // Account holder's name (max 49 characters + null terminator)
    char pin[5];          // 4-digit PIN + null terminator for security
    float balance;        // Current account balance in Malaysian Ringgit
    int status;           // Account status: 0=active, 1=closed
    char accountType[10]; // Account type: "Savings" or "Current"
    char idNumber[20];    // Identification number for verification (min 4 chars)
} Account;

// Function prototypes - declarations of all functions used in the system
void displayAccount(Account *acc);                    // Display account details in formatted table
int saveAccount(Account* acc);                        // Save account data to file
Account* getAccount(int num);                         // Load account data from file
void welcome();                                       // Display welcome banner
void showSession();                                   // Show current session information
void logTransaction(char* action);                    // Log transactions for audit trail
void mainMenu();                                      // Display main menu and handle user input
void createAccount();                                 // Create a new bank account
void deleteAccount();                                 // Delete an existing account
void deposit();                                       // Deposit money into an account
void withdraw();                                      // Withdraw money from an account
void remittance();                                    // Transfer money between accounts
void initDatabase();                                  // Initialize database directory and files
int listAllAccountsAndSelect(int *selectedAccountNum); // List all accounts and allow selection 

// Entry point: bootstrap storage, show intro, and start interactive menu
// This is the main function that controls the program flow
int main() {
    // Prepare storage files, greet user, show session info, then enter menu loop
    initDatabase();    // Ensure database directory and files exist
    welcome();         // Display welcome banner with ASCII art
    showSession();     // Show current session time and account count
    mainMenu();        // Enter main menu loop for user interaction
    
    return 0;          // Successful program termination
}

// Ensures the backing directory/index file exist before any operations run
// This function initializes the database structure for the banking system
void initDatabase() {
    // Create database directory with appropriate permissions
    #ifdef _WIN32
        mkdir("database");              // Windows: simple directory creation
    #else
        mkdir("database", 0700);        // Unix/Linux: with read/write/execute permissions for owner only
    #endif
    
    // Ensure index.txt exists so future reads do not fail
    // The index file tracks all account numbers for quick enumeration
    FILE *fp = fopen("database/index.txt", "a");
    if(fp != NULL) {
        fclose(fp);                     // Close immediately after creating/opening
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
    printf("                                                                                                  created by Skim\n");
    printf("\n");
}

// Displays current session time and total number of stored accounts
// This function provides session context and system status information
void showSession() {
    time_t now = time(NULL);    // Get current system time
    int count = 0;              // Counter for total accounts
    int num;                    // Temporary variable for reading account numbers
    FILE *fp = fopen("database/index.txt", "r");  // Open index file for reading
    
    printf("\n+==============================================+\n");
    printf("  Banking Management System - Session Info\n");
    printf("+==============================================+\n");
    printf("  Session Time: %s", ctime(&now));  // Display current time
    
    if(fp != NULL) {
        // Count how many account numbers are recorded in the index file
        // This gives us the total number of accounts in the system
        while(fscanf(fp, "%d", &num) == 1)
            count++;
        fclose(fp);  // Always close file handles
    }
    
    printf("  Total Accounts: %d\n", count);
    if(count == 0)
        printf("  Note: No accounts found. Create one to start.\n");
    printf("+==============================================+\n");
}

// Appends every significant action to a transaction log for auditing
// This function maintains a complete audit trail of all system activities
void logTransaction(char* action) {
    FILE *fp = fopen("database/transaction.log", "a");  // Open log file in append mode
    if(fp != NULL) {
        time_t now = time(NULL);    // Get current timestamp
        char timeStr[100];
        strcpy(timeStr, ctime(&now));
        timeStr[strlen(timeStr)-1] = '\0'; /*Remove newline character from ctime output*/
        
        // Append the action with a timestamp to the log file
        // Format: [YYYY-MM-DD HH:MM:SS] action description
        fprintf(fp, "[%s] %s\n", timeStr, action);
        fclose(fp);  // Ensure log file is properly closed
    }
}

// Pretty-print the current state of an account in tabular form
void displayAccount(Account *acc) {
    // Pretty-print the current state of an account in tabular form
    char *stat = (acc->status == 0) ? "Active" : "Closed";
    printf("\n+------------------------------------------------------------------+\n");
    printf("| Account No | Name      | PIN  | Balance    | Type     | Status   |\n");
    printf("|%11d |%10s |%5s |%11.2f |%8s  |%8s  |\n",
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
    // Using an index file avoids scanning the whole directory on each request
    
    if(fp == NULL) {
        // No index file means no accounts were ever created
        printf("No accounts found!\n");
        return 0;
    }
    
    printf("\n+==================================================================+\n");
    printf("| No | Account No | Name       | Balance    | Type     | Status   |\n");
    printf("+----+------------+------------+------------+----------+----------+\n");
    
    while(fscanf(fp, "%d", &num) == 1 && count < 100) {
        // Load each account so we can show its latest balance and status
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
    
    if(count == 0) {
        printf("No accounts available.\n");
        return 0;
    }
    
    while(1) {
        // Keep asking until the operator chooses a valid account or cancels
        printf("\nEnter account number (1-%d) or 0 to enter account number directly: ", count);
        if(scanf("%d", &selection) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while(getchar() != '\n');
            continue;
        }
        getchar();
        
        if(selection == 0) {
            // Allow operators to type the exact account number if they know it
            printf("Enter account number: ");
            if(scanf("%d", selectedAccountNum) != 1) {
                printf("Invalid account number!\n");
                while(getchar() != '\n');
                continue;
            }
            getchar();
            return 1;
        }
        else if(selection >= 1 && selection <= count) {
            *selectedAccountNum = accountNumbers[selection - 1];
            return 1;
        }
        else {
            printf("Invalid selection! Please try again.\n");
        }
    }
}

// Serializes the in-memory Account struct into a flat text file
int saveAccount(Account* acc) {
    char filename[100];
    sprintf(filename, "database/%d.txt", acc->accountNumber);
    FILE *fp = fopen(filename, "w");
    
    if(fp != NULL) {
        // Persist each field in a simple key/value format for easy parsing
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
Account* getAccount(int num) {
    Account *acc = (Account*)malloc(sizeof(Account));
    char filename[100];
    char label[50];
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
        // Signal missing account by zeroing the account number
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
    
    if(digits == 7)
        num = 1000000 + rand() % 9000000;
    else if(digits == 8)
        num = 10000000 + rand() % 90000000;
    else
        num = 100000000 + rand() % 900000000;
    // Randomize 7-9 digit account numbers to keep IDs unique without manual input
    
    fp = fopen("database/index.txt", "r");
    if(fp != NULL) {
        // Make sure randomly chosen number not already in use
        while(fscanf(fp, "%d", &exists) == 1) {
            if(exists == num) {
                num++;
                break;
            }
        }
        fclose(fp);
    }
    
    acc.accountNumber = num;
    
    printf("Enter name (max 49 chars): ");
    scanf("%49s", acc.accountName);
    
    // Require ID numbers that are long enough for later verification checks
    while(1) {
        printf("Enter ID number (min 4 chars, max 19 chars): ");
        scanf("%19s", acc.idNumber);
        if(strlen(acc.idNumber) >= 4) {
            break;
        }
        printf("ID number must be at least 4 characters!\n");
    }
    
    // Only allow recognized account classes to avoid typos in downstream logic
    while(1) {
        printf("Account type (Savings/Current): ");
        scanf("%9s", acc.accountType);
        if(strcmp(acc.accountType, "Savings") == 0 || 
           strcmp(acc.accountType, "Current") == 0) {
            break;
        }
        printf("Invalid type! Please enter 'Savings' or 'Current'\n");
    }
    
    // Force numeric PINs with exactly four digits to simplify authentication
    while(1) {
        printf("Enter 4-digit PIN: ");
        scanf("%4s", acc.pin);
        if(strlen(acc.pin) == 4) {
            int valid = 1;
            for(int i = 0; i < 4; i++) {
                if(!isdigit(acc.pin[i])) {
                    valid = 0;
                    break;
                }
            }
            if(valid) break;
        }
        printf("PIN must be exactly 4 digits!\n");
    }
    getchar();
    
    acc.balance = 0.00;
    acc.status = 0;
    
    if(saveAccount(&acc)) {
        // Append new account number to index for quick listing later
        fp = fopen("database/index.txt", "a");
        if(fp != NULL) {
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
    
    if(!listAllAccountsAndSelect(&num)) {
        return;
    }
    
    acc = getAccount(num);
    if(acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }
    
    printf("Last 4 digits of ID: ");
    scanf("%s", id);
    
    int len = strlen(acc->idNumber);
    // Compare the provided ID suffix with the stored ID for extra validation
    if(len < 4 || strcmp(&acc->idNumber[len-4], id) != 0) {
        printf("ID verification failed!\n");
        free(acc);
        return;
    }
    
    for(i = 0; i < 3; i++) {
        // Give the user up to three attempts to authenticate
        printf("Enter PIN: ");
        scanf("%4s", pin);
        getchar();
        
        if(strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            
            if(acc->balance > 0)
                // Warn operators so they can refund customers before deletion
                printf("Warning: Balance is RM%.2f\n", acc->balance);
            
            printf("Confirm delete? (1=Yes/0=No): ");
            scanf("%d", &confirm);
            getchar();
            
            if(confirm == 1) {
                sprintf(filename, "database/%d.txt", acc->accountNumber);
                remove(filename);
                
                FILE *fp1 = fopen("database/index.txt", "r");
                FILE *fp2 = fopen("database/temp.txt", "w");
                int tmp;
                
                if(fp1 != NULL && fp2 != NULL) {
                    // Copy all other account numbers into a temp file
                    while(fscanf(fp1, "%d", &tmp) == 1) {
                        if(tmp != num)
                            fprintf(fp2, "%d\n", tmp);
                    }
                    fclose(fp1);
                    fclose(fp2);
                    // Replace original index atomically
                    remove("database/index.txt");
                    rename("database/temp.txt", "database/index.txt");
                    
                    printf("Account deleted successfully!\n");
                    
                    sprintf(logMsg, "delete account - Account: %d", num);
                    logTransaction(logMsg);
                } else {
                    printf("Error updating index file!\n");
                    if(fp1) fclose(fp1);
                    if(fp2) fclose(fp2);
                }
            } else {
                printf("Cancelled.\n");
            }
            free(acc);
            return;
        }
        if(i < 2)
            printf("Wrong PIN! %d tries left.\n", 2-i);
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
    
    if(!listAllAccountsAndSelect(&num)) {
        return;
    }
    
    acc = getAccount(num);
    if(acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }
    
    // Refuse deposits into closed accounts to maintain audit integrity
    if(acc->status == 1) {
        printf("Account closed!\n");
        free(acc);
        return;
    }
    
    for(i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%4s", pin);
        getchar();
        
        if(strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            
            while(1) {
                // Enforce numeric input, positive amount, and max limit
                printf("Deposit amount (Max RM50,000): RM");
                if(scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    while(getchar() != '\n');
                    continue;
                }
                getchar();
                
                if(amount <= 0) {
                    printf("Amount must be greater than RM0!\n");
                    continue;
                }
                
                if(amount > 50000) {
                    printf("Amount exceeds maximum limit of RM50,000!\n");
                    continue;
                }
                
                break;
            }
            
            // At this point validation passed, so we can safely credit the funds
            acc->balance += amount;
            
            if(!saveAccount(acc)) {
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
        if(i < 2)
            printf("Wrong PIN! %d tries left.\n", 2-i);
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
    
    if(!listAllAccountsAndSelect(&num)) {
        return;
    }
    
    acc = getAccount(num);
    if(acc->accountNumber == 0) {
        printf("Account not found!\n");
        free(acc);
        return;
    }
    
    // Withdrawal cannot continue once the account is marked closed
    if(acc->status == 1) {
        printf("Account closed!\n");
        free(acc);
        return;
    }
    
    for(i = 0; i < 3; i++) {
        printf("Enter PIN: ");
        scanf("%4s", pin);
        getchar();
        
        if(strcmp(acc->pin, pin) == 0) {
            displayAccount(acc);
            printf("Available balance: RM%.2f\n", acc->balance);
            
            while(1) {
                // Keep prompting until the requested amount is valid
                printf("Withdraw amount: RM");
                if(scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    while(getchar() != '\n');
                    continue;
                }
                getchar();
                
                if(amount <= 0) {
                    printf("Invalid amount! Must be greater than RM0.\n");
                    continue;
                }
                
                if(amount > acc->balance) {
                    printf("Insufficient funds! Available: RM%.2f\n", acc->balance);
                    continue;
                }
                
                break;
            }
            
            // Debit the balance only after confirming sufficient funds
            acc->balance -= amount;
            
            if(!saveAccount(acc)) {
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
        if(i < 2)
            printf("Wrong PIN! %d tries left.\n", 2-i);
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
    if(!listAllAccountsAndSelect(&sender)) {
        return;
    }
    
    printf("\n=== Select Receiver Account ===\n");
    if(!listAllAccountsAndSelect(&receiver)) {
        return;
    }
    
    if(sender == receiver) {
        // Prevent accidental self-transfers that would only consume fees
        printf("Sender and receiver must be different!\n");
        return;
    }
    
    acc1 = getAccount(sender);
    acc2 = getAccount(receiver);
    
    // Validate both endpoints exist on disk before moving any money
    if(acc1->accountNumber == 0) {
        printf("Sender account not found!\n");
        free(acc1);
        free(acc2);
        return;
    }
    
    if(acc2->accountNumber == 0) {
        printf("Receiver account not found!\n");
        free(acc1);
        free(acc2);
        return;
    }
    
    if(acc1->status == 1) {
        printf("Sender account is closed!\n");
        free(acc1);
        free(acc2);
        return;
    }
    
    if(acc2->status == 1) {
        printf("Receiver account is closed!\n");
        free(acc1);
        free(acc2);
        return;
    }
    
    for(i = 0; i < 3; i++) {
        // Sender must pass PIN check before funds can move
        printf("Enter sender PIN: ");
        scanf("%4s", pin);
        getchar();
        
        if(strcmp(acc1->pin, pin) == 0) {
            displayAccount(acc1);
            
            while(1) {
                // Validate amount and calculate any dynamic fees
                printf("\nEnter transfer amount: RM");
                if(scanf("%f", &amount) != 1) {
                    printf("Invalid input! Please enter a number.\n");
                    while(getchar() != '\n');
                    continue;
                }
                getchar();
                
                if(amount <= 0) {
                    printf("Invalid amount! Must be greater than RM0.\n");
                    continue;
                }
                
                if(strcmp(acc1->accountType, "Savings") == 0 && 
                   strcmp(acc2->accountType, "Current") == 0) {
                    // Savings → Current incurs 2% fee per business rules
                    fee = amount * 0.02;
                    printf("Remittance fee (2%%): RM%.2f\n", fee);
                } 
                else if(strcmp(acc1->accountType, "Current") == 0 && 
                        strcmp(acc2->accountType, "Savings") == 0) {
                    // Current → Savings incurs a slightly higher 3% fee
                    fee = amount * 0.03;
                    printf("Remittance fee (3%%): RM%.2f\n", fee);
                }
                else {
                    // Same-type transfers are free
                    printf("No remittance fee applied.\n");
                }
                
                if(acc1->balance < amount + fee) {
                    printf("Insufficient funds! Need: RM%.2f (including fee)\n", amount + fee);
                    printf("Available: RM%.2f\n", acc1->balance);
                    char retry;
                    printf("Try different amount? (y/n): ");
                    scanf(" %c", &retry);
                    getchar();
                    if(retry == 'y' || retry == 'Y') {
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
            
            if(!saveAccount(acc1) || !saveAccount(acc2)) {
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
            
            sprintf(logMsg, "remittance - From: %d to %d, Amount: RM%.2f, Fee: RM%.2f", 
                    sender, receiver, amount, fee);
            logTransaction(logMsg);
            
            free(acc1);
            free(acc2);
            return;
        }
        if(i < 2)
            printf("Wrong PIN! %d tries left.\n", 2-i);
    }
    printf("Max attempts exceeded.\n");
    free(acc1);
    free(acc2);
}

// User input to the right operation based on menu selection
void mainMenu() {
    char input[20];
    int c;
    
    while(1) {
        // Loop indefinitely until operator chooses to exit
        printf("\n+========================================+\n");
        printf("| 1. Deposit    | 4. Create  Account     |\n");
        printf("| 2. Withdraw   | 5. Delete  Account     |\n");
        printf("| 3. Remittance | 0. Exit  System        |\n");
        printf("+========================================+\n");
        printf("Please select (number or keyword): ");
        
        if(scanf("%s", input) != 1) {
            while((c = getchar()) != '\n' && c != EOF);
            printf("==============================================\n");    
            printf("Invalid input!\n");
            continue;
        }
        
        while((c = getchar()) != '\n' && c != EOF);
        
        // Convert to lowercase for case-insensitive comparison
        for(int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }
        
        if(strcmp(input, "1") == 0 || strcmp(input, "deposit") == 0)
            deposit();
        else if(strcmp(input, "2") == 0 || strcmp(input, "withdraw") == 0 || 
                strcmp(input, "withdrawal") == 0)
            withdraw();
        else if(strcmp(input, "3") == 0 || strcmp(input, "remittance") == 0 || 
                strcmp(input, "transfer") == 0)
            remittance();
        else if(strcmp(input, "4") == 0 || strcmp(input, "create") == 0 || 
                strcmp(input, "new") == 0)
            createAccount();
        else if(strcmp(input, "5") == 0 || strcmp(input, "delete") == 0 || 
                strcmp(input, "remove") == 0)
            deleteAccount();
        else if(strcmp(input, "0") == 0 || strcmp(input, "exit") == 0 || 
                strcmp(input, "quit") == 0) {
            printf("==============================================\n");
            printf("Thank you for using Banking System. Goodbye!\n");
            logTransaction("exit system");
            exit(0);
        }
        else {
            printf("==============================================\n");    
            printf("Invalid option! Please try again.\n");
        }
    }
}

// PrayForSuccess (º̩̩́⌣º̩̩̀ʃƪ)