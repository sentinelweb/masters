/*
 * Account.h
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */
using namespace std;
#ifndef ACCOUNT_H_
#define ACCOUNT_H_


#include <string>
#include <vector>

class Account {
public:
	string name;
	float balance;

    string getName() const
    {
        return name;
    }

    void setName(string name)
    {
        this->name = name;
    }

    float getBalance() const
    {
        return balance;
    }

    void setBalance(float balance)
    {
        this->balance = balance;
    }

    Account();
    Account(string name, float initialBalance);
    void deposit(float amt);
    void withdraw(float amt);
    void print();
	virtual ~Account();
};

#endif /* ACCOUNT_H_ */
