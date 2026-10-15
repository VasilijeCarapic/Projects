#include "Account.h"

namespace bank{
    int Account::nextId_ = 1;

    std::ostream &operator<<(std::ostream &os, const Account &acc)
    {
        // TODO: insert return statement here
        os << "Account #" << acc.id_ << " (" << acc.owner_ << "):" << acc.balance_; 
        return os; 
    }

    Account::Account(std::string owner, Money initial)
    {
        this-> owner_ = owner; 
        this -> id_ = nextId_++; 
        this -> balance_ = initial; 
    }

    int Account::getId() const
    {
        return this->id_;
    }

    std::string Account::getOwner() const
    {
        return this->owner_;
    }

    Money Account::getBalance() const
    {
        return this->balance_;
    }

    void Account::deposit(const Money &m)
    {
        this -> balance_ += m;  
    }

    std::string Account::withdraw(const Money &m)
    {
        if (m > balance_) return "not withdrawn"; 
        balance_ -= m; 
        return "withdrawn succesfully"; 
    }

    int Account::totalCreated()
    {
        return nextId_ - 1;
    }
}
