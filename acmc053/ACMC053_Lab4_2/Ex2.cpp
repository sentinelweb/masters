/*
 * Ex2.cpp
 *
 *  Created on: 23-Oct-2009
 *      Author: robm
 */
using namespace std;
#include "Ex2.h"
#include <cstdlib>
#include <list>
#include <iterator>
#include "Vector2D.h"
Ex2::Ex2() {
	// TODO Auto-generated constructor stub

}

Ex2::~Ex2() {
	// TODO Auto-generated destructor stub
}
int main (  int argc, char *argv[]) {
	try {
		if (argc < 4) {// not the first arg is the exe filename - args are set in the run configuration
			throw 1;
		}
		char* fileToChange = argv[1];
		cerr<< fileToChange<<"\n";
		int position = atol(argv[2]);
		char charToInsert = argv[3][0];
		cerr<< position<< ":" <<charToInsert <<"\n";
		// exercise 1
		ifstream fin;
		fin.exceptions(std::ios::badbit );//| std::ios::eofbit| std::ios::failbit
		ofstream fileout;
		fileout.exceptions(std::ios::badbit  );//| std::ios::eofbit | std::ios::failbit

		try {
			//string path = "file.txt1";  // cause an exception
			string path = "file.txt";
			fin.open(path.c_str());
			string opath = "file1.txt";
			fileout.open(opath.c_str(),ios_base::trunc);

		   if( !fin ) {
			 cerr << "Error opening input stream" << endl;
		   }
		   if( !fileout ) {
			 cerr << "Error opening output stream" << endl;
		   }
		   if (fin && fileout) {
			   // or
			   int MAX_LENGTH = 100;
			   char line[MAX_LENGTH];

			   while( fin.getline(line, MAX_LENGTH) ) {
				   fileout << "read line: " << line << endl;
			   }
		   }
		   fileout.flush();fileout.close();
		   //cerr << "part 2" << "\n";
		   fstream fstr;
		   //fstr.exceptions(std::ios::badbit  );//| std::ios::eofbit | std::ios::failbit
		   fstr.open(fileToChange,ios_base::in);
		   if( !fstr ) {
			 cerr << "Error opening output stream" << endl;
		   }
		   cerr << "---" << "\n";
		   if (fstr) {
			   char* buf = new char[1];
			   int pos = 0;
			   string outBuf;
			   while(fstr.read(buf,1) ) {
				   pos++;
				  // cerr << "---" << "\n";
				   if (pos==position) { cerr << "-+-" << buf[0] << "\n";  buf[0]=charToInsert;   cerr << "-+-" << buf[0] << "\n"; }
				   //cerr << buf << "\n";
				   outBuf.append(buf);
			   }
			   //cout << outBuf;
			   fstr.close();
			   fstream fstr1;
			   fstr1.open(fileToChange,ios_base::out);
			   if( !fstr1 ) {
				 cerr << "Error opening output stream" << endl;
			   } else {
				   fstr1 << outBuf.c_str();
				   fstr1.flush();
				   fstr1.close();
			   }

		   }
		   // part3 : lists
		   list<int> list1(0) ;
		   for (int i=0;i<10;i++) {	   int x = rand();list1.push_back(x);   }
		   list<int>::iterator it;
		   for( it = list1.begin(); it != list1.end(); ++it ) {
			 cout << *it<< "\n";
		   }
		   list1.sort();
		   cout << "----" << "\n";
		   for( it = list1.begin(); it != list1.end(); ++it ) {
			 cout << *it << "\n";
		   }
		   cout << "----" << "\n";
		   for( it = --list1.end(); it != --list1.begin(); --it ) {
			 cout << *it << "\n";
		   }
		   cout << "----" << "\n";
		   cerr << "finished" << "\n";

		   // part 5: operator overloading
		   Vector2D v1 = Vector2D(4.5f,5.5f);
		   Vector2D v2 = Vector2D(4.5f,5.5f);
		   Vector2D v3 = Vector2D(6.5f,6.5f);
		   cout << (v1==v2) << ":" << (v2==v3) << endl;
		} catch (exception e) {
			cout << e.what() << "\n";
			if (fin) {
				if (fin.bad()) {
					cerr << "bad" << "\n";
				}
				if (fin.fail()) {
					cerr << "fail" << "\n";
				}
				if (fin.eof()) {
					cerr << "eof" << "\n";
				}
			}
		}
	}catch (int err) {
		if (err==1) {
			cerr << "error:" << err << "\n";
		}
	}
}
