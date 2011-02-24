/*
 * osc.h
 *
 *  Created on: 29-Dec-2010
 *      Author: robm
 */

#ifndef OSC_H_
#define OSC_H_

lo_address tgtOscAddr ;
lo_server_thread svrOSCThread;
void oscr_error_callback(int num, const char *m, const char *path);
int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
#endif /* OSC_H_ */
