/*
 * Ex5.cpp
 *
 *  Created on: 11-Oct-2009
 *      Author: robm
 */
using namespace std;
#include <iostream>
#include <string>
#include <cstring>

#include "Ex5.h"

Ex5::Ex5() {
	// TODO Auto-generated constructor stub

}

Ex5::~Ex5() {
	// TODO Auto-generated destructor stub
}


void check(char *a, char *b, int (*cmp)(const char *, const char *));
int doop(int *a, int *b, int (*cmp)( int *,  int *));
int add(int *a, int *b){return (*a)+(*b);}
int sub(int *a, int *b){return (*a)-(*b);}
int mul(int *a, int *b){return (*a)*(*b);}
int div(int *a, int *b){return (*a)/(*b);}

int main() {
  char s1[80], s2[80];
  int (*p)(const char *, const char *);

 p = strcmp;

 cin >> s1;
 cin >> s2;

 check(s1, s2, p);
 int *a = new int(5);
 int *b = new int(6);

 int (*o)( int *,  int *);
 o=add;
 doop(a,b,o);
 o=mul;
 doop(a,b,o);
 o=sub;
 doop(a,b,o);
 o=div;
 doop(a,b,o);
 return 0;
}

int doop(int *a, int *b, int (*cmp)( int *,  int *)) {
	int res = (*cmp)(a, b);
	cout << *a << " " << *b << " = " << res << "\n";
	return res;
}

void check(char *a, char *b, int (*cmp)(const char *, const char *))
{
  cout << "Testing for equality." << a << ":" << b;
  cout << '\n';

  if (!(*cmp)(a, b))
    cout << "Equal";
  else
    cout << "Not equal";
  cout << "\n";
}
