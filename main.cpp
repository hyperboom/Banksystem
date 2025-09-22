#include <iostream>
#include <string>

#include "utils/Database.hpp"
#include "utils/Bank.hpp"
#include "utils/BankAccount.hpp"
#include "utils/Transactions.hpp"

int main() {

    /*
    try {

        BankAccount bankaccount(1, "1234");
        BankAccount bankaccount2(2, "4321");


        bankaccount.deposit(10000);
        std::cout << "Deposit" << std::endl;

        bankaccount.withdraw(100);
        std::cout << "Withdraw" << std::endl;

        bankaccount.transferTo(100, bankaccount2);
        std::cout << "Transfer" << std::endl;;

        std::cout << "Balance: " << bankaccount.getBalance() << std::endl;

        std::cout << "AccountNumber: " << bankaccount.getAccountNumber() << std::endl;

        std::cout << "AccountHolder: " << bankaccount.getAccountHolder() << std::endl;

        bankaccount.applyInterest(5.0);
        std::cout << "Interest" << std::endl;

        bankaccount.lockAccount();
        std::cout << "LockAccount" << std::endl;

        bankaccount.unlockAccount("1234");
        std::cout << "UnlockAccount" << std::endl;

        std::cout << "IsAccountLocked: " << bankaccount.isAccountLocked() << std::endl;

        std::cout << "OverdraftLimit: " << bankaccount.getOverdraftLimit() << std::endl;

        std::cout << "CreditLimit: " << bankaccount.getCreditLimit() << std::endl;

        std::cout << "AccountType: " << bankaccount.getAccountType() << std::endl;

        bankaccount.~BankAccount();
        bankaccount2.~BankAccount(); 
    } 
    catch (const std::exception& e) {
        std::cerr << "Ein Fehler ist aufgetreten: " << e.what() << std::endl;
    } 
    */

    //Transaction transaction(1);

    Bank bank(1);

    //bank.createAccount("Martina Dönicke", "12345", 4000, 1000, 500, "Savings");
    //BankAccount account = bank.getAccount(1);
    //std::cout << account.getAccountHolder() << std::endl;

    std::cout << bank.getAccount(1).getBalance() << std::endl;
    BankAccount b1 = bank.getAccount(1);
    double balance = b1.getBalance();

    bank.~Bank();
    return 0;
}
