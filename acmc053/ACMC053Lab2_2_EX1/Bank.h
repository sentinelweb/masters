/*
 * Bank.h
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */
using namespace std;
#ifndef BANK_H_
#define BANK_H_

#include <vector>
#include "Account.h"

class Bank {
public:
	vector<Account> accounts;
	void newAccount(string name, float initialBalance);
	void printAccounts();
	Bank();
	virtual ~Bank();
};

#endif /* BANK_H_ */
