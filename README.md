# Banking Management System

A comprehensive command-line banking system written in C, featuring secure account management, transaction processing, fee calculation, and audit logging.

## Features

* **Account Management** – Create and delete accounts with unique auto-generated account numbers
* **Secure Transactions** – Deposit, withdraw, and transfer funds safely
* **PIN Protection** – 4-digit PIN verification for every transaction
* **Account Types** – Supports *Savings* and *Current* accounts
* **Transaction Fees** – Automatic fee calculation for cross-type transfers
* **Audit Logging** – Full record of all operations and transactions
* **Data Persistence** – File-based database for all account and log data

## System Requirements

* C compiler (GCC recommended)
* OS: Windows, Linux, or macOS

## Installation

1. Clone or download this repository
2. Navigate into the project directory
3. Compile the program:

   ```bash
   gcc BankSystem.c -o BankSystem
   ```

## Usage

Run the executable:

```bash
./BankSystem
```

A `database` directory will automatically be created to store account files and transaction logs.

## Account Operations

### ➤ Create Account

* Generates a unique 7–9 digit account number
* Requires: Name, ID number, Account type, 4-digit PIN
* Supports *Savings* and *Current* accounts

### ➤ Deposit

* Allows deposits up to **RM50,000**
* Requires PIN authentication

### ➤ Withdraw

* Prevents negative balance
* Requires PIN authentication

### ➤ Remittance (Transfer)

* Transfer between any two accounts
* Fee rules:

  * Savings → Current: **2% fee**
  * Current → Savings: **3% fee**
  * Same type: **No fee**

### ➤ Delete Account

* Requires ID and PIN verification
* Warns the user if balance remains

## Data Storage Structure

```
database/
│── index.txt                # List of all account numbers
│── transaction.log          # Full audit trail
└── [account_number].txt     # Individual account files
```

## Security Features

* 4-digit PIN authentication
* ID verification for account deletion
* Max 3 PIN attempts
* Account status tracking (Active / Closed)
* Complete logging for auditing and debugging

## Sample Output

```
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
```

## Author

Created by **Skim**

## License

This project is created for educational purposes as part of **COMP1312 Programming 1** coursework.
