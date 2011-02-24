#include <wx/wx.h>
#include "window.h"
#include <wx/image.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dc.h>
#include <wx/slider.h>
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/menuitem.h>
#include <wx/scrolwin.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <iostream>
#include <math.h>
#include "Histogram.h"

using namespace std;

static MyFrame *m_self;

static const wxChar *FILETYPES = _T("All files (*.*)|*.*");
// check /usr/share/doc/wx2.8-examples for some examples.
IMPLEMENT_APP(BasicApplication)


bool BasicApplication::OnInit(){
	wxInitAllImageHandlers();
	MyFrame *frame = new MyFrame(_T("Basic Frame"), 50, 50, 1024, 768);
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

MyFrame::MyFrame(const wxChar *title, int xpos, int ypos, int width, int height): wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height)){

	fileMenu = new wxMenu;
	fileMenu->Append(LOAD_FILE_ID, _T("&Open file"));
	fileMenu->Append(SAVE_FILE_ID, _T("&Save file"));
	fileMenu->Append(REVERT_FILE_ID, _T("&Revert file"));
	fileMenu->AppendSeparator();
	fileMenu->Append(EXIT_ID, _T("E&xit"));

	processMenu = new wxMenu;
	processMenu->Append(INVERT_IMAGE_ID, _T("&Invert image"));
	processMenu->Append(CONVERT_GREY_ID, _T("Convert &Grey"));
	processMenu->Append(SCALE_ID, _T("&Rescale image"));
	processMenu->Append(SHIFT_ID, _T("&Shift image"));
	processMenu->Append(SCALE_ID, _T("S&cale image"));
	processMenu->Append(THRESH_LOW_ID, _T("&Low Threshold"));
	processMenu->Append(THRESH_HIGH_ID, _T("&High Threshold"));


	convolveMenu = new wxMenu;
	wxMenu* averageMenu = new wxMenu;
	averageMenu->Append(CNV_AVG3_ID, _T("3"));
	averageMenu->Append(CNV_AVG5_ID, _T("5"));
	averageMenu->Append(CNV_AVG7_ID, _T("7"));
	averageMenu->Append(CNV_AVG9_ID, _T("9"));
	averageMenu->Append(CNV_AVG11_ID, _T("11"));
	convolveMenu->Append(CNV_AVG_ID, _T("&Average"),averageMenu);
	wxMenu* waverageMenu = new wxMenu;
	waverageMenu->Append(CNV_WAVG3_ID, _T("3"));
	waverageMenu->Append(CNV_WAVG5_ID, _T("5"));
	waverageMenu->Append(CNV_WAVG7_ID, _T("7"));
	waverageMenu->Append(CNV_WAVG9_ID, _T("9"));
	waverageMenu->Append(CNV_WAVG11_ID, _T("11"));
	convolveMenu->Append(CNV_WAVG_ID, _T("&Weighted Avg"),waverageMenu);
	convolveMenu->Append(CNV_LP4_ID, _T("&Laplacian 4"));
	convolveMenu->Append(CNV_LP4E_ID, _T("&Laplacian 4 enhance"));
	convolveMenu->Append(CNV_LP8_ID, _T("&Laplacian 8"));
	convolveMenu->Append(CNV_LP8E_ID, _T("&Laplacian  enhance"));
	convolveMenu->Append(CNV_ROB_ID, _T("&Roberts"));
	convolveMenu->Append(CNV_ROBA_ID, _T("&Roberts Avg"));
	convolveMenu->Append(CNV_SOBX_ID, _T("&Sobel X"));
	convolveMenu->Append(CNV_SOBY_ID, _T("&Sobel Y"));
	convolveMenu->Append(CNV_SOBDN_ID, _T("&Sobel Diagonal Neg"));
	convolveMenu->Append(CNV_SOBDP_ID, _T("&Sobel Diagonal Pos"));
	convolveMenu->Append(CNV_PREWX_ID, _T("&Prewitt X"));
	convolveMenu->Append(CNV_PREWY_ID, _T("&Prewitt Y"));
	convolveMenu->Append(CNV_PREWDN_ID, _T("&Prewitt Diagonal Neg"));
	convolveMenu->Append(CNV_PREWDP_ID, _T("&Prewitt Diagonal Pos"));
	convolveMenu->Append(CNV_HISTO_ID, _T("&Histogram"));
	convolveMenu->Append(CNV_COLOUR_HISTO_ID, _T("&Histogram (Colour)"));
	convolveMenu->Append(CNV_EQUALISE_ID, _T("&Equalise"));
	convolveMenu->Append(CNV_COLOUR_EQUALISE_ID, _T("&Equalise (Colour)"));
	convolveMenu->Append(CNV_TEST_YUV_ID, _T("&Test YUV"));
	/*
	 * // trying to make dynamic connection to menuitems - so dont have to use evnt tables
	wxMenuItem *wavg = new wxMenuItem(convolveMenu,-1,_T("&Weighted Avg"));
	wxEventHandler *evt_hdlr = new wxEventHandler(MyFrame::OnCnvAvg);
	evt_hdlr->Connect(wxEVT_COMMAND_MENU_SELECTED);
	convolveMenu->Append(wavg);
	*/


	editMenu = new wxMenu;
	editMenu->Append(UNDO_ID, _T("&Undo"));

	roiMenu = new wxMenu;
	roiMenu->Append(ROI_THRESH_LOW_ID, _T("&Low Threshold"));
	roiMenu->Append(ROI_THRESH_HIGH_ID, _T("&High Threshold"));
	roiMenuShow = new wxMenuItem(roiMenu,ROI_SHOW, _T("&Show/Hide"),_T(""),wxITEM_CHECK);
	roiMenu->Append(roiMenuShow);
	roiMenu->Append(ROI_CLEAR, _T("&Clear"));

	osfMenu = new wxMenu;
	//wxMenu* minMenu = new wxMenu;
	//minMenu->Append(OSF_MIN3, _T("&3"),_T(""),wxITEM_RADIO);
	//minMenu->Append(OSF_MIN5, _T("&5"),_T(""),wxITEM_RADIO);
	//minMenu->Append(OSF_MIN7, _T("&7"),_T(""),wxITEM_RADIO);
	//minMenu->Append(OSF_MIN, _T("&9"),_T(""),wxITEM_RADIO);
	//minMenu->Append(OSF_MIN, _T("&11"),_T(""),wxITEM_RADIO);
	osfMenu->Append(OSF_MIN, _T("&Min"));//,minMenu
	osfMenu->Append(OSF_MAX, _T("&Max"));
	osfMenu->Append(OSF_MIDPOINT, _T("&Midpoint"));
	osfMenu->Append(OSF_MEDIAN, _T("&Median"));

	menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(editMenu, _T("&Edit"));
	menuBar->Append(processMenu, _T("&Process"));
	menuBar->Append(convolveMenu, _T("&Convolve"));
	menuBar->Append(roiMenu, _T("&ROI"));
	menuBar->Append(osfMenu, _T("&OSF"));

	SetMenuBar(menuBar);
	CreateStatusBar(3);
	//this->scrollArea = new wxScrolledWindow(this,SCROLL_ID);
	//this->scrollArea->SetVirtualSize(5000,8000);
	//this->AddChild(this->scrollArea);

	oldWidth = 0;
	oldHeight = 0;

	roiHigh=255;
	roiLow=0;
	roiImgBuf=(unsigned char *)malloc(0);
	clearROI();

	loadedImage = 0;

	/* initialise the variables that we added */
	imgWidth = imgHeight = 0;
	imgBuf = workingImgBuf = NULL;
	workingImage = NULL;
	stuffToDraw = 0;

	m_self=this;
}
void MyFrame::makeSliderPopup(wxString title){
	popupFrame = new wxFrame((wxWindow*)this,
			POP_ID,
			title,
			wxPoint(0,0),
			wxSize(200,200),
			wxDEFAULT_FRAME_STYLE,
			wxString(_T("sliderFrame"))
	);
	//wxWindowID winId(-1);
	//wxSlider *
	slider = new wxSlider(
			(wxWindow*)popupFrame,
			SLIDER_ID,
			0,
			-255,
			255,
			wxDefaultPosition,
			wxSize(200,20),
			wxSL_HORIZONTAL,
			wxDefaultValidator,
			wxString(_T("slider"))
		);
	wxButton *b = new wxButton((wxWindow*)popupFrame,SCALE_BUT_OK , _T("&OK"),wxPoint(0,40));

	popupFrame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MyFrame::OnClosePopup));
	popupFrame->SetLabel(title);
	popupFrame->Show(TRUE);
}
///////////////// slider window /////////////////////////////////
MyFrame::~MyFrame(){
	if(loadedImage){
		loadedImage->Destroy();
		loadedImage = 0;
		//free(imgBuf);
	}
	  /* release resources */
	  if(workingImage){
	    workingImage->Destroy();
	    workingImage = 0;
	    //free(workingImgBuf);
	  }
	  m_self = 0;
	  //if (popupFrame!=NULL) {
	//	  popupFrame->Destroy();
	 // }
	  /*  if(NULL!=imgBuf)
	    free(imgBuf);
	  if(NULL!=invertedImgBuf)
	  free(invertedImgBuf); */
	  // Because the Destroy method of image has already released the image buffer.

}
void MyFrame::OnSlide(wxScrollEvent & wxEvent){
	if (!sliderSemaphore) {
		sliderSemaphore=true;
		float val = slider->GetValue()*sliderScaleFactor;
		wxString str = wxString(_T("value"));
		(this ->* ((MyFrame*)this)->MyFrame::processingOperation) (&val);
		stuffToDraw = PREVIEW_IMG;
		Refresh();
		changesPending=1;
		//cerr<<changesPending<<"\n";
		//cerr<<popupFrame<<"\n";
		sliderSemaphore=false;
	}
}
void MyFrame::OnClosePopup(wxCloseEvent& event)	{
	// use m_self reference to MyFrame::this as popup->Connect binds to the popup frame
	if (m_self->changesPending==1) {
	  wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Update changes?"), wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
	  int ret = dial->ShowModal();
	  dial->Destroy();
	  if (ret == wxID_YES) {
		  m_self->commitChanges();
	  } else {
		  m_self->stuffToDraw = WORKING_IMG;
		  m_self->Refresh();
	  }
	}
	m_self->popupFrame->Destroy();
}
////////////// File menu handlers /////////////////////////////////////
void MyFrame::OnOpenFile(wxCommandEvent & event){
	wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);
	openFileDialog->SetDirectory(wxT("/home/robm/Documents/uni/AMCM053/images"));
	//openFileDialog->SetDirectory(wxT("/home/robm/Documents/uni/acmc053/images"));
	if(openFileDialog->ShowModal() == wxID_OK){
		wxString filename = openFileDialog->GetFilename();
		wxString path = openFileDialog->GetDirectory() + wxString(_T("/"));
		loadedImage = new wxImage(path + filename);
		if(loadedImage->Ok()){
			 /* As long as the image has been loaded successfully, we retrieve its data. */
			 imgWidth  = loadedImage->GetWidth();
			 imgHeight = loadedImage->GetHeight();
			 imgBuf = (unsigned char*)malloc(imgWidth*imgHeight*3); // suppose we have RGB image.
			                                                             // PITFALL! Here using 'malloc' rather than 'new' is required
			                                                             // by the implementation of the frame work.
			 imgBuf=loadedImage->GetData(); // get image data

			 workingImgBuf=copy(imgBuf);
			 workingImage = new wxImage(imgWidth, imgHeight);
			 workingImage->SetData(workingImgBuf);
			 clearROI();
			 stuffToDraw = WORKING_IMG;    // set the display flag
		} else {
			loadedImage->Destroy();
			loadedImage = 0;
		}
		Refresh();
	}
}

void MyFrame::OnSaveFile(wxCommandEvent & event){
	wxFileDialog *saveFileDialog = new wxFileDialog ( this, _T("Save file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);
	if(saveFileDialog->ShowModal() == wxID_OK){
		wxString filename = saveFileDialog->GetFilename();
		wxString path = saveFileDialog->GetDirectory() + wxString(_T("/") +filename);
		wxString status ;
		wxImage *saveImage = loadedImage;
		if (stuffToDraw == WORKING_IMG) {
			saveImage = workingImage;
		}
		if(saveImage->Ok()){
			wxString result;
			if (saveImage->SaveFile(path)) {// save image data
				  result = wxString(_T("Saved ..."));

			} else {
				  result = wxString(_T("Not saved !!!!"));
			}
			//result += status;
			GetStatusBar()->SetStatusText(result);
		}
	}
}

void MyFrame::OnRevert (wxCommandEvent & event){
	free(workingImgBuf);
	workingImgBuf=copy(imgBuf);
	workingImage = new wxImage(imgWidth, imgHeight);
	workingImage->SetData(workingImgBuf);
	stuffToDraw = WORKING_IMG;
	Refresh();
}
void MyFrame::OnExit (wxCommandEvent & event){
	Close(TRUE);
}
////////////// Edit menu handlers /////////////////////////////////////
void MyFrame::OnUndo (wxCommandEvent & event){
	if (undoImgBuf>0) {
		free(workingImgBuf);
		//workingImgBuf = undoImgBuf;
		//copy back buffer instead
		workingImgBuf=copy(undoImgBuf);
		free(undoImgBuf);
		undoImgBuf=0;
		workingImage = new wxImage(imgWidth, imgHeight);
		workingImage->SetData(workingImgBuf);
		stuffToDraw = WORKING_IMG;
		Refresh();
	} else {
		GetStatusBar()->SetStatusText(wxT("No Undos"));
	}
}

void MyFrame::OnInvertImage(wxCommandEvent & event){
    checkImageLoaded(event);
    previewImgBuf = (unsigned char*)malloc(imgWidth*imgHeight*3); // as above
    for( int i=0;i<imgHeight;i++) {
      for(int j=0;j<imgWidth;j++){
    	  int idx = (i * imgWidth + j) * 3;
    	  if (checkInROI(j,i)) {
    		  previewImgBuf[idx  ] = 255-workingImgBuf[idx];
    		  previewImgBuf[idx+1] = 255-workingImgBuf[idx+1];
    		  previewImgBuf[idx+2] = 255-workingImgBuf[idx+2];
    	  } else {
			for (int k=0;k<3;k++) {
				previewImgBuf[idx + k] = workingImgBuf[idx + k];
			}
    	  }
      }
    }
    previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
  /*
  if (stuffToDraw == WORKING_IMG) {
	  GetStatusBar()->SetStatusText(wxString(_T("Displaying Inverted Image")));
  } else {
	  GetStatusBar()->SetStatusText(wxString(_T("Displaying Normal Image")));
  }
  */
  Refresh();
}
void MyFrame::OnShift(wxCommandEvent & event) {
	checkImageLoaded(event);
	makeSliderPopup(wxT("Shift image"));
	processingOperation = &MyFrame::shiftImage;
	sliderScaleFactor = 1.0;
}
void MyFrame::OnScale(wxCommandEvent & event){
	checkImageLoaded(event);
	makeSliderPopup(wxT("Scale image"));
	processingOperation = &MyFrame::scaleImage;
	sliderScaleFactor = 512.0/255.0;
}
void MyFrame::OnLowThreshold(wxCommandEvent & event) {
	checkImageLoaded(event);
	makeSliderPopup(wxT("Low Threshold"));
	processingOperation = &MyFrame::lowThreshold;
	sliderScaleFactor = 1.0;
}
void MyFrame::OnHighThreshold(wxCommandEvent & event) {
	checkImageLoaded(event);
	makeSliderPopup(wxT("High Threshold"));
	processingOperation = &MyFrame::highThreshold;
	sliderScaleFactor = 1.0;
}

void MyFrame::OnConvertGrey(wxCommandEvent & event){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	for(int j = 0;j < imgHeight;j++){
		for(int i = 0;i < imgWidth;i++){
			int idx = (j * imgWidth + i) * 3;
			if (checkInROI(i,j)) {
				unsigned char r = workingImgBuf[ idx];
				unsigned char g = workingImgBuf[idx+1];
				unsigned char b = workingImgBuf[idx+2];
				float yl = 0.299 * r + 0.587*g + 0.114*b;
				for (int k=0;k<3;k++) {
					int idx1 = (j * imgWidth + i) * 3 + k;
					previewImgBuf[idx1] = yl;
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
////////////// Convolution menu handlers /////////////////////////////////////
void MyFrame::OnCnvAvg3(wxCommandEvent & event) {doAvg(event,3);}
void MyFrame::OnCnvAvg5(wxCommandEvent & event) {doAvg(event,5);}
void MyFrame::OnCnvAvg7(wxCommandEvent & event) {doAvg(event,7);}
void MyFrame::OnCnvAvg9(wxCommandEvent & event) {doAvg(event,9);}
void MyFrame::OnCnvAvg11(wxCommandEvent & event) {doAvg(event,11);}

void MyFrame::doAvg(wxCommandEvent & event,int size) {
	//currentMatrix = new Mask((float*)average3x3, 3, average3x3_mult);
	float* matrix  = genAvg(size);
	currentMatrix = new Mask(matrix, size,(1.0/(size*size)));
	//currentMatrix = genAvg(11);
	doConvlove(event);
}
void MyFrame::doWAvg(wxCommandEvent & event,int size) {
	//currentMatrix = new Mask((float*)weight3x3, 3, weight3x3_mult);
	//float size = 5.0;
	//float* matrix  = genWAvg(size);
	//currentMatrix = new Mask(matrix, size,(1.0/(size*size)));
	int sizeM = sizeof(float)*size*size;
	float* matrix  = (float*)malloc(sizeM);
	currentMatrix = genWAvg(matrix,size);
	doConvlove(event);
}
void MyFrame::OnCnvWAvg3(wxCommandEvent& event) {doWAvg(event,3);}
void MyFrame::OnCnvWAvg5(wxCommandEvent& event) {doWAvg(event,5);}
void MyFrame::OnCnvWAvg7(wxCommandEvent& event) {doWAvg(event,7);}
void MyFrame::OnCnvWAvg9(wxCommandEvent& event) {doWAvg(event,9);}
void MyFrame::OnCnvWAvg11(wxCommandEvent& event) {doWAvg(event,11);}

void MyFrame::OnCnvLP4(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)laplacian4, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvLP4E(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)laplacian4enhance, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvLP8(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)laplacian8, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvLP8E(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)laplacian8enhance, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvRob(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)roberts, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvRobAvg(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)robertsabs, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvSobX(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)sobelX, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvSobY(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)sobelY, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvSobDiagN(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)sobelDiagNeg, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvSobDiagP(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)sobelDiagPos, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvPrewX(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)prewittX, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvPrewY(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)prewittY, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvPrewDiagN(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)prewittDiagNeg, 3, 1.0);
	doConvlove(event);
}
void MyFrame::OnCnvPrewDiagP(wxCommandEvent& event) {
	currentMatrix = new Mask((float*)prewittDiagPos, 3, 1.0);
	doConvlove(event);
}
void MyFrame::doConvlove(wxCommandEvent & event) {
	checkImageLoaded(event);
	processingOperation = &MyFrame::convolve;
	float *val = 0;
	(this ->* ((MyFrame*)this)->MyFrame::processingOperation) (val);
	commitChanges();
}



void MyFrame::convolve(float* val){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	//int arraySize = 3;
	//float currentMatrix[][] = average3x3;
    for(int y = 0;y < imgHeight;y++){
        for(int x = 0;x < imgWidth;x++){
        	int idx =(y * imgWidth + x) * 3;
        	if (checkInROI(x,y)) {
				for (int k=0;k<3;k++) {// colours
					float accum = 0;
					// process neighbourhood
					int midpoint = currentMatrix->size/2; // assumes odd size square matrix.
					for (int l=0;l<currentMatrix->size;l++) {
						for (int m=0;m<currentMatrix->size;m++) {
							//cerr<< currentMatrix->get(l,m) << "\n";
							int nx=x+(l-midpoint);nx=nx<0?0:nx;nx=nx>imgWidth?imgWidth:nx;
							int ny=y+(m-midpoint);ny=nx<0?0:ny;ny=ny>imgHeight?imgHeight:ny;
							float v = ((float) workingImgBuf[(ny * imgWidth + nx) * 3 + k]) * currentMatrix->get(l,m);
							accum+=v;
						}
					}
					previewImgBuf[ idx+ k] = round(accum*currentMatrix->multiplier); // as above
				}
        	} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
        }
    }
    previewImage = new wxImage(imgWidth, imgHeight);
    previewImage->SetData(previewImgBuf);
}
void MyFrame::OnTestYUVConversion(wxCommandEvent & event) {
	checkImageLoaded(event);
	if (this->histoGram != NULL) {
		delete (this->histoGram);
	}
	this->histoGram = new Histogram();
	this->histoGram->setImgHeight(imgHeight);
	this->histoGram->setImgWidth(imgWidth);
	unsigned char *tmp = this->histoGram->toYUV(workingImgBuf);
	previewImgBuf = this->histoGram->toRGB(tmp);
	free(tmp);
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
	Refresh();
}
void MyFrame::OnColourHistogram(wxCommandEvent & event) {
	checkImageLoaded(event);
	if (this->histoGram != NULL) {
		delete (this->histoGram);
	}
	this->histoGram = new Histogram();
	this->histoGram->setImgHeight(imgHeight);
	this->histoGram->setImgWidth(imgWidth);
	this->histoGram->makeHistogram(workingImgBuf);
	histImgBuf = this->histoGram->displayColour();
	showHist=true;
	Refresh();
}
void MyFrame::OnHistogram(wxCommandEvent & event){
	checkImageLoaded(event);
	if (this->histoGram != NULL) {
		delete (this->histoGram);
	}
	this->histoGram = new Histogram();
	this->histoGram->setImgHeight(imgHeight);
	this->histoGram->setImgWidth(imgWidth);
	this->histoGram->makeHistogram(workingImgBuf);
	histImgBuf = this->histoGram->display();
	showHist=true;
	Refresh();
}

void MyFrame::OnEqualise(wxCommandEvent & event){
	checkImageLoaded(event);
	if (this->histoGram != NULL) {
		delete (this->histoGram);
	}
	this->histoGram = new Histogram();
	this->histoGram->setImgHeight(imgHeight);
	this->histoGram->setImgWidth(imgWidth);
	previewImgBuf = this->histoGram->equalise(workingImgBuf);
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
void MyFrame::OnColourEqualise(wxCommandEvent & event){
	checkImageLoaded(event);
	if (this->histoGram != NULL) {
		delete (this->histoGram);
	}
	this->histoGram = new Histogram();
	this->histoGram->setImgHeight(imgHeight);
	this->histoGram->setImgWidth(imgWidth);
	previewImgBuf = this->histoGram->equaliseColour(workingImgBuf);
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
////////////// OSF menu handlers /////////////////////////////////////
void MyFrame::OnMin(wxCommandEvent & event){// only works for grey
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	//int arraySize = 3;
	int size=5;
	//float currentMatrix[][] = average3x3;
	for(int y = 0;y < imgHeight;y++){
		for(int x = 0;x < imgWidth;x++){
			int idx =(y * imgWidth + x) * 3;
			if (checkInROI(x,y)) {

				for (int k=0;k<3;k++) {// colours
					char minValue = -1;
					int midpoint = size/2;
					for (int l=0;l<size;l++) {
						for (int m=0;m<size;m++) {
							//cerr<< currentMatrix->get(l,m) << "\n";
							int nx=x+(l-midpoint);nx=nx<0?0:nx;nx=nx>imgWidth?imgWidth:nx;
							int ny=y+(m-midpoint);ny=nx<0?0:ny;ny=ny>imgHeight?imgHeight:ny;

							int v = ((float) workingImgBuf[(ny * imgWidth + nx) * 3 + k]);
							if (minValue==-1 || minValue>v) {
								minValue=v;
							}
						}
					}
					previewImgBuf[ idx+ k] = minValue; //
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}

void MyFrame::OnMax(wxCommandEvent & event){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	int size = 5;
	for(int y = 0;y < imgHeight;y++){
		for(int x = 0;x < imgWidth;x++){
			int idx =(y * imgWidth + x) * 3;
			if (checkInROI(x,y)) {

				for (int k=0;k<3;k++) {// colours
					char maxValue = -1;
					int midpoint = size/2;
					for (int l=0;l<size;l++) {
						for (int m=0;m<size;m++) {
							//cerr<< currentMatrix->get(l,m) << "\n";
							int nx=x+(l-midpoint);nx=nx<0?0:nx;nx=nx>imgWidth?imgWidth:nx;
							int ny=y+(m-midpoint);ny=nx<0?0:ny;ny=ny>imgHeight?imgHeight:ny;

							int v = ((float) workingImgBuf[(ny * imgWidth + nx) * 3 + k]);
							if (maxValue==-1 || maxValue<v) {
								maxValue=v;
							}
						}
					}
					previewImgBuf[ idx+ k] = maxValue; //
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
int compare_chars( const void* a, const void* b ) {  // compare function
	char* arg1 = (char*) a;
	char* arg2 = (char*) b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    else return 1;
}

void MyFrame::OnMedian(wxCommandEvent & event){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	int size = 5;
	for(int y = 0;y < imgHeight;y++){
		for(int x = 0;x < imgWidth;x++){
			int idx =(y * imgWidth + x) * 3;
			if (checkInROI(x,y)) {

				for (int k=0;k<3;k++) {// colours
					char values[size*size];
					int midpoint = size/2;
					for (int l=0;l<size;l++) {
						for (int m=0;m<size;m++) {
							//cerr<< currentMatrix->get(l,m) << "\n";
							int nx=x+(l-midpoint);nx=nx<0?0:nx;nx=nx>imgWidth?imgWidth:nx;
							int ny=y+(m-midpoint);ny=nx<0?0:ny;ny=ny>imgHeight?imgHeight:ny;

							int v = ((float) workingImgBuf[(ny * imgWidth + nx) * 3 + k]);
							values[l*size+m]=(char) v;
						}
					}
					qsort(values,size*size,sizeof(char),compare_chars);
					previewImgBuf[ idx+ k] = values[(size*size)/2]; //
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
void MyFrame::OnMidpoint(wxCommandEvent & event){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	int size = 5;
	//float currentMatrix[][] = average3x3;
	for(int y = 0;y < imgHeight;y++){
		for(int x = 0;x < imgWidth;x++){
			int idx =(y * imgWidth + x) * 3;
			if (checkInROI(x,y)) {
				for (int k=0;k<3;k++) {// colours
					char maxValue = -1;
					char minValue = -1;
					int midpoint = size/2;
					for (int l=0;l<size;l++) {
						for (int m=0;m<size;m++) {
							//cerr<< currentMatrix->get(l,m) << "\n";
							int nx=x+(l-midpoint);nx=nx<0?0:nx;nx=nx>imgWidth?imgWidth:nx;
							int ny=y+(m-midpoint);ny=nx<0?0:ny;ny=ny>imgHeight?imgHeight:ny;

							int v = ((float) workingImgBuf[(ny * imgWidth + nx) * 3 + k]);
							if (maxValue==-1 || maxValue<v) {
								maxValue=v;
							}
							if (minValue==-1 || minValue<v) {
								minValue=v;
							}
						}
					}
					previewImgBuf[ idx+ k] = (maxValue-minValue)/2; //
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
	commitChanges();
}
////////////// ROI menu handlers /////////////////////////////////////
void MyFrame::OnROIHighThreshold(wxCommandEvent &event){
	checkImageLoaded(event);
	makeSliderPopup(wxT("ROI High Threshold"));
	processingOperation = &MyFrame::doROIHigh;
	sliderScaleFactor = 1.0;
	showROI=true;
	roiMenuShow->Check(showROI);
};
void MyFrame::OnROILowThreshold(wxCommandEvent &event){
	checkImageLoaded(event);
	makeSliderPopup(wxT("ROI Low Threshold"));
	processingOperation = &MyFrame::doROILow;
	sliderScaleFactor = 1.0;
	showROI=true;
	roiMenuShow->Check(showROI);
};
void MyFrame::OnROIShowHide(wxCommandEvent &event){
	if (event.IsChecked()) {
		showROI=true;
	} else {
		showROI=false;
	}
	cerr << "show" << showROI <<"\n";
	Refresh();
};
void MyFrame::OnROIClear(wxCommandEvent &event){
	roiLow = 0;
	roiHigh=255;
	clearROI();
};
void MyFrame::doROIHigh(float* val){
	if (*val>=roiLow) {
		roiHigh=*val;
	} else {
		roiHigh=roiLow;
	}
	genROI();

};
void MyFrame::doROILow(float* val){
	if (*val<=roiHigh) {
		roiLow=*val;
	} else {
		roiLow=roiHigh;
	}
	roiLow=*val;
	genROI();
};
void MyFrame::genROI(){
	clearROI();
	for(int i = 0;i < imgHeight;i++){
		for(int j = 0;j < imgWidth;j++){
			//cerr << "here:set" << "\n";
			int idx = (i * imgWidth + j) * 3;
			unsigned char r = workingImgBuf[ idx];
			unsigned char g = workingImgBuf[idx+1];
			unsigned char b = workingImgBuf[idx+2];
			float yl = 0.299 * r + 0.587*g + 0.114*b;
			for (int k=0;k<3;k++) {
				if (yl>roiHigh || yl<roiLow) {
					roiImgBuf[idx+k] = 0;
				} else {
					roiImgBuf[idx+k] = 255;
				}
			}
		}
	}
	//keep slider happy.
	previewImgBuf=(unsigned char*)(malloc(imgWidth * imgHeight*3 ));//workingImgBuf;
	for (int i=0;i<imgWidth * imgHeight*3;i++) {
		previewImgBuf[i]=workingImgBuf[i];
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
};
bool MyFrame::checkInROI(int x,int y){
	int idx = (y * imgWidth + x) * 3;
	return (roiImgBuf[idx]==255);
}
void MyFrame::clearROI(){
	free(roiImgBuf);

	roiImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight*3 ));
	for (int i=0;i<imgWidth * imgHeight*3;i++) {
		roiImgBuf[i]=255;
	}
	Refresh();
};
////////////// Image modification util methods /////////////////////////////////////
void MyFrame::shiftImage(float* val){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
    for(int i = 0;i < imgHeight;i++){
        for(int j = 0;j < imgWidth;j++){
        	int idx = (i * imgWidth + j) * 3 ;
        	if (checkInROI(j,i)) {
				for (int k=0;k<3;k++) {
					float v = (float) workingImgBuf[idx + k] ;
					v+=*val;
					v=(v > 255) ? 255 : v;
					v=(v < 0) ? 0 : v;
					previewImgBuf[(i * imgWidth + j) * 3 + k] = round(v);
				}
        	} else {
        		for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
        	}
        }
    }
    previewImage = new wxImage(imgWidth, imgHeight);
    previewImage->SetData(previewImgBuf);
}

void MyFrame::scaleImage(float* val){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
    for(int i = 0;i < imgHeight;i++){
        for(int j = 0;j < imgWidth;j++){
        	int idx = (i * imgWidth + j) * 3 ;
        	if (checkInROI(j,i)) {
				for (int k=0;k<3;k++) {
					float v = (float) workingImgBuf[idx + k] ;
					v+=*val;
					v=(v > 255) ? 255 : v;
					v=(v < 0) ? 0 : v;
					previewImgBuf[(i * imgWidth + j) * 3 + k] = round(v);
				}
        	}else {
        		for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
        	}
        }
    }
    previewImage = new wxImage(imgWidth, imgHeight);
    previewImage->SetData(previewImgBuf);
    //commitChanges();
}

void MyFrame::lowThreshold(float* val){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
    for(int j = 0;j < imgHeight;j++){
        for(int i = 0;i < imgWidth;i++){
        	int idx = (j * imgWidth + i) * 3;
        	if (checkInROI(i,j)) {
				unsigned char r = workingImgBuf[ idx];
				unsigned char g = workingImgBuf[idx+1];
				unsigned char b = workingImgBuf[idx+2];
				float yl = 0.299 * r + 0.587*g + 0.114*b;
				for (int k=0;k<3;k++) {
					int idx1 = (j * imgWidth + i) * 3 + k;
					if (yl<*val) {
						previewImgBuf[idx1] = 0;
					} else {
						previewImgBuf[idx1] = workingImgBuf[idx1];
					}
				}
        	} else {
        		for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
        	}
        }
    }
    previewImage = new wxImage(imgWidth, imgHeight);
    previewImage->SetData(previewImgBuf);
    //commitChanges();
}

void MyFrame::highThreshold(float* val){
	previewImgBuf = (unsigned char*)(malloc(imgWidth * imgHeight * 3)); // as above
	for(int j = 0;j < imgHeight;j++){
		for(int i = 0;i < imgWidth;i++){
			int idx = (j * imgWidth + i) * 3;
			if (checkInROI(i,j)) {
				unsigned char r = workingImgBuf[ idx];
				unsigned char g = workingImgBuf[idx+1];
				unsigned char b = workingImgBuf[idx+2];
				float yl = 0.299 * r + 0.587*g + 0.114*b;
				for (int k=0;k<3;k++) {
					int idx1 = (j * imgWidth + i) * 3 + k;
					if (yl>*val) {
						previewImgBuf[idx1] = 0;
					} else {
						previewImgBuf[idx1] = workingImgBuf[idx1];
					}
				}
			} else {
				for (int k=0;k<3;k++) {
					previewImgBuf[idx + k] = workingImgBuf[idx + k];
				}
			}
		}
	}
	previewImage = new wxImage(imgWidth, imgHeight);
	previewImage->SetData(previewImgBuf);
}

void MyFrame::checkImageLoaded(wxCommandEvent & event){
	if (loadedImage == NULL) {
		GetStatusBar()->SetStatusText(wxString(_T("Please load an image.")));
		OnOpenFile(event);
		if (loadedImage == NULL) { return;}
	}
}

void MyFrame::commitChanges(){
	free(undoImgBuf);
	undoImgBuf=copy(workingImgBuf );
    free(workingImgBuf);
    workingImgBuf=copy(previewImgBuf);
    free(previewImgBuf);
    workingImage = new wxImage(imgWidth, imgHeight);
    workingImage->SetData(workingImgBuf);
    stuffToDraw = WORKING_IMG;
    Refresh();
    changesPending=0;
}

void MyFrame::OnOperationOK(wxCommandEvent & event){
    commitChanges();
    popupFrame->Destroy();
}
unsigned char* MyFrame::copy(unsigned char* src){
	unsigned char* tgt=(unsigned char*)(malloc(imgWidth * imgHeight * 3));
	for (int i=0;i<imgWidth * imgHeight*3;i++) {
		tgt[i]=src[i];
	}
	return tgt;
}
void MyFrame::OnPaint(wxPaintEvent & event){
	//cerr <<"here"<<"\n";
	//wxPaintDC dc(scrollArea);
	wxImage *roiImage;
	wxPaintDC dc(this);
	//cerr <<"here"<<"\n";
	int cWidth, cHeight;
	//scrollArea->GetSize(&cWidth, &cHeight);
	GetSize(&cWidth, &cHeight);
	//cerr <<"here"<<"\n";
	wxMemoryDC *temp_dc = new wxMemoryDC(&dc);
	// We can now draw into the offscreen DC...
	temp_dc->Clear();

	// draw all the things for this frame ie put your code here
	// .....

	//if(loadedImage) {
	//	wxBitmap bitmap = wxBitmap(*loadedImage);
	//	temp_dc->SelectObject(bitmap);//given bitmap
	//}
	// overlay ROI
	wxBitmap *bitmap;

	switch(stuffToDraw){
	  case NOTHING:
	    return;
	  case ORIGINAL_IMG:
	    bitmap = new wxBitmap(*loadedImage);
	    break;
	  case WORKING_IMG:
	    bitmap = new wxBitmap(*workingImage);
	    break;
	  case PREVIEW_IMG:
		bitmap = new wxBitmap(*previewImage);
		break;
	}

	temp_dc->SelectObject(*bitmap);//given bitmap

	//end draw all the things
	// Copy from this DC to another DC.
	dc.Blit(0, 0, cWidth, cHeight, temp_dc, 0, 0);

	if (showHist) {
		wxImage *histImage = new wxImage(255, 255);
		histImage->SetData(histImgBuf);
		wxBitmap bitmap1 = wxBitmap(*histImage);
		temp_dc->SelectObject(bitmap1);
		dc.Blit(cWidth-255, 0, 255, 255, temp_dc, 0, 0);
		free(histImage);
		showHist=false;
	}
	if (showROI) {
		roiImage = new wxImage(imgWidth, imgHeight);
		roiImage->SetData(roiImgBuf);
		wxBitmap *bitmap1 = new wxBitmap(*roiImage);
		//wxMask mask = wxMask(*bitmap1,wxColor(255,255,255));
		//bitmap1->SetMask(&mask);
		temp_dc->SelectObject(*bitmap1);
		//dc.Blit(cWidth-255, 0, imgWidth, imgWidth, temp_dc, 0, 0);
		dc.Blit(0, 0, cWidth, cHeight, temp_dc, 0, 0,wxAND);
		free(roiImage);
		cerr << "mask" <<"\n";
	}
	free(bitmap);
	//if (showROI) {	free(roiImage);	}
	delete temp_dc; // get rid of the memory DC
}

BEGIN_EVENT_TABLE (MyFrame, wxFrame)
	EVT_MENU ( LOAD_FILE_ID, MyFrame::OnOpenFile)
	EVT_MENU ( INVERT_IMAGE_ID,  MyFrame::OnInvertImage)
	EVT_MENU ( SAVE_FILE_ID,  MyFrame::OnSaveFile)
	EVT_MENU ( REVERT_FILE_ID,  MyFrame::OnRevert)
	EVT_MENU ( CONVERT_GREY_ID,  MyFrame::OnConvertGrey)
	EVT_MENU ( SCALE_ID,  MyFrame::OnScale)
	EVT_MENU ( SHIFT_ID,  MyFrame::OnShift)
	EVT_MENU ( THRESH_LOW_ID,  MyFrame::OnLowThreshold)
	EVT_MENU ( THRESH_HIGH_ID,  MyFrame::OnHighThreshold)
	EVT_MENU ( EXIT_ID, MyFrame::OnExit)
	EVT_MENU ( UNDO_ID, MyFrame::OnUndo)
	//ROI
	EVT_MENU ( ROI_THRESH_LOW_ID, MyFrame::OnROILowThreshold)
	EVT_MENU ( ROI_THRESH_HIGH_ID, MyFrame::OnROIHighThreshold)
	EVT_MENU ( ROI_SHOW, MyFrame::OnROIShowHide)
	EVT_MENU ( ROI_CLEAR, MyFrame::OnROIClear)
	//OSF
	EVT_MENU ( OSF_MIN, MyFrame::OnMin)
	//EVT_MENU ( OSF_MIN3, MyFrame::OnMin)
	//EVT_MENU ( OSF_MIN5, MyFrame::OnMin)
	//EVT_MENU ( OSF_MIN7, MyFrame::OnMin)
	EVT_MENU ( OSF_MAX, MyFrame::OnMax)
	EVT_MENU ( OSF_MIDPOINT, MyFrame::OnMidpoint)
	EVT_MENU ( OSF_MEDIAN, MyFrame::OnMedian)
	// convolution routines
	EVT_MENU ( CNV_AVG_ID, MyFrame::OnCnvAvg3)
	EVT_MENU ( CNV_AVG3_ID, MyFrame::OnCnvAvg3)
	EVT_MENU ( CNV_AVG5_ID, MyFrame::OnCnvAvg5)
	EVT_MENU ( CNV_AVG7_ID, MyFrame::OnCnvAvg7)
	EVT_MENU ( CNV_AVG9_ID, MyFrame::OnCnvAvg9)
	EVT_MENU ( CNV_AVG11_ID, MyFrame::OnCnvAvg11)
	EVT_MENU ( CNV_WAVG_ID, MyFrame::OnCnvWAvg3)
	EVT_MENU ( CNV_WAVG3_ID, MyFrame::OnCnvWAvg3)
	EVT_MENU ( CNV_WAVG5_ID, MyFrame::OnCnvWAvg5)
	EVT_MENU ( CNV_WAVG7_ID, MyFrame::OnCnvWAvg7)
	EVT_MENU ( CNV_WAVG9_ID, MyFrame::OnCnvWAvg9)
	EVT_MENU ( CNV_WAVG11_ID, MyFrame::OnCnvWAvg11)
	EVT_MENU ( CNV_LP4_ID, MyFrame::OnCnvLP4)
	EVT_MENU ( CNV_LP4E_ID, MyFrame::OnCnvLP4E)
	EVT_MENU ( CNV_LP8_ID, MyFrame::OnCnvLP8)
	EVT_MENU ( CNV_LP8E_ID, MyFrame::OnCnvLP8E)
	EVT_MENU ( CNV_ROB_ID, MyFrame::OnCnvRob)
	EVT_MENU ( CNV_ROBA_ID, MyFrame::OnCnvRobAvg)
	EVT_MENU ( CNV_SOBX_ID, MyFrame::OnCnvSobX)
	EVT_MENU ( CNV_SOBY_ID, MyFrame::OnCnvSobY)
	EVT_MENU ( CNV_SOBDN_ID, MyFrame::OnCnvSobDiagN)
	EVT_MENU ( CNV_SOBDP_ID, MyFrame::OnCnvSobDiagP)
	EVT_MENU ( CNV_PREWX_ID, MyFrame::OnCnvPrewX)
	EVT_MENU ( CNV_PREWY_ID, MyFrame::OnCnvPrewY)
	EVT_MENU ( CNV_PREWDN_ID, MyFrame::OnCnvPrewDiagN)
	EVT_MENU ( CNV_PREWDP_ID, MyFrame::OnCnvPrewDiagP)
	EVT_MENU ( CNV_HISTO_ID, MyFrame::OnHistogram)
	EVT_MENU ( CNV_COLOUR_HISTO_ID, MyFrame::OnColourHistogram)
	EVT_MENU ( CNV_EQUALISE_ID, MyFrame::OnEqualise)
	EVT_MENU ( CNV_COLOUR_EQUALISE_ID, MyFrame::OnColourEqualise)
	EVT_MENU ( CNV_TEST_YUV_ID, MyFrame::OnTestYUVConversion)
	EVT_COMMAND_SCROLL(SLIDER_ID, MyFrame::OnSlide)
	EVT_PAINT (MyFrame::OnPaint)
	EVT_BUTTON(SCALE_BUT_OK, MyFrame::OnOperationOK)
END_EVENT_TABLE()

