#include "instrument_controller.h"
#include "sequencer.h"

// open controller
void InstrumentController::openController()
{
	if (instruments_->size() > 0) sequencer_->setInstrumentPlayingOn((*instruments_)[instrumentNumber_]);
	else sequencer_->setInstrumentPlayingOff();
}

// Instrument window
void InstrumentController::instrumentWindowCallback(Widget *widget, Event *event)
{
	
	if (event == &(view_->newInstrumentButton_.onClick_))
	{
		newInstrumentName_ = "";
				
		view_->openNewInstrumentWindow();
	}
	else if (event == &(view_->deleteInstrumentButton_.onClick_))
	{
		view_->openDeleteInstrumentWindow();
	}
	else if (event == &(view_->instrumentPropertiesButton_.onClick_))
	{
		instrumentNewName_ = (*instruments_)[instrumentNumber_]->getName();
		instrumentNewType_ = (*instruments_)[instrumentNumber_]->getType();

		view_->openInstrumentPropertiesWindow();
	}
	else if (event == &(view_->sampleMappingButton_.onClick_))
	{
		if (getInstrumentType(instrumentNumber_) == 0)
		{
			sampleNumber_ = 0;

			setZoom();

			view_->openSampleMappingWindow();
		}
	}
	else if (event == &(view_->instrumentList_.chooseEvent_))
	{
		instrumentNumber_ = view_->instrumentList_.chooseEvent_.getNumber();

		sequencer_->setInstrumentPlayingOn((*instruments_)[instrumentNumber_]); // set instrumentplaying on

		view_->updateInstrumentWindow();
	}
}

void InstrumentController::instrumentWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->instrumentWindowCallback(widget, event);
}


// New instrument window
void InstrumentController::newInstrumentWindowCallback(Widget *widget, Event *event)
{
	if (event == &(view_->newInstrumentOkButton_.onClick_))
	{
		Instrument *newInstrument = new Instrument(newInstrumentName_);
		instruments_->push_back(newInstrument);

		sequencer_->setInstrumentPlayingOn((*instruments_)[instrumentNumber_]);

		system_->closeWindow(&(view_->newInstrumentWindow_));

		view_->updateInstrumentWindow();

		// this will be removed!!
		for (unsigned int index = 0; index < sequencer_->getTrackAmount(); index++)
		{
			Track *track = sequencer_->getTrack(index);

			track->instrument = newInstrument;
		}

	}
	else if (event == &(view_->newInstrumentCancelButton_.onClick_))
	{
		system_->closeWindow(&(view_->newInstrumentWindow_));
	}
	else if (event == &(view_->newInstrumentName_.textChange_))
	{
		newInstrumentName_ = view_->newInstrumentName_.textChange_.getText();
	}
	
}

void InstrumentController::newInstrumentWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->newInstrumentWindowCallback(widget, event);
}


// Delete instrument window
void InstrumentController::deleteInstrumentWindowCallback(Widget *widget, Event *event)
{
	if (widget == &view_->deleteInstrumentOkButton_)
	{
		delete (*instruments_)[instrumentNumber_];
		instruments_->erase(instruments_->begin() + instrumentNumber_);

		if (instrumentNumber_ > 0) instrumentNumber_--;


		if (instrumentNumber_ < instruments_->size()) sequencer_->setInstrumentPlayingOn((*instruments_)[instrumentNumber_]);
		else sequencer_->setInstrumentPlayingOff();

		
		system_->closeWindow(&view_->deleteInstrumentWindow_);
		view_->updateInstrumentWindow();
	}
	else if (widget == &view_->deleteInstrumentCancelButton_)
	{
		system_->closeWindow(&view_->deleteInstrumentWindow_);
	}
}

void InstrumentController::deleteInstrumentWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->deleteInstrumentWindowCallback(widget, event);
}


// Instrument properties window
void InstrumentController::instrumentPropertiesWindowCallback(Widget *widget, Event *event)
{
	if (event == &view_->instrumentPropertiesOkButton_.onClick_)
	{
		(*instruments_)[instrumentNumber_]->setName(instrumentNewName_);
		(*instruments_)[instrumentNumber_]->setType(instrumentNewType_);

		system_->closeWindow(&view_->instrumentPropertiesWindow_);
		view_->updateInstrumentWindow();
	}
	else if (event == &view_->instrumentPropertiesCancelButton_.onClick_)
	{
		system_->closeWindow(&view_->instrumentPropertiesWindow_);
	}
	else if (event == &view_->instrumentName_.textChange_)
	{
		instrumentNewName_ = view_->instrumentName_.getText();
	}
	else if (event == &view_->instrumentType_.chooseEvent_)
	{
		instrumentNewType_ = view_->instrumentType_.getButtonNumber();
	}
}

void InstrumentController::instrumentPropertiesWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->instrumentPropertiesWindowCallback(widget, event);
}


// Sample mapping window
void InstrumentController::sampleMappingWindowCallback(Widget *widget, Event *event)
{
	if (event == &view_->sampleMappingOkButton_.onClick_)
	{
		system_->closeWindow(&view_->sampleMappingWindow_);
	}
	else if (event == &view_->addSampleButton_.onClick_)
	{
		sampleBankSampleNumber_ = 0;

		view_->openAddSampleWindow();
	}
	else if (event == &view_->removeSampleButton_.onClick_)
	{
		view_->openRemoveSampleWindow();
	}
	else if (event == &view_->sampleList_.chooseEvent_)
	{
		sampleNumber_ = view_->sampleList_.chooseEvent_.getNumber();

		setZoom();
		
		view_->updateSampleMappingWindow();
	}
	else if (event == &view_->loopType_.chooseEvent_)
	{
		(*(*instruments_)[instrumentNumber_]->getSampleData())[sampleNumber_].loopType = view_->loopType_.chooseEvent_.getNumber();
	}
	else if (event == &view_->rootNoteMenu_.chooseEvent_)
	{
		(*(*instruments_)[instrumentNumber_]->getSampleData())[sampleNumber_].rootNote = view_->rootNoteMenu_.chooseEvent_.getNumber();
	}
	else if (event == &view_->lowNoteMenu_.chooseEvent_)
	{
		(*(*instruments_)[instrumentNumber_]->getSampleData())[sampleNumber_].lowNote = view_->lowNoteMenu_.chooseEvent_.getNumber();
	}
	else if (event == &view_->highNoteMenu_.chooseEvent_)
	{
		(*(*instruments_)[instrumentNumber_]->getSampleData())[sampleNumber_].highNote = view_->highNoteMenu_.chooseEvent_.getNumber();
	}
	else if (event == &view_->playSampleButton_.onClick_)
	{
		//sequencer_->setPlayingSample((*(*instruments_)[instrumentNumber_]->getSampleData())[sampleNumber_].sample);
		Time time;
		time.microSeconds = 0;
		sequencer_->noteOn(0, 128, time);
	}
	
}

void InstrumentController::sampleMappingWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->sampleMappingWindowCallback(widget, event);
}


// Add sample window
void InstrumentController::addSampleWindowCallback(Widget *widget, Event *event)
{
	if (event == &view_->addSampleOkButton_.onClick_)
	{
		// create a new sample instance for the instrument
		SampleData data;
		data.sample = (*samples_)[sampleBankSampleNumber_];

		data.name = data.sample->getName();
		data.rootNote = 0;
		data.lowNote = 0;
		data.highNote = 0;

		data.attack = 0.00001;
		data.decay = 0.1;
		data.sustain = 1;
		data.release = 0.01;

		data.loopType = 0;
		data.loopStart = 0;
		data.loopEnd = data.sample->getLength() - 1;

		data.poly = true;
		data.portamento = false;
		data.portamentoRate = 1.2;

		bool zoom = false;

		if (getSampleAmount(instrumentNumber_) == 0) zoom = true;


		(*instruments_)[instrumentNumber_]->addSampleData(data);

		system_->closeWindow(&view_->addSampleWindow_);

		if (zoom) setZoom();


		view_->updateSampleMappingWindow();
	}
	else if (event == &view_->addSampleCancelButton_.onClick_)
	{
		system_->closeWindow(&view_->addSampleWindow_);
	}
	else if (event == &view_->loadSampleButton_.onClick_)
	{
		loadSampleFileWindowController_.initData();

		
		view_->loadSampleFileWindowView_.open(view_->addSampleWindow_.getX() + 10, view_->addSampleWindow_.getY() + 10, 500, 600);
	}
	else if (event == &view_->addSampleList_.chooseEvent_)
	{
		sampleBankSampleNumber_ = view_->addSampleList_.chooseEvent_.getNumber();
	}

}

void InstrumentController::addSampleWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->addSampleWindowCallback(widget, event);
}



// Load sample callback
void InstrumentController::loadSampleCallback(FileWindowController *fileWindowController)
{

	std::string fileName = fileWindowController->getFileName();

	Wrapper *wrapper = system_->getWrapper();


	// wav-loader here
	Wave wave;
	if (wrapper->loadWave(&wave, (char*)fileName.c_str()))
	{
		Sample *sample = new Sample;


		sample->setData(wave.buffer);
		sample->setSampleRate(wave.sampleRate);
		sample->setLength(wave.samplesPerChannel);
		sample->setName(fileName);

		samples_->push_back(sample);

		
	}

	view_->updateAddSampleWindow();

}

void InstrumentController::loadSampleCallback_(void *object, FileWindowController *fileWindowController)
{
	InstrumentController *p = (InstrumentController*)object;
	p->loadSampleCallback(fileWindowController);

}


// Remove sample window
void InstrumentController::removeSampleWindowCallback(Widget *widget, Event *event)
{
	if (event == &view_->removeSampleOkButton_.onClick_)
	{
		std::vector <SampleData> *dataVector = (*instruments_)[instrumentNumber_]->getSampleData();

		dataVector->erase(dataVector->begin() + sampleNumber_);

		if (sampleNumber_ > 0) sampleNumber_--;

		setZoom();

		system_->closeWindow(&view_->removeSampleWindow_);
		view_->updateSampleMappingWindow();
	}
	else if (event == &view_->removeSampleCancelButton_.onClick_)
	{
		system_->closeWindow(&view_->removeSampleWindow_);
		view_->updateSampleMappingWindow();
	}
}

void InstrumentController::removeSampleWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController*)object;

	p->removeSampleWindowCallback(widget, event);
}


void InstrumentController::sampleGFXWindowCallback(Widget *widget, Event *event)
{
	if (event == &view_->sampleGFXWindow_.onClick_)
	{
		zoomX_ = view_->sampleGFXWindow_.onClick_.getLocalX();

		zoomSpot_ = (zoomX_ + view_->sampleGFXWindow_.getScrollX())*zoomRatio_;
	}
	else if (event == &view_->sampleZoomInButton_.onClick_)
	{

	
		unsigned int sampleLength = getSampleLength(instrumentNumber_, sampleNumber_);

		zoomRatio_ /= 1.3;

		if (zoomRatio_ < (double)1 / (double)10) zoomRatio_ = (double)1 / (double)10;

		view_->sampleGFXWindow_.setScrollWidth((double)(sampleLength) / zoomRatio_);
		int newScrollX = (((double)zoomSpot_) / zoomRatio_) - zoomX_;

		view_->sampleGFXWindow_.setScrollX(newScrollX);

	}
	else if (event == &view_->sampleZoomOutButton_.onClick_)
	{
	
		unsigned int sampleLength = getSampleLength(instrumentNumber_, sampleNumber_);

		zoomRatio_ *= 1.3;

		if (zoomRatio_ > ((double)(sampleLength) / ((double)view_->waveFormSprite_.width)))
			zoomRatio_ = ((double)(sampleLength) / ((double)view_->waveFormSprite_.width));

		view_->sampleGFXWindow_.setScrollWidth((double)(sampleLength) / zoomRatio_);
		int newScrollX = (((double)zoomSpot_) / zoomRatio_) - zoomX_;

		view_->sampleGFXWindow_.setScrollX(newScrollX);
	}
}

void InstrumentController::sampleGFXWindowCallback_(void *object, Widget *widget, Event *event)
{
	InstrumentController *p = (InstrumentController *)object;

	p->sampleGFXWindowCallback(widget, event);
}

void InstrumentController::setZoom()
{
	if (getSampleAmount(instrumentNumber_) > 0)
	{
		unsigned int sampleLength = getSampleLength(instrumentNumber_, sampleNumber_);

		view_->sampleGFXWindow_.setScrollX(0);

		zoomRatio_ = ((double)(sampleLength) / ((double)view_->waveFormSprite_.width));
		zoomSpot_ = ((double)(sampleLength / 2));
		zoomX_ = zoomSpot_ / zoomRatio_ - view_->sampleGFXWindow_.getScrollX();
	}
	
}