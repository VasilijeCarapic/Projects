#ifndef _MONEY_H 
#define _MONEY_H 

#include <ostream>
#include <string>

namespace bank{
    class Money{
        public: 
            Money() = default; 

            explicit Money(long long cents): balance_(cents) {}; 
            Money(long long euros, long long cents): balance_(euros * 100 + cents){};

            ~Money() = default;

            void setBalance(long long balance); 
            long long getBalance() const; 


            // (return type) operator operation (parameters)
            friend Money operator+(const Money& m1,const Money& m2);
            friend Money operator-(const Money& m1,const Money& m2);

            Money& operator+=(const Money& other); 
            Money& operator-=(const Money& other); 

            friend bool operator==(const Money& m1, const Money& m2);
            friend bool operator!=(const Money& m1, const Money& m2);
            friend bool operator>(const Money& m1, const Money& m2);
            friend bool operator<(const Money& m1, const Money& m2);
            friend bool operator<=(const Money& m1, const Money& m2);
            friend bool operator>=(const Money& m1, const Money& m2);

            friend std::ostream& operator<<(std::ostream& os, const Money& m);

        private: 
            long long balance_ = 0; 

    };
}
#endif /* bank */