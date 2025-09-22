#ifndef BANK_HPP
#define BANK_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <optional>
#include <vector>
#include <algorithm>

#include "./BankAccount.hpp"
#include "./Database.hpp"
#include "./Transactions.hpp"

class Bank {

    int bankID;
    std::string created_at;

    std::vector<BankAccount> accounts;
    std::string bankName;
    std::vector<Transaction> transaction;

    std::string sqlQuery;
    std::vector<std::vector<std::string>> data;
    Database* db;

    time_t zeit;
    std::string date;

public:

    Bank (int pBankID) : db(new Database("./utils/data/data.db")) {
        sqlQuery = "SELECT * FROM \"Bank\" WHERE bank_id=" + std::to_string(pBankID) + ";";
        data = db->executeQuery(sqlQuery.c_str());

        bankID = std::stoi(data[0][0]);
        bankName = data[0][1];
        created_at = data[0][2];
    }

    Bank (std::string pBankName) : db(new Database("./utils/data/data.db")) {
        
        sqlQuery = "SELECT * FROM Bank WHERE bank_name='" + pBankName + "';";
        data = db->executeQuery(sqlQuery.c_str());

        if (!data.empty() && !data[0].empty()) {
            std::cerr << "Bank mit diesem Namen existiert bereits -> mit der ID: " + data[0][0] << std::endl;

            bankID = std::stoi(data[0][0]);
            bankName = data[0][1];
            created_at = data[0][2];

        } else {
            zeit = time(nullptr);
            date = std::asctime(std::localtime(&zeit));
            date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

            try {
                sqlQuery = "INSERT INTO \"Bank\" (bank_name, created_at) VALUES ('" + pBankName + "', '" + date + "');";
                db->executeQuery(sqlQuery.c_str());
                std::cout << "Bank erstellt" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Fehler beim Erstellen der Bank: " << e.what() << std::endl;
                return;
            }
        }
    }

    BankAccount createAccount(std::string accountHolder, std::string password, double initialBalance, double overdraft_limit, double credit_limit,  std::string accountType) {
        if (accountType == "Checking") {
            accountType = "1";
        } else if (accountType == "Savings") {
            accountType = "2";
        } else if (accountType == "Loan") {
            accountType = "3";
        } else {
            std::cerr << "Gib einen gültigen Account Typen an (Checking; Saving; Loan)" << std::endl;
        }

        sqlQuery = "SELECT * FROM Account WHERE account_holder='" + accountHolder + "' AND bank_id=" + std::to_string(bankID) + ";";
        data = db->executeQuery(sqlQuery.c_str());

        if (!data.empty() && !data[0].empty()) {
            std::cerr << "Account mit diesem Namen existiert bereits -> mit der ID: " + data[0][0] << std::endl;
            BankAccount bankAccount(-1, " ");
            return bankAccount;
        } else {
            zeit = time(nullptr);
            date = std::asctime(std::localtime(&zeit));
            date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

            try {
                sqlQuery = "INSERT INTO Account ("
                    "account_holder, password, balance, "
                    "overdraft_limit, credit_limit, "
                    "is_locked, created_at, "
                    "account_type_id, bank_id) "
                "VALUES ('"
                    + accountHolder + "', '"
                    + password + "', '"
                    + std::to_string(initialBalance) + "', '"
                    + std::to_string(overdraft_limit) + "', '"
                    + std::to_string(credit_limit) + "', '"
                    + std::to_string(true) + "', '"
                    + date + "', '"
                    + accountType + "', '"
                    + std::to_string(bankID) +
                "');";
                db->executeQuery(sqlQuery.c_str());


                sqlQuery = "SELECT * FROM Account WHERE date=" + date + ";";
                data = db->executeQuery(sqlQuery.c_str());
                BankAccount bankAccount(std::stoi(data[0][0]), password);
                return bankAccount;
                
            } catch (const std::exception& e) {
                std::cerr << "Fehler beim Erstellen des Accounts: " << e.what() << std::endl;
                BankAccount bankAccount(-1, " ");
                return bankAccount;
            }
        }
    }

    BankAccount getAccount(int accountNumber) {

        sqlQuery = "SELECT * FROM \"Account\" WHERE account_ID=" + std::to_string(accountNumber) + " AND bank_id=" + std::to_string(bankID) + ";";
        data = db->executeQuery(sqlQuery.c_str());

        if (!data.empty() && !data[0].empty()) {            
            
            try {
                BankAccount bankAccount(accountNumber, data[0][2]);
                return bankAccount;
            } catch (const std::exception& e) {
                std::cerr << "Fehler beim finden des Accounts: " << e.what() << std::endl;
                BankAccount bankAccount(-1, " ");
                return bankAccount;
            }

        } else {
            std::cerr << "Account konnte bei dieser Bank nicht gefunden werden!" << std::endl;
            BankAccount bankAccount(-1, " ");
            return bankAccount;  
        }
    }

    bool depositToAccount(int accountNumber, double amount) {
        try {

            if (amount <= 0) {
                std::cerr << "Der Betrag muss größer 0 sein!" << std::endl;
                return false;
            }

            zeit = time(nullptr);
            date = std::asctime(std::localtime(&zeit));

            sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(accountNumber) + " AND bank_id=" + std::to_string(bankID) + ";";
            data = db->executeQuery(sqlQuery.c_str());

            if (!data.empty() && !data[0].empty()) {
                int balance = std::stoi(data[0][3]);
                balance += amount;

                sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance) + " WHERE account_ID=" + std::to_string(accountNumber) + " AND bank_id=" + std::to_string(bankID) + ";";
                db->excCommand(sqlQuery.c_str());

                sqlQuery = "INSERT INTO \"Transaction\" (account_id_from, account_id_to, amount, transaction_type, status, transaction_date) VALUES (" 
                    + std::to_string(accountNumber) + ", "
                    + std::to_string(accountNumber) + ", "
                    + std::to_string(amount) + ", "
                    + "'deposit', " 
                    + "'completed'" + ", "
                    + + "'" + date + "'"  
                    + ");";
                db->excCommand(sqlQuery.c_str());

                return true;
            } else {
                std::cerr << "Account konnte bei dieser Bank nicht gefunden werden!" << std::endl;
                return false;
            }

            return true;
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim Geld aufladen zu Account mit der ID: " << accountNumber << std::endl;
            return false; 
        }
    }

    bool withdrawFromAccount(int accountNumber, double amount) {
        try {

            if (amount <= 0) {
                std::cerr << "Der Betrag muss größer 0 sein!" << std::endl;
                return false;
            }

            zeit = time(nullptr);
            date = std::asctime(std::localtime(&zeit));

            sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(accountNumber) + " AND bank_id=" + std::to_string(bankID) + ";";
            data = db->executeQuery(sqlQuery.c_str());

            if (!data.empty() && !data[0].empty()) {
                int balance = std::stoi(data[0][3]);
                
                if (amount > std::stod(data[0][4])) {
                    balance -= amount;
                } else {
                    return false;
                }

                sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance) + " WHERE account_ID=" + std::to_string(accountNumber) + " AND bank_id=" + std::to_string(bankID) + ";";
                db->excCommand(sqlQuery.c_str());

                sqlQuery = "INSERT INTO \"Transaction\" (account_id_from, account_id_to, amount, transaction_type, status, transaction_date) VALUES (" 
                    + std::to_string(accountNumber) + ", "
                    + std::to_string(accountNumber) + ", "
                    + std::to_string(amount) + ", "
                    + "'withdraw', " 
                    + "'completed'" + ", "
                    + + "'" + date + "'"  
                    + ");";
                db->excCommand(sqlQuery.c_str());

                return true;
            } else {
                std::cerr << "Account konnte bei dieser Bank nicht gefunden werden!" << std::endl;
                return false;
            }

            return true;
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim Geld abheben zu Account mit der ID: " << accountNumber << std::endl;
            return false; 
        }
    }

    bool transferBetweenAccounts(int sourceAccountID, int targetAccountID, double amount) {
        try {

            if (amount <= 0) {
                std::cerr << "Der Betrag muss größer 0 sein!" << std::endl;
                return false;
            }

            zeit = time(nullptr);
            date = std::asctime(std::localtime(&zeit));
            
            sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(targetAccountID) + " AND bank_id=" + std::to_string(bankID) + ";";
            data = db->executeQuery(sqlQuery.c_str());

            if (!data.empty() && !data[0].empty()) {

                sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(sourceAccountID) + " AND bank_id=" + std::to_string(bankID) + ";";
                data = db->executeQuery(sqlQuery.c_str());

                if (!data.empty() && !data[0].empty()) {
                    int balance = std::stoi(data[0][3]);

                    if (amount < std::stod(data[0][3])) {
                        balance -= amount;
                    } else {
                        return false;
                    }

                    sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance) + " WHERE account_ID=" + std::to_string(sourceAccountID) + " AND bank_id=" + std::to_string(bankID) + ";";
                    db->excCommand(sqlQuery.c_str());

                    sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(targetAccountID) + " AND bank_id=" + std::to_string(bankID) + ";";
                    data = db->executeQuery(sqlQuery.c_str());

                    sqlQuery = "UPDATE Account SET balance=" + std::to_string((std::stod(data[0][3]) + amount)) + " WHERE account_ID=" + std::to_string(targetAccountID) + " AND bank_id=" + std::to_string(bankID) + ";";
                    db->excCommand(sqlQuery.c_str());

                    sqlQuery = "INSERT INTO \"Transaction\" (account_id_from, account_id_to, amount, transaction_type, status, transaction_date) VALUES (" 
                        + std::to_string(sourceAccountID) + ", "
                        + std::to_string(targetAccountID) + ", "
                        + std::to_string(amount) + ", "
                        + "'transfer', " 
                        + "'completed'" + ", "
                        + + "'" + date + "'"  
                        + ");";
                    db->excCommand(sqlQuery.c_str());

                    return true;
                } else {
                    std::cerr << "Start Account konnte nicht gefunden werden" << std::endl;
                    return false;
                }

                return true;
            } else {
                std::cerr << "Ziel Account konnte nicht gefunden werden!" << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler beim Geld überweisen zu Account mit der ID: " << targetAccountID << " Und der QuellID: " << sourceAccountID << std::endl;
            return false; 
        }
    }

    

    ~Bank() {
        db->~Database();
    }

};

#endif