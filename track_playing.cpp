#include "sequencer.h"
#include "sample.h"
#include "instrument.h"
#include <sstream>

extern int debugInt;
void Sequencer::newTrack()
{
	Track newTrack;

	newTrack.eventLoop = false;
	newTrack.eventLoopIndex = 0;

	newTrack.instrument = 0;

	newTrack.recording = true;

	tracks_.push_back(newTrack);
}

void Sequencer::roofFunction(int bar, unsigned int beat, double pulse, int *endBar, unsigned int *endBeat, unsigned int *endPulse, bool allowLooping)
{
	*endBar = bar;
	*endBeat = beat;
	*endPulse = (int)pulse;

	bool looped;

	if (((double)((int)pulse)) != pulse)
	{
		INTEGER_calculateNewPosition(1, *endBar, *endBeat, *endPulse, endBar, endBeat, endPulse, &looped);
	}

	if (looped && (!allowLooping))
	{
		*endBar = loopEndBar_;
		*endBeat = 0;
		*endPulse = 0;
	}

}

double Sequencer::pulseAmount(int beginBar, unsigned int beginBeat, double beginPulse,
	int endBar, unsigned int endBeat, double endPulse)
{
	double sum = 0;

	if (endBar < beginBar)
	{
		for (int bar = beginBar; bar < loopEndBar_; bar++)
		{
			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += pulsesPerMeasure(signature);
		}

		for (int bar = loopBeginBar_; bar < endBar; bar++)
		{
			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += pulsesPerMeasure(signature);
		}
	}
	else
	{
		for (int bar = beginBar; bar < endBar; bar++)
		{
			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += pulsesPerMeasure(signature);
		}
	}

	sum -= (beginBeat)*ppq_;
	sum -= beginPulse;

	sum += (endBeat)*ppq_;
	sum += endPulse;

	return sum;
}


int Sequencer::INTEGER_pulseAmount(int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse)
{
	int sum = 0;

	if (endBar < beginBar)
	{
		for (int bar = beginBar; bar < loopEndBar_; bar++)
		{
			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += INTEGER_pulsesPerMeasure(signature);
		}

		for (int bar = loopBeginBar_; bar < endBar; bar++)
		{

			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += INTEGER_pulsesPerMeasure(signature);
		}
	}
	else
	{
		for (int bar = beginBar; bar < endBar; bar++)
		{
			TimeSignature signature;
			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			sum += INTEGER_pulsesPerMeasure(signature);
		}
	}

	

	sum -= (int)(beginBeat)*(int)ppq_;
	sum -= (int)beginPulse;

	sum += (int)(endBeat)*(int)ppq_;
	sum += (int)endPulse;

	return sum;
}

unsigned int Sequencer::lastPulse(double pulsesPerMeasure)
{
	while (pulsesPerMeasure >= ppq_)
	{
		pulsesPerMeasure -= ppq_;
	}

	if (pulsesPerMeasure > ((double)((unsigned int)pulsesPerMeasure))) return (unsigned int)pulsesPerMeasure;
	else return ((unsigned int)pulsesPerMeasure) - 1;

}

unsigned int Sequencer::beatsPerMeasure(TimeSignature signature)
{
	return (signature.numerator * 4 ) / signature.denominator;
}

double Sequencer::pulsesPerMeasure(TimeSignature signature)
{
	return (double)(signature.numerator * 4 * ppq_) / (double)signature.denominator;
}

int Sequencer::INTEGER_pulsesPerMeasure(TimeSignature signature)
{
	return (signature.numerator * 4 * ppq_) / signature.denominator;
}

double Sequencer::samplesPerPulse(unsigned int sampleRate)
{
	return (double)(sampleRate * 60) / (tempo_ * (double)ppq_);
}

double Sequencer::pulsesPerSample(unsigned int sampleRate)
{
	return (double)ppq_ * (tempo_ / (double)60) / (double)sampleRate;
}

double Sequencer::getBarPulse(double pulse, unsigned int beat)
{
	return (double)(beat*ppq_) + pulse;
}

unsigned int Sequencer::INTEGER_getBarPulse(unsigned int pulse, unsigned int beat)
{
	return beat*ppq_ + pulse;
}


void Sequencer::getNoteTime(unsigned int noteIndex, Phrase *phrase, int *bar, unsigned int *beat, unsigned int *pulse)
{
	if (phrase->notes.size() <= noteIndex) return;

	Note note = phrase->notes[noteIndex];

	unsigned int beginBar = phrase->beginBar;
	unsigned int beginBeat = phrase->beginBeat;
	unsigned int beginPulse = phrase->beginPulse;


	int endBar;
	unsigned int endBeat, endPulse;
	bool looped;

	INTEGER_calculateNewPosition(note.position, beginBar, beginBeat, beginPulse, &endBar, &endBeat, &endPulse, &looped);

	*bar = endBar;
	*beat = endBeat;
	*pulse = endPulse;
}

bool Sequencer::eventSmallerThan(MidiEvent event1, MidiEvent event2)
{
	if (event1.bar < event2.bar) return true;
	else if (event1.bar == event2.bar)
	{
		if (event1.beat < event2.beat) return true;
		else if (event1.beat == event2.beat)
		{
			if (event1.pulse < event2.pulse) return true;
			else return false;
		}
	}

	return false;
}

void Sequencer::calculateNewPosition(double pulsesAdvanced, int bar, unsigned int beat, double pulse, int *endBar, unsigned int *endBeat, double *endPulse, bool *looped)
{
	*looped = false;
	pulse += pulsesAdvanced;
	
	TimeSignature signature;
	
	if (bar < 0) signature = signatures_[0];
	else if (bar >= signatures_.size()) signature = currentSignature_;
	else signature = signatures_[bar];
	
	
	while (getBarPulse(pulse, beat) >= pulsesPerMeasure(signature)) // test if sequence advances in bars
	{
		pulse = getBarPulse(pulse, beat) - pulsesPerMeasure(signature);
				
		beat = 0;
		bar++;
		

		if (loop_)
		{
			if (bar >= loopEndBar_)
			{
				*looped = true;
				bar = loopBeginBar_;
			}
		}
		
		if (bar < 0) signature = signatures_[0];
		else if (bar >= signatures_.size()) signature = currentSignature_;
		else signature = signatures_[bar];
	}

	while (pulse >= ppq_)
	{
		pulse -= ppq_;
		beat++;
	}

	*endBar = bar;
	*endBeat = beat;
	*endPulse = pulse;
}


void Sequencer::INTEGER_calculateNewPosition(unsigned int pulsesAdvanced, int bar, unsigned int beat, unsigned int pulse, int *endBar, unsigned int *endBeat, unsigned int *endPulse, bool *looped)
{

	*looped = false;
	pulse += pulsesAdvanced;

	TimeSignature signature;

	if (bar < 0) signature = signatures_[0];
	else if (bar >= signatures_.size()) signature = currentSignature_;
	else signature = signatures_[bar];


	while (INTEGER_getBarPulse(pulse, beat) >= INTEGER_pulsesPerMeasure(signature)) // test if sequence advances in bars
	{
		pulse = INTEGER_getBarPulse(pulse, beat) - INTEGER_pulsesPerMeasure(signature);

		beat = 0;
		bar++;


		if (loop_)
		{
			if (bar >= loopEndBar_)
			{
				*looped = true;
				bar = loopBeginBar_;
			}
		}

		if (bar < 0) signature = signatures_[0];
		else if (bar >= signatures_.size()) signature = currentSignature_;
		else signature = signatures_[bar];
	}

	while (pulse >= ppq_)
	{
		pulse -= ppq_;
		beat++;
	}

	*endBar = bar;
	*endBeat = beat;
	*endPulse = pulse;
}


void Sequencer::insertEvent(Track *track, MidiEvent event, bool looped)
{
	bool last = true;
	
	if (looped)
	{
		for (unsigned int index = track->eventLoopIndex; index < track->events.size(); index++)
		{
			if (eventSmallerThan(event, track->events[index]))
			{
				track->events.insert(track->events.begin() + index, event);
				last = false;

				break;
			}

		}

		track->eventLoop = true;

		if(last) track->events.push_back(event);
	}
	else
	{
		unsigned int lastIndex;

		if (track->eventLoop) lastIndex = track->eventLoopIndex;
		else lastIndex = track->events.size();

		for (unsigned int index = 0; index < lastIndex; index++)
		{
			if (eventSmallerThan(event, track->events[index]))
			{
				track->events.insert(track->events.begin() + index, event);
				last = false;

				break;
			}
		}

		if (last) track->events.insert(track->events.begin() + lastIndex, event);

		track->eventLoopIndex++;
	}
}


void Sequencer::deleteAllEvents(Track *track)
{
	track->events.clear();
	track->eventLoop = false;
	track->eventLoopIndex = 0;
}

void Sequencer::deleteEvents(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse)
{
	MidiEvent scanBegin;
	scanBegin.bar = beginBar;
	scanBegin.beat = beginBeat;
	scanBegin.pulse = beginPulse;


	// First: delete old events

	unsigned int deletedAmount = 0;

	unsigned int lastIndex = track->events.size();
	if (track->eventLoop) lastIndex = track->eventLoopIndex;

	// check how many events are needed to be deleted
	for (unsigned int i = 0; i < lastIndex; i++)
	{
		if (eventSmallerThan(track->events[i], scanBegin)) deletedAmount++;
		else break;
	}

	track->eventLoopIndex -= deletedAmount;

	while (deletedAmount > 0)
	{
		track->events.erase(track->events.begin());
		deletedAmount--;
	}
}

void Sequencer::scanNotes(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse)
{

	MidiEvent scanBegin;
	scanBegin.bar = beginBar;
	scanBegin.beat = beginBeat;
	scanBegin.pulse = beginPulse;

	MidiEvent scanEnd;
	scanEnd.bar = endBar;
	scanEnd.beat = endBeat;
	scanEnd.pulse = endPulse;

	

	for (unsigned int phraseIndex = 0; phraseIndex < track->phrases.size(); phraseIndex++)
	{
		Phrase *phrase = &(track->phrases[phraseIndex]);

		MidiEvent phraseBegin;
		phraseBegin.bar = phrase->beginBar;
		phraseBegin.beat = phrase->beginBeat;
		phraseBegin.pulse = phrase->beginPulse;

		MidiEvent phraseEnd;
		phraseEnd.bar = phrase->endBar;
		phraseEnd.beat = phrase->endBeat;
		phraseEnd.pulse = phrase->endPulse;


		if (eventSmallerThan(phraseBegin, scanEnd) && eventSmallerThan(scanBegin, phraseEnd))
		{
			for (unsigned int noteIndex = 0; noteIndex < phrase->notes.size(); noteIndex++)
			{
				Note *note = &(phrase->notes[noteIndex]);
				MidiEvent noteBegin;

				getNoteTime(noteIndex, phrase, &noteBegin.bar, &noteBegin.beat, &noteBegin.pulse);

				if ((!eventSmallerThan(noteBegin, scanBegin)) && eventSmallerThan(noteBegin, scanEnd))
				{
					noteBegin.type = MIDIEVENT_NOTEON;
					noteBegin.value = note->value;
					noteBegin.velocity = note->velocity;

					insertEvent(track, noteBegin, false);


					MidiEvent noteEnd;

					noteEnd.type = MIDIEVENT_NOTEOFF;
					noteEnd.value = note->value;

					
					bool looped;

					INTEGER_calculateNewPosition(note->duration, noteBegin.bar, noteBegin.beat, noteBegin.pulse, &noteEnd.bar, &noteEnd.beat, &noteEnd.pulse, &looped);
										

					insertEvent(track, noteEnd, looped);
				}
			}
		}
	}
}



void Sequencer::playNotes(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse)
{

	MidiEvent endEvent;

	
	roofFunction(endBar, endBeat, endPulse, &endEvent.bar, &endEvent.beat, &endEvent.pulse, false);


	unsigned int lastBar = beginBar;
	unsigned int lastBeat = beginBeat;
	double lastPulse = beginPulse;

	
	unsigned int playLength = sampleAmount;
	

	int lastIndex = track->events.size();
	if (track->eventLoop) lastIndex = track->eventLoopIndex;


	unsigned int offset = 0;

	for (unsigned int eventIndex = 0; eventIndex < lastIndex; eventIndex++)
	{
		MidiEvent event = track->events[eventIndex];

		

		
		//if (!eventSmallerThan(endEvent, event))
		if (eventSmallerThan(event, endEvent))
		{

			double pulses = pulseAmount(lastBar, lastBeat, lastPulse, event.bar, event.beat, event.pulse);
			//double pulses = pulseAmount(beginBar, beginBeat, beginPulse, event.bar, event.beat, event.pulse);

			int samples = samplesPerPulse(sampleRate)*pulses;

			// this is for notes that are just being played or recorded

			// not ONLY!!
			if (samples > 0)
			{

				unsigned int playSamples = samples;

				if (offset + playSamples > (unsigned)sampleAmount) playSamples = (unsigned)sampleAmount - offset;



				// play existing playing notes to this point

				unsigned int noteIndex = 0;
				while (noteIndex < track->playingNotes.size())
				{
					if (!track->playingNotes[noteIndex].countersFinished)
						playNote(&track->playingNotes[noteIndex], track->instrument, &leftBuffer[offset], &rightBuffer[offset], playSamples, sampleRate);

					if (track->playingNotes[noteIndex].countersFinished && track->playingNotes[noteIndex].released) track->playingNotes.erase(track->playingNotes.begin() + noteIndex);
					else noteIndex++;
				}


				offset += playSamples;


				lastBar = event.bar;
				lastBeat = event.beat;
				lastPulse = event.pulse;
			}

			// create a new playing note
			if (event.type == MIDIEVENT_NOTEON)
			{
				createPlayingNote(&track->playingNotes, track->instrument, event.value, event.velocity);
			}
			else if (event.type == MIDIEVENT_NOTEOFF)
			{
				releasePlayingNote(&track->playingNotes, track->instrument, event.value);
			}
		}
	}


	unsigned int noteIndex = 0;
	while (noteIndex < track->playingNotes.size())
	{
		if (!track->playingNotes[noteIndex].countersFinished)
			playNote(&track->playingNotes[noteIndex], track->instrument, &leftBuffer[offset], &rightBuffer[offset], sampleAmount -offset, sampleRate);

		if (track->playingNotes[noteIndex].countersFinished && track->playingNotes[noteIndex].released) track->playingNotes.erase(track->playingNotes.begin() + noteIndex);
		else noteIndex++;
	}



}

void Sequencer::playTrack(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse, bool looped)
{

	MidiEvent lastScanEnd;
	roofFunction(beginBar, beginBeat, beginPulse, &lastScanEnd.bar, &lastScanEnd.beat, &lastScanEnd.pulse, false);

	MidiEvent scanBegin = lastScanEnd;

	MidiEvent scanEnd;
	roofFunction(endBar, endBeat, endPulse, &scanEnd.bar, &scanEnd.beat, &scanEnd.pulse, false);

	MidiEvent recBegin;
	recBegin.bar = recBeginBar_;
	recBegin.beat = recBeginBeat_;
	recBegin.pulse = recBeginPulse_;

	bool doScan = true;



	// don't scan if in count-in mode
	if (countInModeOn_ && (!eventSmallerThan(recBegin, scanEnd))) doScan = false;
	else if (countInModeOn_ && eventSmallerThan(lastScanEnd, recBegin)) scanBegin = recBegin;



	if (looped)
	{

		double pulsesBeforeLoopEnd = pulseAmount(beginBar, beginBeat, beginPulse, loopEndBar_, 0, 0);
		
		unsigned int samplesBeforeLoopEnd = (unsigned int)(pulsesBeforeLoopEnd*samplesPerPulse(sampleRate));

		deleteEvents(track, lastScanEnd.bar, lastScanEnd.beat, lastScanEnd.pulse);
		if (doScan) scanNotes(track, scanBegin.bar, scanBegin.beat, scanBegin.pulse, loopEndBar_, 0, 0);


		playNotes(track, leftBuffer, rightBuffer, samplesBeforeLoopEnd, sampleRate, beginBar, beginBeat, beginPulse, loopEndBar_, 0, 0);
		if (rec_ && track->recording) continuePhrases(track, beginBar, beginBeat, (int)beginPulse, loopEndBar_, 0, 0, true);


		unsigned int size = track->eventLoopIndex;
		unsigned int index = 0;

		while (index < size)
		{
			track->events.erase(track->events.begin());
			index++;
		}

		track->eventLoopIndex = track->events.size();
		track->eventLoop = false;
		
		if ((sampleAmount - samplesBeforeLoopEnd) > 0)
		{
			deleteEvents(track, loopBeginBar_, 0, 0);

			scanNotes(track, loopBeginBar_, 0, 0, scanEnd.bar, scanEnd.beat, scanEnd.pulse);
			playNotes(track, leftBuffer + samplesBeforeLoopEnd, rightBuffer + samplesBeforeLoopEnd, sampleAmount - samplesBeforeLoopEnd, sampleRate, loopBeginBar_, 0, 0, endBar, endBeat, endPulse);
			if (rec_ && track->recording) continuePhrases(track, loopBeginBar_, 0, 0, scanEnd.bar, scanEnd.beat, scanEnd.pulse, false);
		}
		
	}
	else
	{
		
		deleteEvents(track, lastScanEnd.bar, lastScanEnd.beat, lastScanEnd.pulse);

		if(doScan)
			scanNotes(track, scanBegin.bar, scanBegin.beat, scanBegin.pulse, scanEnd.bar, scanEnd.beat, scanEnd.pulse);


		playNotes(track, leftBuffer, rightBuffer, sampleAmount, sampleRate, beginBar, beginBeat, beginPulse, endBar, endBeat, endPulse);

		if (rec_ && track->recording) continuePhrases(track, beginBar, beginBeat, (int)beginPulse, scanEnd.bar, scanEnd.beat, scanEnd.pulse, false);
	}

}


void Sequencer::stopNotes(Track *track)
{
	for (unsigned int index = 0; index < track->playingNotes.size(); index++)
	{
		releasePlayingNote(&(track->playingNotes), track->instrument, track->playingNotes[index].value);
	}
}

void Sequencer::playSong(short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate)
{
	
	
	double pulsesAdvanced = 0;
	
	if(play_) pulsesAdvanced = pulsesPerSample(sampleRate) * (double)sampleAmount;

	int endBar;
	unsigned int endBeat;
	double endPulse;
	
	bool looped;

	calculateNewPosition(pulsesAdvanced, bar_, beat_, pulse_, &endBar, &endBeat, &endPulse, &looped);

	// see if count in ends
	if (rec_ && countInModeOn_)
	{
		MidiEvent recBegin;
		recBegin.bar = recBeginBar_;
		recBegin.beat = recBeginBeat_;
		recBegin.pulse = recBeginPulse_;

		MidiEvent scanEnd;
		roofFunction(endBar, endBeat, endPulse, &scanEnd.bar, &scanEnd.beat, &scanEnd.pulse, false);

		if (!eventSmallerThan(scanEnd, recBegin))
		{
			countInModeOn_ = false;

			for (unsigned int index = 0; index < tracks_.size(); index++)
			{
				Track *track = &tracks_[index];
								
				if (track->recording)
				{

					Phrase *search = searchForPhrase(track, recBeginBar_, recBeginBeat_, recBeginPulse_);

					if (!search)
					{
						Phrase newPhrase;
						newPhrase.beginBar = recBeginBar_;
						newPhrase.beginBeat = recBeginBeat_;
						newPhrase.beginPulse = recBeginPulse_;

						newPhrase.endBar = recBeginBar_;
						newPhrase.endBeat = recBeginBeat_;
						newPhrase.endPulse = recBeginPulse_;

						newPhrase.recording = true;

						track->phrases.push_back(newPhrase);

						search = &track->phrases[track->phrases.size() - 1];
					}

					search->recording = true;
				}
			}
		}
	}
	
	

	for (unsigned int track = 0; track < tracks_.size(); track++)
	{
		playTrack(&tracks_[track], leftBuffer, rightBuffer, sampleAmount, sampleRate, bar_, beat_, pulse_, endBar, endBeat, endPulse, looped);
	}

	playMetronomeTrack(&metronomeTrack_, leftBuffer, rightBuffer, sampleAmount, sampleRate, bar_, beat_, pulse_, endBar, endBeat, endPulse, looped);

	
	
	
	pulse_ = endPulse;
	beat_ = endBeat;
	bar_ = endBar;
		
}

void Sequencer::trackNoteOn(Track *track, unsigned int value, unsigned int velocity)
{

	MidiEvent noteOn;
	noteOn.type = MIDIEVENT_NOTEON;
	noteOn.value = value;
	noteOn.velocity = velocity;


	roofFunction(bar_, beat_, pulse_, &noteOn.bar, &noteOn.beat, &noteOn.pulse, true);
	
	insertEvent(track, noteOn, false);


	if (rec_ && track->recording)
	{
		bool recordIt = true;
		int recBar, recBeat, recPulse;

		recBar = bar_;
		recBeat = beat_;
		recPulse = (int)pulse_;

			
		if (countInModeOn_)
		{
			
			if (INTEGER_pulseAmount(bar_, beat_, pulse_, recBeginBar_, recBeginBeat_, recBeginPulse_) >= ppq_ / 4) recordIt = false;
			

			recBar = recBeginBar_;
			recBeat = recBeginBeat_;
			recPulse = (int)recBeginPulse_;
		}

		if (recordIt)
		{
			Phrase *phrase = 0;

			phrase = searchForPhrase(track, recBar, recBeat, recPulse);

			if (!phrase)
			{

				Phrase newPhrase;

				newPhrase.beginBar = recBar;
				newPhrase.beginBeat = recBeat;
				newPhrase.beginPulse = recPulse;
				
				newPhrase.endBar = recBar;
				newPhrase.endBeat = recBeat;
				newPhrase.endPulse = recPulse;
				

				track->phrases.push_back(newPhrase);

				phrase = &track->phrases[track->phrases.size() - 1];
			}

			Note note;
			note.value = value;
			note.velocity = velocity;

			unsigned int noteBar, noteBeat, notePulse;
			noteBar = recBar;
			noteBeat = recBeat;
			notePulse = recPulse;


			note.position = INTEGER_pulseAmount(phrase->beginBar, phrase->beginBeat, phrase->beginPulse, noteBar, noteBeat, notePulse);

			phrase->recording = true;
			phrase->recordingNotes.push_back(note);
		}
	}


}

void Sequencer::trackNoteOff(Track *track, unsigned int value)
{
	MidiEvent noteOff;
	noteOff.type = MIDIEVENT_NOTEOFF;
	noteOff.value = value;
	

	roofFunction(bar_, beat_, pulse_, &noteOff.bar, &noteOff.beat, &noteOff.pulse, true);

	insertEvent(track, noteOff, false);

	if (rec_ && track->recording)
	{
		for (unsigned int index = 0; index < track->phrases.size(); index++)
		{
			Phrase *phrase = &track->phrases[index];

			if (!phrase->recording) continue;

			unsigned int noteIndex = 0;

			while (noteIndex < phrase->recordingNotes.size())
			{
				if (phrase->recordingNotes[noteIndex].value == value)
					finishRecordingNote(phrase, noteIndex);
				else noteIndex++;
			}
		}
	}
}


