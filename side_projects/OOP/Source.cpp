#include <iostream> 

#include "Money.h"
#include "Account.h"

int main(){
    bank::Account a("Marko", bank::Money(100, 0));
    bank::Account b("Ana");
    a.deposit(bank::Money(50));
    std::cout << a << "\n";                                // Account #1 (Marko): 100.50
    std::cout << b << "\n";                                // Account #2 (Ana): 0.00
    std::cout << a.withdraw(bank::Money(500, 0)) << "\n";  // not withdrawn
    std::cout << a.withdraw(bank::Money(100, 50)) << "\n"; // withdrawn succesfully (celo stanje, mora da prođe)
    std::cout << a << "\n";                                // Account #1 (Marko): 0.00
    std::cout << bank::Account::totalCreated() << "\n";    // 2
    
    return 0; 
}