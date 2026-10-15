#ifndef ACCOUNT_H 
#define ACCOUNT_H 

#include <string> 
#include <ostream> 
#include "Money.h"


namespace bank{
    class Account{
        public: 
            Account(std::string owner, Money initial = Money());

            int getId() const; 
            std::string getOwner() const; 
            Money getBalance() const; 

            void deposit(const Money&);
            std::string withdraw(const Money&);
            static int totalCreated();
            
            friend std::ostream& operator<<(std::ostream&, const Account& acc); 

        private:
            static int nextId_;      // jedan za sve račune: koji ID je sledeći
            int id_;                 // svoj za svaki račun
            std::string owner_; 
            Money balance_; 
    };
}

#endif 