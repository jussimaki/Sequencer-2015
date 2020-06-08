#include "instrument_window.h"
#include "sequencer.h"
#include <sstream>


InstrumentLayout::InstrumentLayout(Windows *system, InstrumentController *instrumentController) : system_(system), instrumentController_(instrumentController),
loadSampleFileWindowView_(system, instrumentController->getLoadSampleFileWindowController())
{
	system_ = system;
	instrumentController_ = instrumentController;
	instrumentController_->setView(this);

	createInstrumentLayout();
	configureInstrumentLayout();
}

void InstrumentLayout::createInstrumentLayout()
{
	// Instrument window

	instrumentWindow_.setX(120);
	instrumentWindow_.setY(120);
	instrumentWindow_.setWidth(600);
	instrumentWindow_.setHeight(500);
	
	instrumentWindow_.addRegion("InstrumentLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		400, ATTACH_NONE, "",
		20, ATTACH_NONE, "");
	
	instrumentWindow_.addRegion("InstrumentList",
		0, ATTACH_LEFT, "InstrumentLabel",
		10, ATTACH_BOTTOM, "InstrumentLabel",
		250, ATTACH_NONE, "",
		400, ATTACH_NONE, "");

	instrumentWindow_.addRegion("NewInstrumentButton",
		0, ATTACH_LEFT, "InstrumentList",
		10, ATTACH_BOTTOM, "InstrumentList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentWindow_.addRegion("DeleteInstrumentButton",
		10, ATTACH_RIGHT, "NewInstrumentButton",
		0, ATTACH_TOP, "NewInstrumentButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");
	
	instrumentWindow_.addRegion("InstrumentPropertiesButton",
		10, ATTACH_RIGHT, "InstrumentList",
		0, ATTACH_TOP, "InstrumentList",
		150, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentWindow_.addRegion("SampleMappingButton",
		0, ATTACH_LEFT, "InstrumentPropertiesButton",
		10, ATTACH_BOTTOM, "InstrumentPropertiesButton",
		150, ATTACH_NONE, "",
		30, ATTACH_NONE, "");
	
	

	// New instrument window
		
	newInstrumentWindow_.addRegion("CreateNewInstrumentLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentNameLabel",
		0, ATTACH_LEFT, "CreateNewInstrumentLabel",
		5, ATTACH_BOTTOM, "CreateNewInstrumentLabel",
		130, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentName",
		5, ATTACH_RIGHT, "NewInstrumentNameLabel",
		0, ATTACH_TOP, "NewInstrumentNameLabel",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");


	newInstrumentWindow_.addRegion("NewInstrumentOkButton",
		10, ATTACH_LEFT, "",
		5, ATTACH_BOTTOM, "NewInstrumentName",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	newInstrumentWindow_.addRegion("NewInstrumentCancelButton",
		10, ATTACH_RIGHT, "NewInstrumentOkButton",
		0, ATTACH_TOP, "NewInstrumentOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	

	// Delete instrument window
	
	deleteInstrumentWindow_.addRegion("DeleteInstrumentLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		420, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	deleteInstrumentWindow_.addRegion("DeleteInstrumentOkButton",
		0, ATTACH_LEFT, "DeleteInstrumentLabel",
		5, ATTACH_BOTTOM, "DeleteInstrumentLabel",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	deleteInstrumentWindow_.addRegion("DeleteInstrumentCancelButton",
		10, ATTACH_RIGHT, "DeleteInstrumentOkButton",
		0, ATTACH_TOP, "DeleteInstrumentOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	// Instrument properties window
	
	instrumentPropertiesWindow_.addRegion("InstrumentPropertiesLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	instrumentPropertiesWindow_.addRegion("InstrumentNameLabel",
		0, ATTACH_LEFT, "InstrumentPropertiesLabel",
		5, ATTACH_BOTTOM, "InstrumentPropertiesLabel",
		130, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	instrumentPropertiesWindow_.addRegion("InstrumentName",
		10, ATTACH_RIGHT, "InstrumentNameLabel",
		0, ATTACH_TOP, "InstrumentNameLabel",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");
	
	instrumentPropertiesWindow_.addRegion("InstrumentTypeLabel",
		0, ATTACH_LEFT, "InstrumentNameLabel",
		5, ATTACH_BOTTOM, "InstrumentNameLabel",
		130, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	instrumentPropertiesWindow_.addRegion("InstrumentTypeRadioButtons",
		10, ATTACH_RIGHT, "InstrumentTypeLabel",
		0, ATTACH_TOP, "InstrumentTypeLabel",
		300, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	instrumentPropertiesWindow_.addRegion("InstrumentPropertiesOkButton",
		0, ATTACH_LEFT, "InstrumentTypeLabel",
		5, ATTACH_BOTTOM, "InstrumentTypeLabel",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	instrumentPropertiesWindow_.addRegion("InstrumentPropertiesCancelButton",
		10, ATTACH_RIGHT, "InstrumentPropertiesOkButton",
		0, ATTACH_TOP, "InstrumentPropertiesOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");


	

	// Sample mapping window

	sampleMappingWindow_.addRegion("SampleMappingInstrumentName",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SamplesLabel",
		0, ATTACH_LEFT, "SampleMappingInstrumentName",
		5, ATTACH_BOTTOM, "SampleMappingInstrumentName",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleList",
		0, ATTACH_LEFT, "SamplesLabel",
		5, ATTACH_BOTTOM, "SamplesLabel",
		200, ATTACH_NONE, "",
		300, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("AddSampleButton",
		0, ATTACH_LEFT, "SampleList",
		10, ATTACH_BOTTOM, "SampleList",
		100, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("RemoveSampleButton",
		10, ATTACH_RIGHT, "AddSampleButton",
		0, ATTACH_TOP, "AddSampleButton",
		70, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleMappingOkButton",
		0, ATTACH_LEFT, "AddSampleButton",
		10, ATTACH_BOTTOM, "AddSampleButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleMappingSampleName",
		10, ATTACH_RIGHT, "SampleList",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleLengthLabel",
		0, ATTACH_LEFT, "SampleMappingSampleName",
		5, ATTACH_BOTTOM, "SampleMappingSampleName",
		300, ATTACH_NONE, "",
		25, ATTACH_NONE, "");



	sampleMappingWindow_.addRegion("RootNoteLabel",
		0, ATTACH_LEFT, "SampleLengthLabel",
		10, ATTACH_BOTTOM, "SampleLengthLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("RootNoteMenu",
		10, ATTACH_RIGHT, "RootNoteLabel",
		0, ATTACH_TOP, "RootNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LowNoteLabel",
		0, ATTACH_LEFT, "RootNoteLabel",
		10, ATTACH_BOTTOM, "RootNoteLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LowNoteMenu",
		10, ATTACH_RIGHT, "LowNoteLabel",
		0, ATTACH_TOP, "LowNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("HighNoteLabel",
		0, ATTACH_LEFT, "LowNoteLabel",
		10, ATTACH_BOTTOM, "LowNoteLabel",
		80, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("HighNoteMenu",
		10, ATTACH_RIGHT, "HighNoteLabel",
		0, ATTACH_TOP, "HighNoteLabel",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleAttackLabel",
		0, ATTACH_LEFT, "HighNoteLabel",
		10, ATTACH_BOTTOM, "HighNoteLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleAttack",
		10, ATTACH_RIGHT, "SampleAttackLabel",
		0, ATTACH_TOP, "SampleAttackLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleDecayLabel",
		0, ATTACH_LEFT, "SampleAttackLabel",
		10, ATTACH_BOTTOM, "SampleAttackLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleDecay",
		10, ATTACH_RIGHT, "SampleDecayLabel",
		0, ATTACH_TOP, "SampleDecayLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleSustainLabel",
		0, ATTACH_LEFT, "SampleDecayLabel",
		10, ATTACH_BOTTOM, "SampleDecayLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleSustain",
		10, ATTACH_RIGHT, "SampleSustainLabel",
		0, ATTACH_TOP, "SampleSustainLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleReleaseLabel",
		0, ATTACH_LEFT, "SampleSustainLabel",
		10, ATTACH_BOTTOM, "SampleSustainLabel",
		60, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleRelease",
		10, ATTACH_RIGHT, "SampleReleaseLabel",
		0, ATTACH_TOP, "SampleReleaseLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("PlaySampleButton",
		0, ATTACH_LEFT, "SampleReleaseLabel",
		10, ATTACH_BOTTOM, "SampleReleaseLabel",
		100, ATTACH_NONE, "",
		50, ATTACH_NONE, "");





	sampleMappingWindow_.addRegion("SampleZoomInButton",
		60, ATTACH_RIGHT, "RootNoteMenu",
		0, ATTACH_TOP, "RootNoteMenu",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleZoomOutButton",
		10, ATTACH_RIGHT, "SampleZoomInButton",
		0, ATTACH_TOP, "SampleZoomInButton",
		50, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopStartLabel",
		10, ATTACH_RIGHT, "SampleZoomOutButton",
		0, ATTACH_TOP, "SampleZoomOutButton",
		70, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopStart",
		5, ATTACH_RIGHT, "LoopStartLabel",
		0, ATTACH_TOP, "LoopStartLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopEndLabel",
		10, ATTACH_RIGHT, "LoopStart",
		0, ATTACH_TOP, "LoopStart",
		70, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopEnd",
		5, ATTACH_RIGHT, "LoopEndLabel",
		0, ATTACH_TOP, "LoopEndLabel",
		100, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopTypeLabel",
		0, ATTACH_LEFT, "SampleZoomInButton",
		10, ATTACH_BOTTOM, "SampleZoomInButton",
		40, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("LoopType",
		5, ATTACH_RIGHT, "LoopTypeLabel",
		0, ATTACH_TOP, "LoopTypeLabel",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	sampleMappingWindow_.addRegion("SampleGFXWindow",
		0, ATTACH_LEFT, "LoopTypeLabel",
		10, ATTACH_BOTTOM, "LoopTypeLabel",
		600, ATTACH_NONE, "",
		300, ATTACH_NONE, "");

	

	// Add sample window

	addSampleWindow_.addRegion("AddSampleLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		600, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	addSampleWindow_.addRegion("AddSampleList",
		0, ATTACH_LEFT, "AddSampleLabel",
		10, ATTACH_BOTTOM, "AddSampleLabel",
		200, ATTACH_NONE, "",
		200, ATTACH_NONE, "");

	addSampleWindow_.addRegion("AddSampleOkButton",
		0, ATTACH_LEFT, "AddSampleList",
		10, ATTACH_BOTTOM, "AddSampleList",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	addSampleWindow_.addRegion("AddSampleCancelButton",
		10, ATTACH_RIGHT, "AddSampleOkButton",
		0, ATTACH_TOP, "AddSampleOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	addSampleWindow_.addRegion("LoadSampleButton",
		10, ATTACH_RIGHT, "AddSampleCancelButton",
		0, ATTACH_TOP, "AddSampleCancelButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	

	
	// Remove sample window

	removeSampleWindow_.addRegion("RemoveSampleLabel",
		10, ATTACH_LEFT, "",
		10, ATTACH_TOP, "",
		200, ATTACH_NONE, "",
		25, ATTACH_NONE, "");

	removeSampleWindow_.addRegion("RemoveSampleOkButton",
		0, ATTACH_LEFT, "RemoveSampleLabel",
		10, ATTACH_BOTTOM, "RemoveSampleLabel",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");

	removeSampleWindow_.addRegion("RemoveSampleCancelButton",
		10, ATTACH_RIGHT, "RemoveSampleOkButton",
		0, ATTACH_TOP, "RemoveSampleOkButton",
		50, ATTACH_NONE, "",
		30, ATTACH_NONE, "");
	
}

void InstrumentLayout::configureInstrumentLayout()
{
	// InstrumentWindow

	instrumentWindow_.setWidget("InstrumentLabel", &instrumentLabel_);
	instrumentWindow_.setWidget("NewInstrumentButton", &newInstrumentButton_);
	instrumentWindow_.setWidget("DeleteInstrumentButton", &deleteInstrumentButton_);
	instrumentWindow_.setWidget("InstrumentList", &instrumentList_);
	instrumentWindow_.setWidget("InstrumentPropertiesButton", &instrumentPropertiesButton_);
	instrumentWindow_.setWidget("SampleMappingButton", &sampleMappingButton_);

	instrumentWindow_.setVScrollControlsActive(true);

	newInstrumentButton_.setName("New");
	deleteInstrumentButton_.setName("Delete");
	instrumentPropertiesButton_.setName("Instrument properties");
	sampleMappingButton_.setName("Sample mapping");

	instrumentList_.setVScrollControlsActive(true);
	instrumentList_.setHScrollControlsActive(true);
	
	newInstrumentButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentWindowCallback_));
	deleteInstrumentButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentWindowCallback_));
	instrumentPropertiesButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentWindowCallback_));
	sampleMappingButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentWindowCallback_));
	instrumentList_.chooseEvent_.add(instrumentController_, &(InstrumentController::instrumentWindowCallback_));


	// New instrument window
	newInstrumentWindow_.setWidget("CreateNewInstrumentLabel", &createNewInstrumentLabel_);
	newInstrumentWindow_.setWidget("NewInstrumentNameLabel", &newInstrumentNameLabel_);
	newInstrumentWindow_.setWidget("NewInstrumentName", &newInstrumentName_);

	newInstrumentWindow_.setWidget("NewInstrumentOkButton", &newInstrumentOkButton_);
	newInstrumentWindow_.setWidget("NewInstrumentCancelButton", &newInstrumentCancelButton_);

	newInstrumentName_.textChange_.add(instrumentController_, &(InstrumentController::newInstrumentWindowCallback_));
	newInstrumentName_.textEnter_.add(instrumentController_, &(InstrumentController::newInstrumentWindowCallback_));
	newInstrumentOkButton_.onClick_.add(instrumentController_, &(InstrumentController::newInstrumentWindowCallback_));
	newInstrumentCancelButton_.onClick_.add(instrumentController_, &(InstrumentController::newInstrumentWindowCallback_));

	createNewInstrumentLabel_.text_ = "Create a new instrument?";
	newInstrumentNameLabel_.text_ = "Instrument name:";

	newInstrumentOkButton_.setName("Ok");
	newInstrumentCancelButton_.setName("Cancel");

	// Delete instrument window

	deleteInstrumentWindow_.setWidget("DeleteInstrumentLabel", &deleteInstrumentLabel_);
	deleteInstrumentWindow_.setWidget("DeleteInstrumentOkButton", &deleteInstrumentOkButton_);
	deleteInstrumentWindow_.setWidget("DeleteInstrumentCancelButton", &deleteInstrumentCancelButton_);

	deleteInstrumentOkButton_.setName("Ok");
	deleteInstrumentCancelButton_.setName("Cancel");

	deleteInstrumentOkButton_.onClick_.add(instrumentController_, &(InstrumentController::deleteInstrumentWindowCallback_));
	deleteInstrumentCancelButton_.onClick_.add(instrumentController_, &(InstrumentController::deleteInstrumentWindowCallback_));

	// Instrument properties window

	instrumentPropertiesWindow_.setWidget("InstrumentPropertiesLabel", &instrumentPropertiesLabel_);
	instrumentPropertiesWindow_.setWidget("InstrumentNameLabel", &instrumentNameLabel_);
	instrumentPropertiesWindow_.setWidget("InstrumentName", &instrumentName_);
	instrumentPropertiesWindow_.setWidget("InstrumentTypeLabel", &instrumentTypeLabel_);
	instrumentPropertiesWindow_.setWidget("InstrumentTypeRadioButtons", &instrumentType_);

	instrumentPropertiesWindow_.setWidget("InstrumentPropertiesOkButton", &instrumentPropertiesOkButton_);
	instrumentPropertiesWindow_.setWidget("InstrumentPropertiesCancelButton", &instrumentPropertiesCancelButton_);


	instrumentPropertiesLabel_.text_ = "Instrument properties:";
	instrumentNameLabel_.text_ = "Instrument name:";
	instrumentTypeLabel_.text_ = "Instument type:";

	instrumentType_.add("Melody");
	instrumentType_.add("Drums");

	instrumentPropertiesOkButton_.setName("Ok");
	instrumentPropertiesCancelButton_.setName("Cancel");

	instrumentName_.textChange_.add(instrumentController_, &(InstrumentController::instrumentPropertiesWindowCallback_));
	instrumentType_.chooseEvent_.add(instrumentController_, &(InstrumentController::instrumentPropertiesWindowCallback_));
	instrumentPropertiesOkButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentPropertiesWindowCallback_));
	instrumentPropertiesCancelButton_.onClick_.add(instrumentController_, &(InstrumentController::instrumentPropertiesWindowCallback_));

	// Sample mapping window

	sampleMappingWindow_.setWidget("SampleMappingInstrumentName", &sampleMappingInstrumentName_);
	sampleMappingWindow_.setWidget("SamplesLabel", &samplesLabel_);
	sampleMappingWindow_.setWidget("SampleList", &sampleList_);
	sampleMappingWindow_.setWidget("AddSampleButton", &addSampleButton_);
	sampleMappingWindow_.setWidget("RemoveSampleButton", &removeSampleButton_);
	sampleMappingWindow_.setWidget("SampleMappingOkButton", &sampleMappingOkButton_);

	sampleMappingWindow_.setWidget("SampleMappingSampleName", &sampleMappingSampleName_);
	sampleMappingWindow_.setWidget("SampleLengthLabel", &sampleLength_);
	sampleMappingWindow_.setWidget("SampleZoomInButton", &sampleZoomInButton_);
	sampleMappingWindow_.setWidget("SampleZoomOutButton", &sampleZoomOutButton_);
	sampleMappingWindow_.setWidget("LoopStartLabel", &loopStartLabel_);
	sampleMappingWindow_.setWidget("LoopStart", &loopStart_);
	sampleMappingWindow_.setWidget("LoopEndLabel", &loopEndLabel_);
	sampleMappingWindow_.setWidget("LoopEnd", &loopEnd_);


	sampleMappingWindow_.setWidget("RootNoteLabel", &rootNoteLabel_);
	sampleMappingWindow_.setWidget("LowNoteLabel", &lowNoteLabel_);
	sampleMappingWindow_.setWidget("HighNoteLabel", &highNoteLabel_);
	sampleMappingWindow_.setWidget("RootNoteMenu", &rootNoteMenu_);
	sampleMappingWindow_.setWidget("LowNoteMenu", &lowNoteMenu_);
	sampleMappingWindow_.setWidget("HighNoteMenu", &highNoteMenu_);
	sampleMappingWindow_.setWidget("LoopTypeLabel", &loopTypeLabel_);
	sampleMappingWindow_.setWidget("LoopType", &loopType_);
	sampleMappingWindow_.setWidget("SampleAttackLabel", &sampleAttackLabel_);
	sampleMappingWindow_.setWidget("SampleAttack", &sampleAttack_);
	sampleMappingWindow_.setWidget("SampleDecayLabel", &sampleDecayLabel_);
	sampleMappingWindow_.setWidget("SampleDecay", &sampleDecay_);
	sampleMappingWindow_.setWidget("SampleSustainLabel", &sampleSustainLabel_);
	sampleMappingWindow_.setWidget("SampleSustain", &sampleSustain_);
	sampleMappingWindow_.setWidget("SampleReleaseLabel", &sampleReleaseLabel_);
	sampleMappingWindow_.setWidget("SampleRelease", &sampleRelease_);

	sampleMappingWindow_.setWidget("PlaySampleButton", &playSampleButton_);

	sampleMappingWindow_.setWidget("SampleGFXWindow", &sampleGFXWindow_);


	samplesLabel_.text_ = "Samples:";
	addSampleButton_.setName("Add sample");
	removeSampleButton_.setName("Remove");
	sampleMappingOkButton_.setName("Ok");

	rootNoteLabel_.text_ = "Root note:";
	lowNoteLabel_.text_ = "Low note:";
	highNoteLabel_.text_ = "High note:";

	loopTypeLabel_.text_ = "Loop:";

	loopType_.clear();
	loopType_.add("Off");
	loopType_.add("On");
	loopType_.add("Pingpong");

	loopType_.chooseEvent_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);

	sampleAttackLabel_.text_ = "Attack:";
	sampleDecayLabel_.text_ = "Decay:";
	sampleSustainLabel_.text_ = "Sustain:";
	sampleReleaseLabel_.text_ = "Release:";

	playSampleButton_.setName("Play");
	playSampleButton_.onClick_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	

	loopStartLabel_.text_ = "Loop start:";
	loopEndLabel_.text_ = "Loop end:";

	sampleList_.setHScrollControlsActive(true);
	sampleList_.setVScrollControlsActive(true);
	sampleList_.chooseEvent_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);


	addSampleButton_.onClick_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	removeSampleButton_.onClick_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	sampleMappingOkButton_.onClick_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	rootNoteMenu_.chooseEvent_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	lowNoteMenu_.chooseEvent_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);
	highNoteMenu_.chooseEvent_.add(instrumentController_, &InstrumentController::sampleMappingWindowCallback_);

	
	
	sampleZoomInButton_.setName("Z+");
	sampleZoomOutButton_.setName("Z-");

	sampleZoomInButton_.onClick_.add(instrumentController_, &InstrumentController::sampleGFXWindowCallback_);
	sampleZoomOutButton_.onClick_.add(instrumentController_, &InstrumentController::sampleGFXWindowCallback_);
	
	sampleGFXWindow_.addDrawFunction(this, InstrumentLayout::drawWaveForm_);

	sampleGFXWindow_.onClick_.add(instrumentController_, &InstrumentController::sampleGFXWindowCallback_);
	sampleGFXWindow_.setHScrollControlsActive(true);
	
	waveFormSprite_ = system_->getEmptyWindowSprite(600, 300);
	waveFormCanvas_ = system_->getWindowCanvas(waveFormSprite_);
	
	
	// Add sample window

	addSampleWindow_.setWidget("AddSampleLabel", &addSampleLabel_);
	addSampleWindow_.setWidget("AddSampleList", &addSampleList_);
	addSampleWindow_.setWidget("AddSampleOkButton", &addSampleOkButton_);
	addSampleWindow_.setWidget("AddSampleCancelButton", &addSampleCancelButton_);
	addSampleWindow_.setWidget("LoadSampleButton", &loadSampleButton_);

	
	addSampleLabel_.text_ = "Add a sample?";
	addSampleList_.setVScrollControlsActive(true);
	addSampleList_.chooseEvent_.add(instrumentController_, &InstrumentController::addSampleWindowCallback_);

	addSampleOkButton_.setName("Ok");
	addSampleOkButton_.onClick_.add(instrumentController_, &InstrumentController::addSampleWindowCallback_);

	addSampleCancelButton_.setName("Cancel");
	addSampleCancelButton_.onClick_.add(instrumentController_, &InstrumentController::addSampleWindowCallback_);

	loadSampleButton_.setName("Load");
	loadSampleButton_.onClick_.add(instrumentController_, &InstrumentController::addSampleWindowCallback_);

	// Remove sample window

	removeSampleWindow_.setWidget("RemoveSampleLabel", &removeSampleLabel_);
	removeSampleWindow_.setWidget("RemoveSampleOkButton", &removeSampleOkButton_);
	removeSampleWindow_.setWidget("RemoveSampleCancelButton", &removeSampleCancelButton_);

	removeSampleOkButton_.setName("Ok");
	removeSampleCancelButton_.setName("Cancel");

	removeSampleOkButton_.onClick_.add(instrumentController_, &InstrumentController::removeSampleWindowCallback_);
	removeSampleCancelButton_.onClick_.add(instrumentController_, &InstrumentController::removeSampleWindowCallback_);


}

void InstrumentLayout::openInstrumentWindow()
{
	updateInstrumentWindow();
	system_->addWindow(&instrumentWindow_);
}


void InstrumentLayout::updateInstrumentWindow()
{
	std::string string;
	std::stringstream stream;


	// Set instrument name label
	unsigned int instrumentAmount = instrumentController_->getInstrumentAmount();
	unsigned int instrumentNumber = instrumentController_->getInstrumentNumber();

	if (instrumentAmount > 0)
	{
		string = "Instrument: ";
		string += instrumentController_->getInstrumentName(instrumentNumber);
	}
	else
	{
		string = "No instruments";
	}
	
	instrumentLabel_.text_ = string;

	// Set instrument list
	instrumentList_.clear();
	
	for (unsigned int index = 0; index < instrumentAmount; index++)
	{
		instrumentList_.add(instrumentController_->getInstrumentName(index));
	}

	
	instrumentList_.setRow(instrumentController_->getInstrumentNumber());

	
	if (instrumentAmount == 0)
	{
		instrumentPropertiesButton_.setDisabled(true);
		deleteInstrumentButton_.setDisabled(true);
		sampleMappingButton_.setDisabled(true);
	}
	else
	{
		instrumentPropertiesButton_.setDisabled(false);
		deleteInstrumentButton_.setDisabled(false);
		sampleMappingButton_.setDisabled(false);
	}
}

void InstrumentLayout::openNewInstrumentWindow()
{
	newInstrumentName_.activate();
	newInstrumentName_.setText(instrumentController_->getNewInstrumentName());


	newInstrumentWindow_.setX(instrumentWindow_.getX() + 50);
	newInstrumentWindow_.setY(instrumentWindow_.getY() + 50);
	newInstrumentWindow_.setWidth(500);
	newInstrumentWindow_.setHeight(200);

	system_->addWindow(&newInstrumentWindow_);
}


void InstrumentLayout::updateNewInstrumentWindow()
{
	newInstrumentName_.setText(instrumentController_->getNewInstrumentName());
}

void InstrumentLayout::openDeleteInstrumentWindow()
{
	
	deleteInstrumentLabel_.text_ = "Delete instrument \"" + instrumentController_->getInstrumentName(instrumentController_->getInstrumentNumber()) +"\"?";

	deleteInstrumentWindow_.setX(instrumentWindow_.getX() + 50);
	deleteInstrumentWindow_.setY(instrumentWindow_.getY() + 50);
	deleteInstrumentWindow_.setWidth(450);
	deleteInstrumentWindow_.setHeight(100);
		
	system_->addWindow(&deleteInstrumentWindow_);
}

void InstrumentLayout::openInstrumentPropertiesWindow()
{
	instrumentName_.setText(instrumentController_->getInstrumentNewName());
	instrumentType_.setButtonNumber(instrumentController_->getInstrumentNewType());

	instrumentPropertiesWindow_.setX(instrumentWindow_.getX() + 50);
	instrumentPropertiesWindow_.setY(instrumentWindow_.getY() + 50);
	instrumentPropertiesWindow_.setWidth(450);
	instrumentPropertiesWindow_.setHeight(300);
	
	system_->addWindow(&instrumentPropertiesWindow_);
}

void InstrumentLayout::openSampleMappingWindow()
{
	unsigned int instrumentNumber = instrumentController_->getInstrumentNumber();
	unsigned int sampleAmount = instrumentController_->getSampleAmount(instrumentNumber);
	
	sampleMappingInstrumentName_.text_ = "Instrument: " + instrumentController_->getInstrumentName(instrumentNumber);


	updateSampleMappingWindow();
	
	sampleMappingWindow_.setX(instrumentWindow_.getX() + 50);
	sampleMappingWindow_.setY(instrumentWindow_.getY() + 50);
	sampleMappingWindow_.setWidth(1200);
	sampleMappingWindow_.setHeight(500);

	system_->addWindow(&sampleMappingWindow_);
}

void InstrumentLayout::updateSampleMappingWindow()
{
	unsigned int instrumentNumber = instrumentController_->getInstrumentNumber();
	unsigned int sampleAmount = instrumentController_->getSampleAmount(instrumentNumber);

	sampleList_.clear();

	for (unsigned int sampleIndex = 0; sampleIndex < sampleAmount; sampleIndex++)
	{
		sampleList_.add(instrumentController_->getSampleName(instrumentNumber, sampleIndex));
	}

	rootNoteMenu_.clear();
	lowNoteMenu_.clear();
	highNoteMenu_.clear();

	if (sampleAmount > 0)
	{
		unsigned int sampleNumber = instrumentController_->getSampleNumber();
		unsigned int sampleLength = instrumentController_->getSampleLength(instrumentNumber, sampleNumber);

		sampleList_.setRow(sampleNumber);

		for (int index = 0; index < 128; index++)
		{
			int note = index % 12;
			int octave = index / 12;

			std::string noteString;

			switch (note)
			{
			case 0:
				noteString = "c";
				break;
			case 1:
				noteString = "c#";
				break;
			case 2:
				noteString = "d";
				break;
			case 3:
				noteString = "d#";
				break;
			case 4:
				noteString = "e";
				break;
			case 5:
				noteString = "f";
				break;
			case 6:
				noteString = "f#";
				break;
			case 7:
				noteString = "g";
				break;
			case 8:
				noteString = "g#";
				break;
			case 9:
				noteString = "a";
				break;
			case 10:
				noteString = "a#";
				break;
			case 11:
				noteString = "b";
				break;
			default: break;
			}

			std::stringstream stream;
			stream << noteString;
			stream << "-";
			stream << octave;


			std::string string;
			stream >> string;

			rootNoteMenu_.add(string);
			lowNoteMenu_.add(string);
			highNoteMenu_.add(string);
		}

		rootNoteMenu_.setDisabled(false);
		lowNoteMenu_.setDisabled(false);
		highNoteMenu_.setDisabled(false);

		rootNoteMenu_.setRow(instrumentController_->getSampleRootNote(instrumentNumber, sampleNumber));
		lowNoteMenu_.setRow(instrumentController_->getSampleLowNote(instrumentNumber, sampleNumber));
		highNoteMenu_.setRow(instrumentController_->getSampleHighNote(instrumentNumber, sampleNumber));

		removeSampleButton_.setDisabled(false);

		sampleMappingSampleName_.text_ = "Sample: " + instrumentController_->getSampleName(instrumentNumber, sampleNumber);

		std::stringstream stream;
		stream << sampleLength;
		std::string string;
		stream >> string;
		sampleLength_.text_ = "Length: " + string;

		sampleZoomInButton_.setDisabled(false);
		sampleZoomOutButton_.setDisabled(false);

		stream.clear();
		stream << instrumentController_->getSampleLoopStart(instrumentNumber, sampleNumber);
		stream >> string;
		loopStart_.setText(string);

		stream.clear();
		stream << instrumentController_->getSampleLoopEnd(instrumentNumber, sampleNumber);
		stream >> string;
		loopEnd_.setText(string);

		loopStart_.setDisabled(false);
		loopEnd_.setDisabled(false);

		stream.clear();
		stream << instrumentController_->getSampleAttack(instrumentNumber, sampleNumber);
		stream >> string;
		sampleAttack_.setText(string);

		stream.clear();
		stream << instrumentController_->getSampleDecay(instrumentNumber, sampleNumber);
		stream >> string;
		sampleDecay_.setText(string);

		stream.clear();
		stream << instrumentController_->getSampleSustain(instrumentNumber, sampleNumber);
		stream >> string;
		sampleSustain_.setText(string);

		stream.clear();
		stream << instrumentController_->getSampleRelease(instrumentNumber, sampleNumber);
		stream >> string;
		sampleRelease_.setText(string);

		sampleAttack_.setDisabled(false);
		sampleDecay_.setDisabled(false);
		sampleSustain_.setDisabled(false);
		sampleRelease_.setDisabled(false);

		playSampleButton_.setDisabled(false);

		loopType_.setButtonNumber(instrumentController_->getSampleLoopType(instrumentNumber, sampleNumber));
		loopType_.setDisabled(false);


		sampleGFXWindow_.setDisabled(false);
		
	}
	else
	{
		rootNoteMenu_.setDisabled(true);
		lowNoteMenu_.setDisabled(true);
		highNoteMenu_.setDisabled(true);

		removeSampleButton_.setDisabled(true);

		sampleMappingSampleName_.text_ = "No samples";
		sampleLength_.text_ = "";

		sampleZoomInButton_.setDisabled(true);
		sampleZoomOutButton_.setDisabled(true);

		loopStart_.setText("");
		loopEnd_.setText("");

		loopStart_.setDisabled(true);
		loopEnd_.setDisabled(true);

		sampleAttack_.setText("");
		sampleDecay_.setText("");
		sampleSustain_.setText("");
		sampleRelease_.setText("");

		sampleAttack_.setDisabled(true);
		sampleDecay_.setDisabled(true);
		sampleSustain_.setDisabled(true);
		sampleRelease_.setDisabled(true);

		playSampleButton_.setDisabled(true);

		loopType_.setButtonNumber(0);

		loopType_.setDisabled(true);

		sampleGFXWindow_.setDisabled(true);

	}

}

void InstrumentLayout::openAddSampleWindow()
{
	addSampleWindow_.setX(sampleMappingWindow_.getX() + 50);
	addSampleWindow_.setY(sampleMappingWindow_.getY() + 50);
	addSampleWindow_.setWidth(700);
	addSampleWindow_.setHeight(400);
	
	updateAddSampleWindow();

	system_->addWindow(&addSampleWindow_);
}


void InstrumentLayout::updateAddSampleWindow()
{
	unsigned int sampleBankSampleAmount = instrumentController_->getSampleBankSampleAmount();

	addSampleList_.clear();

	for (int sampleIndex = 0; sampleIndex < sampleBankSampleAmount; sampleIndex++)
	{
		addSampleList_.add(instrumentController_->getSampleBankSampleName(sampleIndex));
	}

	unsigned int sampleBankSampleNumber = instrumentController_->getSampleBankSampleNumber();
	addSampleList_.setRow(sampleBankSampleNumber);


	if (sampleBankSampleAmount == 0) addSampleOkButton_.setDisabled(true);
	else addSampleOkButton_.setDisabled(false);
}

void InstrumentLayout::openRemoveSampleWindow()
{
	removeSampleWindow_.setX(sampleMappingWindow_.getX() + 50);
	removeSampleWindow_.setY(sampleMappingWindow_.getY() + 50);
	removeSampleWindow_.setWidth(300);
	removeSampleWindow_.setHeight(200);

	unsigned int instrumentNumber = instrumentController_->getInstrumentNumber();
	unsigned int sampleNumber = instrumentController_->getSampleNumber();

	removeSampleLabel_.text_ = "Remove sample \"" + instrumentController_->getSampleName(instrumentNumber, sampleNumber) + "\"?";

	system_->addWindow(&removeSampleWindow_);
}

void InstrumentLayout::drawWaveForm(GraphicsWindow *window, int width, int height)
{
	
	unsigned int instrumentNumber = instrumentController_->getInstrumentNumber();
	unsigned int sampleNumber = instrumentController_->getSampleNumber();

	unsigned int sampleLength = instrumentController_->getSampleLength(instrumentNumber, sampleNumber);
	signed short *wave = instrumentController_->getSampleWaveData(instrumentNumber, sampleNumber);
	

	Rect dest;
	dest.x = window->getScrollX();
	dest.y = 0;
	dest.width = width;
	dest.height = height;

	unsigned char *data = waveFormCanvas_.data;
	for (int index = 0; index < waveFormSprite_.width*waveFormSprite_.height; index++)
	{
		data[index * 4] = 0;
		data[index * 4 + 1] = 0;
		data[index * 4 + 2] = 0;
		data[index * 4 + 3] = 255;
	}
	

	double zoomRatio = instrumentController_->getZoomRatio();
	double zoomSpot = instrumentController_->getZoomSpot();

	sampleGFXWindow_.setScrollWidth((double)(sampleLength) / zoomRatio);


	
	int zoomX = ((double)zoomSpot) / zoomRatio - sampleGFXWindow_.getScrollX();

	if (zoomX >= 0 && zoomX < waveFormSprite_.width)
	for (int y = 0; y < waveFormSprite_.height; y++)
	{
		data[(y*waveFormSprite_.width + zoomX) * 4] = 255;
		data[(y*waveFormSprite_.width + zoomX) * 4 + 1] = 0;
		data[(y*waveFormSprite_.width + zoomX) * 4 + 2] = 255;
		data[(y*waveFormSprite_.width + zoomX) * 4 + 3] = 255;
	}

		
	signed short lastValue = wave[(int)(window->getScrollX()*zoomRatio)];

		
	
	for (int x = 0; x < waveFormCanvas_.width; x++)
	{
		signed short fastValue;

		
		//signed short value = wave[(x*(length - 1)) / (waveFormCanvas_->width - 1)];
		signed short value = wave[(int)((x + window->getScrollX())*zoomRatio)];

		int y1 = (waveFormSprite_.height / 2) - (int)(((double)value / (double)32768) * (double)((waveFormSprite_.height / 2) - 5));
		int y2 = (waveFormSprite_.height / 2) - (int)(((double)lastValue / (double)32768) * (double)((waveFormSprite_.height / 2) - 5));
		
		
		if (y1 > y2)
		{
			int temp = y1;
			y1 = y2;
			y2 = temp;
		}

		int red = 255;
		int green = 0;
		int blue = 0;

		
		if (x == zoomX)
		{
			red = 255; green = 255;  blue = 255;
		}
		for (int y = y1; y <= y2; y++)
		{
			data[(y*waveFormSprite_.width + x) * 4] = red;
			data[(y*waveFormSprite_.width + x) * 4 +1] = green;
			data[(y*waveFormSprite_.width + x) * 4 +2] = blue;
			data[(y*waveFormSprite_.width + x) * 4 +3] = 255;
		}
		
		lastValue = value;
	}


	system_->attachWindowCanvas(waveFormCanvas_);
	window->drawWindowSprite(waveFormSprite_, &dest, 0);
	
}

void InstrumentLayout::drawWaveForm_(void *object, GraphicsWindow *window, int width, int height)
{
	InstrumentLayout *p = (InstrumentLayout*)object;
	p->drawWaveForm(window, width, height);
}



