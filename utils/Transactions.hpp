#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <optional>

#include "./Bank.hpp"
#include "./Database.hpp"
#include "./BankAccount.hpp"

class Transaction {

    int transactionID;
    double amount;
    std::string transactionType;
    int sourceAccount;
    int targetAccount;
    std::string date;
    std::string status;

    std::string sqlQuery;
    std::vector<std::vector<std::string>> data;
    Database* db;

public:

    Transaction(int pTranscationID) : db(new Database("./utils/data/data.db")) {
        transactionID = pTranscationID;

        sqlQuery = "SELECT * FROM \"Transaction\" WHERE transaction_id=" + std::to_string(transactionID) + ";";
        data = db->executeQuery(sqlQuery);

        if (!data.empty() && !data[0].empty()) {
            transactionID = std::stoi(data[0][0]);
            sourceAccount = std::stoi(data[0][1]);
            targetAccount = std::stoi(data[0][2]);
            amount = std::stod(data[0][3]);
            transactionType =  data[0][4];
            status = data[0][5];
            date = data[0][6];

        } else {
            std::cout << "Fehler beim finden einer Transaction!" << std::endl;
        }
    }

    int getTransactionID() {
        return transactionID;
    }

    double getAmount() {
        return amount;
    }

    std::string getTransactionType() {
        return transactionType;
    }

    std::string getDate() {
        return date;
    }

    int getSourceAccount() {
        return sourceAccount;
    }

    int getTargetAccount() {
        return targetAccount;
    }

    std::string getStatus() {
        return status;
    }

    ~Transaction() {
        db->~Database();
    }
};

#endif