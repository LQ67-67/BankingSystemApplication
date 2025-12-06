# Banking Management System

A comprehensive banking system written in C that provides account management, transactions, and audit logging capabilities.

## Features

- **Account Management**: Create and delete bank accounts with unique account numbers
- **Secure Transactions**: Deposit, withdraw, and transfer money between accounts
- **PIN Protection**: 4-digit PIN authentication for all transactions
- **Account Types**: Support for Savings and Current accounts
- **Transaction Fees**: Automatic fee calculation for transfers between different account types
- **Audit Logging**: Complete transaction history tracking
- **Data Persistence**: File-based storage system for account data

## System Requirements

- C compiler (GCC recommended)
- Operating System: Windows, Linux, or macOS

## Installation

1. Clone or download the project files
2. Navigate to the project directory
3. Compile the program:
   ```bash
   gcc BankSystem.c -o BankSystem
   ```

## Usage

Run the compiled program:
```bash
./BankSystem
```

The system will create a `database` directory automatically to store account information and transaction logs.

## Account Operations

### Create Account
- Generates unique 7-9 digit account numbers
- Requires account holder name, ID number, account type, and 4-digit PIN
- Supports Savings and Current account types

### Deposit
- Add funds to active accounts
- Maximum deposit limit: RM50,000
- PIN authentication required

### Withdraw
- Withdraw funds from active accounts
- Prevents overdrafts
- PIN authentication required

### Remittance (Transfer)
- Transfer funds between accounts
- Fees apply for transfers between different account types:
  - Savings → Current: 2% fee
  - Current → Savings: 3% fee
  - Same type transfers: No fee

### Delete Account
- Remove accounts from the system
- Requires ID verification and PIN authentication
- Warns if account has remaining balance

## Data Storage

The system uses a file-based storage structure:
- `database/`: Main storage directory
- `database/index.txt`: Index of all account numbers
- `database/[account_number].txt`: Individual account files
- `database/transaction.log`: Complete audit trail of all transactions

## Security Features

- PIN authentication for all transactions
- ID verification for account deletion
- Maximum 3 attempts for PIN entry
- Transaction logging for audit purposes
- Account status tracking (Active/Closed)

## Sample Output

██████   █████  ███    ██ ██   ██ ██ ███    ██  ██████      ███████ ██    ██ ███████ ████████ ███████ ███    ███ 
██   ██ ██   ██ ████   ██ ██  ██  ██ ████   ██ ██           ██       ██  ██  ██         ██    ██      ████  ████ 
██████  ███████ ██ ██  ██ █████   ██ ██ ██  ██ ██   ███     ███████   ████   ███████    ██    █████   ██ ████ ██ 
██   ██ ██   ██ ██  ██ ██ ██  ██  ██ ██  ██ ██ ██    ██          ██    ██         ██    ██    ██      ██  ██  ██ 
██████  ██   ██ ██   ████ ██   ██ ██ ██   ████  ██████      ███████    ██    ███████    ██    ███████ ██      ██ 

                                                                                                  created by Skim
+==============================================+
  Banking Management System - Session Info
+==============================================+
  Session Time: Sat Dec  6 16:22:54 2025
  Total Accounts: 3
+==============================================+

+========================================+
| 1. Deposit    | 4. Create  Account     |
| 2. Withdraw   | 5. Delete  Account     |
| 3. Remittance | 0. Exit  System        |
+========================================+
Please select (number or keyword): 1

+==================================================================+
| No | Account No | Name       | Balance    | Type     | Status   |
+----+------------+------------+------------+----------+----------+
|  1 |   28165204 |67          |    8434.00 |Savings   |Active    |
|  2 |   88908888 |skim        |   22257.00 |Savings   |Active    |
+==================================================================+

Enter account number (1-2) or 0 to enter account number directly: 1
Enter PIN: 1234

+------------------------------------------------------------------+
| Account No | Name      | PIN  | Balance    | Type     | Status   |
|   28165204 |        67 | 1234 |    8434.00 | Savings  |  Active  |
+------------------------------------------------------------------+
Deposit amount (Max RM50,000): RM123

+------------------------------------------------------------------+
| Account No | Name      | PIN  | Balance    | Type     | Status   |
|   28165204 |        67 | 1234 |    8557.00 | Savings  |  Active  |
+------------------------------------------------------------------+
Deposit successful!

+========================================+
| 1. Deposit    | 4. Create  Account     |
| 2. Withdraw   | 5. Delete  Account     |
| 3. Remittance | 0. Exit  System        |
+========================================+
Please select (number or keyword): 0
==============================================
Thank you for using Banking System. Goodbye!

## Author

Created by Skim

## License

This project is for educational purposes as part of COMP1312 Programming 1 coursework.
