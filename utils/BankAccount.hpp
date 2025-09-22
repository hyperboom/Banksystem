#ifndef BANKACCOUNT_HPP
#define BANKACCOUNT_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <optional>

#include "./Bank.hpp"
#include "./Database.hpp"
#include "./Transactions.hpp"

class BankAccount {

    int accountNumber;
    double balance;
    std::string accountHolder;
    double overdraftLimit;
    double creditLimit;
    bool isLocked;
    
    time_t zeit;
    std::string transactionDate;

    std::string sqlQuery;
    std::vector<std::vector<std::string>> data;

    Database *db;

public:

    BankAccount(int accountID, const std::string& password) : db(new Database("./utils/data/data.db")) {
        std::setprecision(2);

        sqlQuery = "SELECT * FROM Account WHERE account_ID=" + std::to_string(accountID) + ";";
        data = db->executeQuery(sqlQuery.c_str());
        isLocked = (data[0][6] != "0.0");
        if (isLocked == std::optional<bool>()) {
            std::cout << "Fehler beim Laden von isLocked" << std::endl;
        }

        if (!data.empty() && !data[0].empty()) {
            //std::cout << "Datenbankantwort: " << data[0][0] << std::endl;

            if (data[0][2] == password) {
                
                if (isLocked == true) {
                    sqlQuery = "UPDATE Account SET is_locked=" + std::to_string(false) + " WHERE account_ID=" + std::to_string(accountID) + ";";
                    data = db->executeQuery(sqlQuery.c_str());

                    // Unlock Account
                    if (isLocked) {
                        if (data[0][2] == password) {

                        sqlQuery = "UPDATE Account SET is_locked=" + std::to_string(false) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
                        data = db->executeQuery(sqlQuery.c_str());

                        isLocked = false;
                        }
                        else {
                            std::cout << "Password ist falsch!" << std::endl;
                        }
                    } 

                    if (isLocked == false) {
                        std::cout << "Account ist entspert" << std::endl;
                    }
                    
                }

                //std::cout << "Account entsperrt!" << std::endl;

                accountNumber = accountID;

                balance = std::stod(data[0][3]);
                if (balance == std::optional<double>()) {
                    std::cout << "Fehler beim Laden von Balance" << std::endl;
                }

                accountHolder = data[0][1];
                if (accountHolder == std::optional<std::string>()) {
                    std::cout << "Fehler beim Laden von accountHolder" << std::endl;
                }

                overdraftLimit = std::stod(data[0][4]);
                if (overdraftLimit == std::optional<double>()) {
                    std::cout << "Fehler beim Laden von overdraftLimit" << std::endl;
                }

                creditLimit = std::stod(data[0][5]);
                if (creditLimit == std::optional<double>()) {
                    std::cout << "Fehler beim Laden von creditLimit" << std::endl;
                }

                zeit = time(nullptr);

            } else {
                std::cout << "ID oder Passwort falsch!" << std::endl;
            }
        } else {
            std::cerr << "Keine Daten gefunden oder Datenbankfehler!" << std::endl;
        } 
    }

    void deposit(double amount) {
        if (isLocked == false) {
            zeit = time(nullptr);
            transactionDate = std::asctime(std::localtime(&zeit));

            balance = balance + amount;

            sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
            db->excCommand(sqlQuery.c_str());

            sqlQuery = "INSERT INTO \"Transaction\" (account_id_from, account_id_to, amount, transaction_type, status, transaction_date) VALUES (" 
                    + std::to_string(this->accountNumber) + ", "
                    + std::to_string(this->accountNumber) + ", "
                    + std::to_string(amount) + ", "
                    + "'deposit', " 
                    + "'completed'" + ", "
                    + + "'" + transactionDate + "'"  
                    + ");";
            db->excCommand(sqlQuery.c_str());
        } else {
            std::cout << "Account ist gesperrt!" << std::endl;
        }
    }

    void withdraw(double amount) {
        if (isLocked == false) {
            zeit = time(nullptr);
            transactionDate = std::asctime(std::localtime(&zeit));

            if ((balance + overdraftLimit) > amount) {

                sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance - amount) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
                db->excCommand(sqlQuery.c_str());
        
                sqlQuery = "INSERT INTO \"Transaction\" (account_id_to, account_id_from, amount, transaction_type, status, transaction_date) VALUES (" 
                    + std::to_string(this->accountNumber) + ", "
                    + std::to_string(this->accountNumber) + ", "
                    + std::to_string(amount) + ", "
                    + "'withdraw', " 
                    + "'completed'" + ", "
                    + + "'" + transactionDate + "'"  
                    + ");";
                db->excCommand(sqlQuery.c_str());
                std::cout << "Es wurden " + std::to_string(amount) + " von deinem Konto abgebucht!" << std::endl;

            } else {
                std::cout << "Du darfst nicht so viel Geld abheben!" << std::endl; 

            }
        } else {
            std::cout << "Account ist gesperrt!" << std::endl;
        }
       
    }

    void transferTo(double amount, BankAccount targetAccount) {
        if (isLocked == false) {
            zeit = time(nullptr);
            transactionDate = std::asctime(std::localtime(&zeit));

            if ((balance + overdraftLimit) > amount) {
                
                //Sender
                sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance - amount) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
                db->excCommand(sqlQuery.c_str());

                // Ziel Account
                balance = targetAccount.getBalance() + amount;
                sqlQuery = "UPDATE Account SET balance=" + std::to_string(balance) + " WHERE account_ID=" + std::to_string(targetAccount.getAccountNumber()) + ";";
                db->excCommand(sqlQuery.c_str());

                // Transaction Save
                sqlQuery = "INSERT INTO \"Transaction\" (account_id_from, account_id_to, amount, transaction_type, status, transaction_date) VALUES (" 
                    + std::to_string(this->accountNumber) + ", "
                    + std::to_string(targetAccount.getAccountNumber()) + ", "
                    + std::to_string(amount) + ", "
                    + "'transfer', " 
                    + "'completed'" + ", "
                    + "'" + transactionDate + "'"  
                    + ");";
                db->excCommand(sqlQuery.c_str());

                balance = this->getBalance();

            } else {
                std::cout << "Du darfst nicht so viel Geld überweisen!" << std::endl; 

            }
        } else {
            std::cout << "Account ist gesperrt!" << std::endl;
        }
    }

    double getBalance() {
        try {
            if (isLocked == false) {
                sqlQuery = "SELECT * FROM \"Account\" WHERE account_ID='" + std::to_string(this->accountNumber) + "';";
                data = db->executeQuery(sqlQuery);

                if (data.empty() || data[0].empty()) {

                    for (int i = 0; i < data.size(); i ++) {
                        for (int j = 0; j < data[i].size(); j++) {
                            std::cout << data[i][j];
                        }
                    }

                    throw std::runtime_error("Fehler: Keine gueltigen Daten fuer den Kontostand.");
                } else {

                    balance = std::stod(data[0][3]);

                    if (!this->isLocked) {
                        return balance;
                    } else {
                        throw std::runtime_error("Account ist gesperrt");
                    }
                }
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getBalance: " << e.what() << std::endl;
            return 0.0;  
        }
    }

    int getAccountNumber() {
        try {
            if (isLocked == false) {
                return this->accountNumber;
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getAccountNumber: " << e.what() << std::endl;
            return -1;  
        }
    }

    std::string getAccountHolder() {
        try {
            if (isLocked == false) {
                return this->accountHolder;
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getAccountHolder: " << e.what() << std::endl;
            return "";  
        }
    }

    void applyInterest(double rate) {
        if (isLocked == false) {
            zeit = time(nullptr);
            transactionDate = std::asctime(std::localtime(&zeit));

            if (rate > 100) {
                std::cout << "Der Zinssatz darf nicht höher als 100% liegen" << std::endl;

            } else {


                sqlQuery = "SELECT interest_id FROM Interest WHERE account_ID=" + 
                    std::to_string(accountNumber) + " AND end_date=NULL;";
                data = db->executeQuery(sqlQuery);
                if (data[0][0] != std::optional<std::string>()) {
                    sqlQuery = "UPDATE Interest SET end_date=" + transactionDate 
                    + " WHERE interest_id=" + data[0][0];
                    + ";";
                    db->executeQuery(sqlQuery);
                }

                balance += (balance * rate / 100);

                sqlQuery = "INSERT INTO \"Interest\"" 
                    "(interest_rate, start_date, account_type_id)"
                    "VALUES ("
                    "'" + std::to_string(rate) + "', "
                    "'" + transactionDate + "', "
                    "(SELECT account_type_id FROM Account WHERE account_ID="+ std::to_string(accountNumber) +")"  
                    ")"
                    ";";
                db->executeQuery(sqlQuery);

            }
        } else {
            std::cout << "Account ist gesperrt!" << std::endl;
        }
    }

    void lockAccount() {
        sqlQuery = "UPDATE Account set is_locked=" + std::to_string(true) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
        data = db->executeQuery(sqlQuery.c_str());

        sqlQuery = "SELECT is_locked FROM Account WHERE account_ID=" + std::to_string(accountNumber) + ";";
        data = db->executeQuery(sqlQuery.c_str());
        isLocked = (data[0][0] != "0.0");

        if (isLocked) {
            std::cout << "Account gesperrt!" << std::endl;
        } else {
            std::cout << "Account konnte nicht gesperrt werden!" << std::endl;
        }
    }

    void unlockAccount(std::string password) {

        sqlQuery = "SELECT password FROM Account WHERE account_ID=" + std::to_string(accountNumber) + ";";

        data = db->executeQuery(sqlQuery.c_str());

        if (isLocked) {
            if (data[0][0] == password) {

            sqlQuery = "UPDATE Account set is_locked=" + std::to_string(false) + " WHERE account_ID=" + std::to_string(accountNumber) + ";";
            data = db->executeQuery(sqlQuery.c_str());

            isLocked = false;
            }
            else {
                std::cout << "Password ist falsch!" << std::endl;
            }
        } 

        if (isLocked == false) {
            std::cout << "Account ist entspert" << std::endl;
        }
    }

    bool isAccountLocked() {
        return isLocked;
    }

    double getOverdraftLimit() {
        try {
            if (isLocked == false) {
                return overdraftLimit;
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getOverdraftLimit: " << e.what() << std::endl;
            return 0.0;  
        }
    }

    double getCreditLimit() {
        try {
            if (isLocked == false) {
                return creditLimit;
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getCreditLimit: " << e.what() << std::endl;
            return 0.0;  
        }
    }

    std::string getAccountType() {
        try {
            if (isLocked == false) {
                sqlQuery = "SELECT account_type_name FROM AccountType JOIN Account "
                        "ON AccountType.account_type_id=Account.account_type_id "
                        "WHERE Account.account_ID=" + std::to_string(accountNumber) + ";";

                data = db->executeQuery(sqlQuery.c_str());

                if (data.empty() || data[0].empty()) {
                    throw std::runtime_error("Fehler: Keine gültigen Daten für den Kontotyp.");
                }

                return data[0][0];
            } else {
                throw std::runtime_error("Account ist gesperrt!");
            }
        } catch (const std::exception& e) {
            std::cerr << "Fehler in getAccountType: " << e.what() << std::endl;
            return "";  
        }
    }

    ~BankAccount() {
        delete db;
    }

};

#endif