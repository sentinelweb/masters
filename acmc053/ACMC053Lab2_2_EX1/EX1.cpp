/*
 * EX1.cpp
 *
 *  Created on: 10-Oct-2009
 *      Author: robm
 */
using namespace std;
#include "EX1.h"
#include "Bank.h"
#include "Account.h"

EX1::EX1() {
	// TODO Auto-generated constructor stub

}

EX1::~EX1() {
	// TODO Auto-generated destructor stub
}
int main() {
	Bank *b = new Bank();
	b->newAccount("Current",1000);
	b->newAccount("Savings",10000);
	/*
	b->accounts[0].withdraw(222);
	b->accounts[0].deposit(444);
	b->accounts[1].withdraw(444);
	b->accounts[1].deposit(222);
	*/
	b->accounts.at(0).withdraw(222);
	b->accounts.at(0).deposit(444);
	b->accounts.at(1).withdraw(444);
	b->accounts.at(1).deposit(222);

	b->printAccounts();
}
