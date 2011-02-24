/*
 * Account.cpp
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */

#include "Account.h"
#include <string>
#include <iostream>

Account::Account() {

}

Account::Account(string name, float initialBalance) {
	this->name=name;
	this->balance=initialBalance;
}
void Account::deposit(float amt){
	this->balance+=amt;
}

void Account::withdraw(float amt){
	this->balance-=amt;
}

void Account::print(){
	cout << this->name << ":" << this->balance << "\n" ;
}


Account::~Account() {
	// TODO Auto-generated destructor stub
}
