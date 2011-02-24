/*
 * Lab1.cpp
 *
 *  Created on: 23-Oct-2009
 *      Author: robm
 */

#include "Lab1.h"
#include <cstdlib>
#include <vector>
#include <cstdio>
Lab1::Lab1() {
	// TODO Auto-generated constructor stub

}

Lab1::~Lab1() {
	// TODO Auto-generated destructor stub
}

void swap(int &a, int &b){
	int temp = a;
	a = b;
	b = temp;
}

void swapp(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}
void increment(int &a, int &b, int inc) {
	a+=inc;
	b+=inc;
}
int main()   {
	int x, y; x=3; y=7;
	swap(x,y);
	cout << x << " " << y << "\n";
	int *f=&x,*g=&y;
	swapp(f,g);
	cout << *f << " " << *g << "\n";
	// question Ex 2
	increment(x,y,5);// f,g point to x,y anyways
	cout << *f << " " << *g << "\n";
	increment(*f,*g,5);// f,g point to x,y anyways
	cout << *f << " " << *g << "\n";
	// question ex 3
	string str = "";
		int add = 0;
	vector<int> *vec = new vector<int>();
	while (add>-1) {
		getline(cin,str);
		add = atol(str.c_str());
		vec->push_back(add);
		cout << add << "\n";
	}
	string s;
	char tmpStr[50];
	for (int i=0;i<vec->size();i++) {
		s.append(" * ");
		// quite odd that i have to use (*vec)[i] .
		cout << (*vec)[i] << "\n";
		sprintf( tmpStr, "%d", (*vec)[i] );
		s.append(tmpStr);
	}
	cout << s << "\n";
}
