#ifndef INSTRUMENT_WINDOW_H
#define INSTRUMENT_WINDOW_H

#include <vector>
#include <string>

#include "window.h"
#include "windows.h"
#include "file_window_view.h"


#include "sample_window.h"
#include "instrument_controller.h"

class InstrumentController;


class InstrumentLayout
{
	friend class InstrumentController;
private:
	Windows *system_;

	InstrumentController *instrumentController_;
	
	//
	Window instrumentWindow_;

	Label instrumentLabel_;
	Button newInstrumentButton_, deleteInstrumentButton_, thirdButton_;
	List instrumentList_;
	Button instrumentPropertiesButton_;
	Button sampleMappingButton_;

	//
	Window newInstrumentWindow_;

	Label createNewInstrumentLabel_;
	Label newInstrumentNameLabel_;
	TextField newInstrumentName_;
	Button newInstrumentOkButton_;
	Button newInstrumentCancelButton_;

	//
	Window deleteInstrumentWindow_;

	Label deleteInstrumentLabel_;
	Button deleteInstrumentOkButton_;
	Button deleteInstrumentCancelButton_;

	//
	Window instrumentPropertiesWindow_;
	Label instrumentPropertiesLabel_;
	Label instrumentNameLabel_;
	TextField instrumentName_;
	Label instrumentTypeLabel_;
	RadioButtons instrumentType_;

	Button instrumentPropertiesOkButton_;
	Button instrumentPropertiesCancelButton_;

	//
	Window sampleMappingWindow_;
	Label sampleMappingInstrumentName_;
	Label samplesLabel_;
	List sampleList_;
	
	Button addSampleButton_;
	Button removeSampleButton_;
	Button sampleMappingOkButton_;
	Label sampleMappingSampleName_;
	Label sampleLength_;
	Label rootNoteLabel_, lowNoteLabel_, highNoteLabel_;
	DropMenu rootNoteMenu_, lowNoteMenu_, highNoteMenu_;

	Label loopTypeLabel_;
	RadioButtons loopType_;

	Label sampleAttackLabel_, sampleDecayLabel_, sampleSustainLabel_, sampleReleaseLabel_;
	TextField sampleAttack_, sampleDecay_, sampleSustain_, sampleRelease_;
	Button playSampleButton_;

	Button sampleZoomInButton_, sampleZoomOutButton_;
	Label loopStartLabel_, loopEndLabel_;
	TextField loopStart_, loopEnd_;

	GraphicsWindow sampleGFXWindow_;

	WindowSprite waveFormSprite_;
	WindowCanvas waveFormCanvas_;



	//
	Window addSampleWindow_;
	Label addSampleLabel_;
	List addSampleList_;
	Button addSampleOkButton_, addSampleCancelButton_, loadSampleButton_;

	FileWindowView loadSampleFileWindowView_;


	//
	Window removeSampleWindow_;
	Label removeSampleLabel_;
	Button removeSampleOkButton_, removeSampleCancelButton_;
	


	
	
	

public:
	InstrumentLayout(Windows *system, InstrumentController *instrumentController);

	void createInstrumentLayout();
	void configureInstrumentLayout();

	void openInstrumentWindow();
	void updateInstrumentWindow();

	void openNewInstrumentWindow();
	void updateNewInstrumentWindow();

	void openDeleteInstrumentWindow();
	
	void openInstrumentPropertiesWindow();

	void openSampleMappingWindow();
	void updateSampleMappingWindow();

	void openAddSampleWindow();
	void updateAddSampleWindow();

	void openRemoveSampleWindow();






	//void static instrumentButtonClick_(void *object, Widget *widget, Event *event);
	//void instrumentButtonClick(Widget *widget, Event *event);



		
	void static drawWaveForm_(void *object, GraphicsWindow *window, int width, int height);
	void drawWaveForm(GraphicsWindow *window, int width, int height);

};



#endif