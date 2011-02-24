/*
 * main.cpp
 *
 *  Created on: 29-Dec-2010
 *      Author: robm
 */
#include "window.h"
#include <iostream>
#include "lo/lo.h"
#include "osc.h"
#include "callbacks.h"

using namespace std;

int main(int argc, char** argv){
	tgtOscAddr = lo_address_new(NULL, "1111");
	svrOSCThread = lo_server_thread_new("1112", oscr_error_callback);
	lo_server_thread_add_method(svrOSCThread, NULL, NULL, generic_handler, NULL);
	createWindow();
	window_main->show();
	 lo_server_thread_start(svrOSCThread);
	cout << "opened" << endl;
	return Fl::run();
}
