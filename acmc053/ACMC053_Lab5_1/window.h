#include "masks.h"
#include "Histogram.h"
class BasicApplication: public wxApp {
public:
	virtual bool OnInit();
};


class MyFrame: public wxFrame {
protected:
	wxMenuBar *menuBar;//main menu bar
	wxMenu *fileMenu;//file menu
	wxMenu *processMenu;//process menu
	wxMenu *convolveMenu;//convlove menu
	wxMenu *editMenu;//edit menu
	wxMenu *roiMenu;//ROI menu
	wxMenuItem *roiMenuShow;//ROI menu show
	wxMenu *osfMenu;//OSF menu

	wxBitmap *back_bitmap; // offscreen memory buffer for drawing
	wxToolBar *toolbar;//toolbar not necessary to use
	wxSlider *slider;
	wxScrolledWindow *scrollArea;

	wxImage *loadedImage;
	wxFrame *popupFrame;
	Mask *currentMatrix;
	//float currentMultiplier;
	/* Add protected data structure to handle image data and control the drawing process */
	wxImage *workingImage;
	wxImage *undoImage;
	wxImage *previewImage;

	Histogram *histoGram;

	int oldWidth, oldHeight; // save old dimensions
	float sliderScaleFactor;
	bool sliderSemaphore;
	bool showHist;
	bool showROI;
	bool isROISet;
	float roiLow,roiHigh;
	int changesPending;

	int imgWidth, imgHeight;
	unsigned char* imgBuf, *workingImgBuf, *undoImgBuf, *tmpImgBuf,*previewImgBuf,*histImgBuf,*roiImgBuf;
	int stuffToDraw;

public:
	MyFrame(const wxChar *title, int xpos, int ypos, int width, int height);
	virtual ~MyFrame();

	void OnExit(wxCommandEvent & event);
	void OnOpenFile(wxCommandEvent & event);
	void OnRevert(wxCommandEvent & event);
	void OnSaveFile(wxCommandEvent & event);
	void OnScale(wxCommandEvent & event);
	void OnShift(wxCommandEvent & event);
	void OnUndo(wxCommandEvent & event);
	void OnPaint(wxPaintEvent & event);
	void OnInvertImage(wxCommandEvent & event);
	void OnClosePopup(wxCloseEvent& event);
	void OnSlide(wxScrollEvent & event);
	void OnOperationOK(wxCommandEvent & event);
	void OnHighThreshold(wxCommandEvent & event);
	void OnLowThreshold(wxCommandEvent & event);
	void OnConvertGrey(wxCommandEvent & event);
	//ROI
	void OnROIHighThreshold(wxCommandEvent & event);
	void OnROILowThreshold(wxCommandEvent & event);
	void OnROIShowHide(wxCommandEvent & event);
	void OnROIClear(wxCommandEvent & event);
	//average
	void OnCnvAvg3(wxCommandEvent& event);
	void OnCnvAvg5(wxCommandEvent& event);
	void OnCnvAvg7(wxCommandEvent& event);
	void OnCnvAvg9(wxCommandEvent& event);
	void OnCnvAvg11(wxCommandEvent& event);
	// weighted average
	void OnCnvWAvg3(wxCommandEvent& event);
	void OnCnvWAvg5(wxCommandEvent& event);
	void OnCnvWAvg7(wxCommandEvent& event);
	void OnCnvWAvg9(wxCommandEvent& event);
	void OnCnvWAvg11(wxCommandEvent& event);
	void OnCnvLP4(wxCommandEvent& event);
	void OnCnvLP4E(wxCommandEvent& event);
	void OnCnvLP8(wxCommandEvent& event);
	void OnCnvLP8E(wxCommandEvent& event);
	void OnCnvRob(wxCommandEvent& event);
	void OnCnvRobAvg(wxCommandEvent& event);
	void OnCnvSobX(wxCommandEvent& event);
	void OnCnvSobY(wxCommandEvent& event);
	void OnCnvSobDiagN(wxCommandEvent& event);
	void OnCnvSobDiagP(wxCommandEvent& event);
	void OnCnvPrewX(wxCommandEvent& event);
	void OnCnvPrewY(wxCommandEvent& event);
	void OnCnvPrewDiagN(wxCommandEvent& event);
	void OnCnvPrewDiagP(wxCommandEvent& event);
	void OnHistogram(wxCommandEvent & event);
	void OnEqualise(wxCommandEvent & event);
	void OnColourHistogram(wxCommandEvent & event);
	void OnColourEqualise(wxCommandEvent & event);
	void OnTestYUVConversion(wxCommandEvent & event);
	//OSF
	void OnMin(wxCommandEvent & event);
	void OnMax(wxCommandEvent & event);
	void OnMidpoint(wxCommandEvent & event);
	void OnMedian(wxCommandEvent & event);
	// operations
	void doAvg(wxCommandEvent & event,int size);
	void doWAvg(wxCommandEvent & event,int size);
	void doConvlove(wxCommandEvent & event);
	void makeSliderPopup(wxString);
	void scaleImage(float *f);
	void shiftImage(float *f);
	void convolve(float *f);
	void checkImageLoaded(wxCommandEvent & event);
	void lowThreshold(float *f);
	void highThreshold(float *f);
	void doROIHigh(float *f);
	void doROILow(float *f);
	void genROI();
	void clearROI();
	bool checkInROI(int x,int y);

	unsigned char*  copy(unsigned char* src);
	void (MyFrame::*processingOperation)(float*);

DECLARE_EVENT_TABLE()void commitChanges();
    ;
};

enum {
	EXIT_ID = wxID_HIGHEST + 1,
	LOAD_FILE_ID,
	INVERT_IMAGE_ID,
	SAVE_FILE_ID,
	REVERT_FILE_ID,
	CONVERT_GREY_ID,
	SCALE_ID,
	SHIFT_ID,
	THRESH_LOW_ID,
	THRESH_HIGH_ID,
	ROI_THRESH_LOW_ID,
	ROI_THRESH_HIGH_ID,
	ROI_SHOW,
	ROI_CLEAR,
	UNDO_ID,
	SLIDER_ID,
	SCALE_BUT_OK,
	POP_ID,
	SCROLL_ID,
	OSF_MIN,
	OSF_MIN3,
	OSF_MIN5,
	OSF_MIN7,
	OSF_MAX,
	OSF_MIDPOINT,
	OSF_MEDIAN,
	// convolution matricies menu items
	CNV_AVG_ID,
	CNV_AVG3_ID,
	CNV_AVG5_ID,
	CNV_AVG7_ID,
	CNV_AVG9_ID,
	CNV_AVG11_ID,
	CNV_WAVG_ID,
	CNV_WAVG3_ID,
	CNV_WAVG5_ID,
	CNV_WAVG7_ID,
	CNV_WAVG9_ID,
	CNV_WAVG11_ID,
	CNV_LP4_ID,
	CNV_LP8_ID,
	CNV_LP4E_ID,
	CNV_LP8E_ID,
	CNV_ROB_ID,
	CNV_ROBA_ID,
	CNV_SOBX_ID,
	CNV_SOBY_ID,
	CNV_SOBDP_ID,
	CNV_SOBDN_ID,
	CNV_PREWX_ID,
	CNV_PREWY_ID,
	CNV_PREWDP_ID,
	CNV_PREWDN_ID,
	CNV_HISTO_ID,
	CNV_EQUALISE_ID,
	CNV_COLOUR_HISTO_ID,
	CNV_COLOUR_EQUALISE_ID,
	CNV_TEST_YUV_ID,
};

/* Define const for control the drawing */
enum {
	NOTHING = 0, ORIGINAL_IMG=1, WORKING_IMG=2,PREVIEW_IMG=3,
};
