/*
 * callbacks.cpp
 *
 *  Created on: 28-Dec-2010
 *      Author: robm
 */

#include <cstdio>
#include <cstring>
#include "window.h"
#include "lo/lo.h"

char* txt = new char[50];

extern void sldr_callback(Fl_Slider *fl_Slider, void* ud){
	//fl_Slider->maximum(255);

	sprintf(txt,"%.0f %.0f %.0f",fl_Slider->minimum(), fl_Slider->value(), fl_Slider->maximum());
	std::cout << txt << std::endl;
	lo_send(tgtOscAddr, "/sldr", "f", fl_Slider->value());
	tdisp->clear();
	tdisp->label(txt);
	//delete txt;
}

extern void dial1_callback(Fl_Dial *fl_Dial, void* ud){
	sprintf(txt,"%.0f %.0f %.0f",fl_Dial->minimum(), fl_Dial->value(), fl_Dial->maximum());
	std::cout << txt << std::endl;
	tdisp->clear();
	tdisp->label(txt);
}

extern void dial2_callback(Fl_Dial *fl_Dial, void* ud){
	sprintf(txt,"%.0f %.0f %.0f",fl_Dial->minimum(), fl_Dial->value(), fl_Dial->maximum());
	std::cout << txt << std::endl;
	slider1->value(fl_Dial->value());
	tdisp->clear();
	tdisp->label(txt);
}
void oscr_error_callback(int num, const char *msg, const char *path){
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}
int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data){
    int i;

    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++) {
		printf("arg %d '%c' ", i, types[i]);
		lo_arg_pp((lo_type)types[i], argv[i]);
		printf("\n");
    }

    if (strcmp(path , "/sldr") == 0) {
    	double d = (double) argv[0]->i;
    	slider1->value(d);
    	dial1->value(d);
    	//slider1->redraw();
    	slider1->draw();
    	//window_main->redraw();
    	printf("dbl %.1f  ", d);
    	//intinput->value(slider->value());

    }
    printf("\n");
    fflush(stdout);

    return 1;
}
