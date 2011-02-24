/*
 * Bank.cpp
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */

#include "Bank.h"

Bank::Bank() {
	// TODO Auto-generated constructor stub
	//this->accounts = new vector<Account>();
}

void Bank::printAccounts(){
	for (int i=0;i<this->accounts.size();i++) {
		Account a = this->accounts.at(i);
		a.print();
	}
}

void Bank::newAccount(string name, float initialBalance){
	Account *acc = new Account(name,initialBalance);
	this->accounts.push_back(*acc);
}

Bank::~Bank() {
	// TODO Auto-generated destructor stub

}
