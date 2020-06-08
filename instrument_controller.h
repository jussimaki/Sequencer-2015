#ifndef INSTRUMENT_CONTROLLER_H
#define INSTRUMENT_CONTROLLER_H

#include "windows.h"
#include "sample.h"
#include "instrument.h"

#include "file_window_controller.h"
#include "instrument_window.h"

class InstrumentLayout;
class Sequencer;

class InstrumentController
{
private:
	Windows *system_;

	Lock *instrumentSampleLock_, *sequencerModeLock_;

	Sequencer *sequencer_;
	InstrumentLayout *view_;

	std::vector <Instrument *> *instruments_;
	std::vector <Sample *> *samples_;

	unsigned int instrumentNumber_;

	// New instrument window
	std::string newInstrumentName_;
	

	// Instrument properties window
	std::string instrumentNewName_;
	unsigned int instrumentNewType_;

	// Sample mapping window
	unsigned int sampleNumber_;

	unsigned int rootNote_, lowNote_, highNote_;

	double attack_, decay_, sustain_, release_;
	
	int zoomX_;
	double zoomRatio_, zoomSpot_;

	FileWindowController loadSampleFileWindowController_;

	// Add sample window

	unsigned int sampleBankSampleNumber_;

	


public:

	InstrumentController(Windows *system, Sequencer *sequencer, std::vector <Instrument *> *instruments, std::vector <Sample *> *samples, Lock *instrumentSampleLock, Lock *sequencerModeLock) : system_(system),
		sequencer_(sequencer), instruments_(instruments), samples_(samples), instrumentSampleLock_(instrumentSampleLock), sequencerModeLock_(sequencerModeLock),
		view_(0),
		instrumentNumber_(0), sampleNumber_(0)

	{

		//loadSampleFileWindow_.clearCallBacks();
			
		loadSampleFileWindowController_.addCallBack(this, &InstrumentController::loadSampleCallback_);
	}

	void setView(InstrumentLayout *view) { view_ = view; }

	void openController();


	Sequencer *getSequencer() { return sequencer_; }

	// Instrument window
	unsigned int getInstrumentAmount() { return instruments_->size(); }
	unsigned int getInstrumentNumber() { return instrumentNumber_; }
	std::string getInstrumentName(unsigned int number) { return (*instruments_)[number]->getName(); }
	unsigned int getInstrumentType(unsigned int number) { return (*instruments_)[number]->getType(); }

	// New instrument window
	std::string getNewInstrumentName() { return newInstrumentName_; }
	
	// Instrument properties window
	std::string getInstrumentNewName() { return instrumentNewName_; }
	unsigned int getInstrumentNewType() { return instrumentNewType_; }

	// Sample mapping window

	FileWindowController *getLoadSampleFileWindowController() { return &loadSampleFileWindowController_; }

	unsigned int getSampleAmount(unsigned int instrumentNumber) { return (*instruments_)[instrumentNumber]->getSampleData()->size(); }
	std::string getSampleName(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].name; }

	unsigned int getSampleNumber() { return sampleNumber_; }

	unsigned int getSampleRootNote(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].rootNote; }
	unsigned int getSampleLowNote(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].lowNote; }
	unsigned int getSampleHighNote(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].highNote; }

	signed short *getSampleWaveData(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].sample->getData(); }
	unsigned int getSampleLength(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].sample->getLength(); }
	
	unsigned int getSampleLoopStart(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].loopStart; }
	unsigned int getSampleLoopEnd(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].loopEnd; }
	unsigned int getSampleLoopType(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].loopType; }

	double getSampleAttack(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].attack; }
	double getSampleDecay(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].decay; }
	double getSampleSustain(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].sustain; }
	double getSampleRelease(unsigned int instrumentNumber, unsigned int sampleNumber) { return (*(*instruments_)[instrumentNumber]->getSampleData())[sampleNumber].release; }

	
	void setZoom();
	int getZoomX() { return zoomX_; }
	double getZoomRatio() { return zoomRatio_; }
	double getZoomSpot() { return zoomSpot_; }

	// Add sample window
	unsigned int getSampleBankSampleAmount() { return samples_->size(); }
	std::string getSampleBankSampleName(unsigned int number) { return (*samples_)[number]->getName(); }
	unsigned int getSampleBankSampleNumber() { return sampleBankSampleNumber_; }
	

	// Callbacks

	void static instrumentWindowCallback_(void *object, Widget *widget, Event *event);
	void instrumentWindowCallback(Widget *widget, Event *event);

	void static newInstrumentWindowCallback_(void *object, Widget *widget, Event *event);
	void newInstrumentWindowCallback(Widget *widget, Event *event);

	void static deleteInstrumentWindowCallback_(void *object, Widget *widget, Event *event);
	void deleteInstrumentWindowCallback(Widget *widget, Event *event);

	void static instrumentPropertiesWindowCallback_(void *object, Widget *widget, Event *event);
	void instrumentPropertiesWindowCallback(Widget *widget, Event *event);

	void static sampleMappingWindowCallback_(void *object, Widget *widget, Event *event);
	void sampleMappingWindowCallback(Widget *widget, Event *event);

	void static addSampleWindowCallback_(void *object, Widget *widget, Event *event);
	void addSampleWindowCallback(Widget *widget, Event *event);

	void static loadSampleCallback_(void *object, FileWindowController *fileWindowController);
	void loadSampleCallback(FileWindowController *fileWindowController);

	void static removeSampleWindowCallback_(void *object, Widget *widget, Event *event);
	void removeSampleWindowCallback(Widget *widget, Event *event);

	void static sampleGFXWindowCallback_(void *object, Widget *widget, Event *event);
	void sampleGFXWindowCallback(Widget *widget, Event *event);

};

#endif