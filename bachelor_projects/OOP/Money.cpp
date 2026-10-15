#include "Money.h"
#include <iostream>
#include <iomanip>

namespace bank{
    void Money::setBalance(long long balance)
    {
        this -> balance_ = balance; 
    }

    long long Money::getBalance() const 
    {
        return this -> balance_;
    }

    Money &Money::operator+=(const Money &other)
    {
        // TODO: insert return statement here
        this->balance_ += other.getBalance(); 

        return* this; 
    }

    Money &Money::operator-=(const Money &other)
    {
        // TODO: insert return statement here
        this -> balance_ -= other.getBalance(); 

        return *this; 
    }

    Money operator+(const Money &m1, const Money &m2)
    {
        long long res =  m1.balance_ + m2.balance_; 

        return Money(res);
    }

    Money operator-(const Money &m1, const Money &m2)
    {   
        long long res = m1.balance_ - m2.balance_; 
        
        return Money(res); // vraca se novi objekat 
    }

    bool operator==(const Money &m1, const Money &m2)
    {
        return (m1.balance_ == m2.balance_);
    }

    bool operator!=(const Money &m1, const Money &m2)
    {
        return (m1.balance_ != m2.balance_);
    }

    bool operator>(const Money &m1, const Money &m2)
    {
        return (m1.balance_ > m2.balance_);
    }

    bool operator<(const Money &m1, const Money &m2)
    {
        return (m1.balance_ < m2.balance_);
    }

    bool operator<=(const Money &m1, const Money &m2)
    {
        return (m1.balance_ <= m2.balance_);
    }

    bool operator>=(const Money &m1, const Money &m2)
    {
        return (m1.balance_ >= m2.balance_);
    }

    std::ostream &operator<<(std::ostream &os, const Money &m)
    {
        long long abs = m.getBalance() < 0 ? -m.getBalance() : m.getBalance();
        if (m.getBalance() < 0) os << '-';
        auto oldFill = os.fill('0');          
        os << abs / 100 << '.' << std::setw(2) << abs % 100;
        os.fill(oldFill);                     
        return os;
    }
}