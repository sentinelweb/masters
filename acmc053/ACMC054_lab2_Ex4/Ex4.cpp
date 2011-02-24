/*
 * Ex4.cpp
 *
 *  Created on: 04-Oct-2009
 *      Author: robm
 */
using namespace std;
#include "Ex4.h"
#include <iostream>
#include <string>

Ex4::Ex4() {
	// TODO Auto-generated constructor stub

}

Ex4::~Ex4() {
	// TODO Auto-generated destructor stub
}


/* EXERCISE 4: C++ STRINGS */
int main(){
		string s;
        while (cin >> s && s!="x") {
        	for (int i=0;i<s.length();i++) {
        		if (i==0){ s[i] = '-';}
        		else if (s[i-1]==' ') {
        			s[i]='-';
        		}
        	}
        	cout << s << " - length:" << s.length() << '\n';
        }

        // Ex 4a
        cout << "Exercise 4a" << '\n';
        string s1 = "hello";
        string s2 = "goodbye";
        string s3 = "bonjour";
        cout << "s1 s2: " << s1 << " " << s2 << '\n';
        cout << "s1+s2: " << (s1+s2) << '\n';
        cout << "s1==s2: " << (s1==s2) <<'\n';
        cout << "s1!=s2: " << (s1!=s2) <<'\n';
        cout << "s1>s2: " << (s1>s2) <<'\n';
        cout << "s1<s2: " << (s1<s2) <<'\n';

        return 0;
}
