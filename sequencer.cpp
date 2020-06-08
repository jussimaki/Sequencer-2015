#include "sequencer.h"
#include "sample.h"
#include "instrument.h"


void Sequencer::callback_(void *sequencerObject, unsigned char *buffer, int len, int sampleRate)
{
	Sequencer *pointer = (Sequencer*)sequencerObject;
	pointer->callback(buffer, len, sampleRate);
}


double Sequencer::getFrequency(double frequency, unsigned int rootNote, unsigned int value)
{
	if (rootNote > value)
	{
		int difference = rootNote - value;

		while (difference >= 12)
		{
			frequency *= 2;
			difference -= 12;
		}

		while (difference > 0)
		{
			frequency *= semiToneRatio_;
			difference--;
		}
	}
	else if (rootNote < value)
	{
		int difference = value - rootNote;

		while (difference >= 12)
		{
			frequency /= 2;
			difference -= 12;
		}

		while (difference > 0)
		{
			frequency /= semiToneRatio_;
			difference--;
		}
	}

	return frequency;
}

void Sequencer::callback(unsigned char *buffer, int len, int sampleRate)
{
	memset(&leftBuffer_, 0, sizeof(short int) * bufferLength);
	memset(&rightBuffer_, 0, sizeof(short int) * bufferLength);

	short *b16 = (short *)buffer;
	int l16 = len / 2;
	
	
	std::vector <NoteOn> localNewNoteOns;
	std::vector <CompletedNote> localCompletedNotes;

	// copy the new noteons to old noteons and create local newnoteons
	
	wrapper_->lock(noteOnLock_);
	
	localNewNoteOns = newNoteOns_;

	for (unsigned int index = 0; index < newNoteOns_.size(); index++)
	{
		noteOns_.push_back(newNoteOns_[index]);
	}

	newNoteOns_.clear();

	localCompletedNotes = completedNotes_;
	completedNotes_.clear();

	wrapper_->unlock(noteOnLock_);


	
	wrapper_->lock(sequencerModeLock_);
	
	// play tracks
	

	wrapper_->lock(trackLock_);	// two nested locks!!

	if (play_)
	{
		
		for (unsigned int trackNumber = 0; trackNumber < tracks_.size(); trackNumber++)
		{
			if (!tracks_[trackNumber].recording) continue;

			for (unsigned int index = 0; index < localNewNoteOns.size(); index++)
			{
				trackNoteOn(&tracks_[trackNumber], localNewNoteOns[index].value, localNewNoteOns[index].velocity);
			}

			for (unsigned int index = 0; index < localCompletedNotes.size(); index++)
			{
				trackNoteOff(&tracks_[trackNumber], localCompletedNotes[index].value);
			}
		}


		
	}

	playSong((short int*)&leftBuffer_, (short int*)&rightBuffer_, l16 / 2, sampleRate);

	wrapper_->unlock(trackLock_);



	// instrument preview mode
	if ((!play_) && instrumentPlayingOn_)
	{
		// create new playing notes for new noteOns
		for (unsigned int index = 0; index < localNewNoteOns.size(); index++)
		{
			createPlayingNote(&playingNotes_, playingInstrument_, localNewNoteOns[index].value, localNewNoteOns[index].velocity);
		}
			
		
	}

	for (unsigned int index = 0; index < localCompletedNotes.size(); index++)
	{
		releasePlayingNote(&playingNotes_, playingInstrument_, localCompletedNotes[index].value);
	}



	// do the actual playing of the notes

	unsigned int index = 0;
	while (index < playingNotes_.size())
	{
		if (!playingNotes_[index].countersFinished)
			playNote(&playingNotes_[index], playingInstrument_, (short int*)&leftBuffer_, (short int*)&rightBuffer_, l16 / 2, sampleRate);

		if (playingNotes_[index].countersFinished && playingNotes_[index].released) playingNotes_.erase(playingNotes_.begin() + index);
		else index++;

	}

	
	wrapper_->unlock(sequencerModeLock_);


	
	for (int index = 0; index < l16 / 2; index++)
	{
		b16[index * 2] = leftBuffer_[index];
		b16[index * 2+1] = rightBuffer_[index];
	}
	
}

void Sequencer::noteOn(unsigned int value, unsigned int velocity, Time time)
{
	wrapper_->lock(noteOnLock_);

	NoteOn newNoteOn;

	newNoteOn.time = time;
	newNoteOn.value = value;
	newNoteOn.velocity = velocity;

	newNoteOns_.push_back(newNoteOn);

	wrapper_->unlock(noteOnLock_);
}



void Sequencer::noteOff(unsigned int value, Time time)
{
	

	wrapper_->lock(noteOnLock_); // lock the noteOns


	unsigned int index = 0;

	while (index < newNoteOns_.size()) // check if one of the new noteOns ends
	{
		if (newNoteOns_[index].value == value)
		{
			CompletedNote note;

			note.value = value;
			note.velocity = newNoteOns_[index].velocity;
			note.start = newNoteOns_[index].time.microSeconds;
			note.duration = time.microSeconds - newNoteOns_[index].time.microSeconds;

			completedNotes_.push_back(note);

			newNoteOns_.erase(newNoteOns_.begin() + index);
		}
		else index++;
	}

	index = 0;

	while (index < noteOns_.size())	// then check if one of the actually playing noteOns ends
	{
		if (noteOns_[index].value == value)
		{
			CompletedNote note;

			note.value = value;
			note.velocity = noteOns_[index].velocity;
			note.start = noteOns_[index].time.microSeconds;
			note.duration = time.microSeconds - noteOns_[index].time.microSeconds;

			completedNotes_.push_back(note);

			noteOns_.erase(noteOns_.begin() + index);
		}
		else index++;
	}

	wrapper_->unlock(noteOnLock_);

	
}


void Sequencer::setInstrumentPlayingOn(Instrument *instrument)
{

	instrumentPlayingOn_ = true;
	playingInstrument_ = instrument;

}

void Sequencer::setInstrumentPlayingOff()
{
	instrumentPlayingOn_ = false;
}


void Sequencer::setPlay(bool play)
{
	bool oldPlay = play_;
	play_ = play;

	pulse_ = (int)pulse_;

	if (!oldPlay && play_ && rec_)
	{
		if (recCountIn_)
		{
			recBeginBar_ = bar_;
			recBeginBeat_ = beat_;
			recBeginPulse_ = pulse_;

			recBeginSignature_ = signatures_[bar_];

			int pulses = INTEGER_pulsesPerMeasure(signatures_[bar_]);

			metronomeTicks_ = (ppq_ * 4) / recBeginSignature_.denominator;
			
			int difference;
						
			do
			{
				bar_--;
				difference = INTEGER_pulseAmount(bar_, beat_, (int)pulse_, recBeginBar_, recBeginBeat_, recBeginPulse_);
			} while (difference < pulses);


			bool looped;

			unsigned int integerPulse;
			INTEGER_calculateNewPosition(difference - pulses, bar_, beat_, pulse_, &bar_, &beat_, &integerPulse, &looped);

			pulse_ = integerPulse;
			

			metronomeBeginBar_ = bar_;
			metronomeBeginBeat_ = beat_;
			metronomeBeginPulse_ = integerPulse;


			countInModeOn_ = true;
		}
		else
		for (unsigned int trackNumber = 0; trackNumber < tracks_.size(); trackNumber++)
		{
			Track *track = &tracks_[trackNumber];
			if (!track->recording) continue;

			Phrase *search = searchForPhrase(track, bar_, beat_, (unsigned int)pulse_);

			if (!search)
			{

				Phrase newPhrase;

				newPhrase.beginBar = bar_;
				newPhrase.beginBeat = beat_;
				newPhrase.beginPulse = (int)pulse_;

				newPhrase.endBar = bar_;
				newPhrase.endBeat = beat_;
				newPhrase.endPulse = (int)pulse_;

				newPhrase.recording = true;
				
				tracks_[trackNumber].phrases.push_back(newPhrase);

				search = &track->phrases[track->phrases.size() - 1];
			}

			search->recording = true;
		}
	}
	else if (oldPlay && !play)
	{
		if (rec_)
		{
			rec_ = false;

			for (unsigned int index = 0; index < tracks_.size(); index++)
			{
				stopRec(&tracks_[index]);
			}
								
		}

		for (unsigned int index = 0; index < tracks_.size(); index++)
		{
			stopNotes(&tracks_[index]);
			deleteAllEvents(&tracks_[index]);
		}




		if (countInModeOn_)
		{
			bar_ = recBeginBar_;
			beat_ = recBeginBeat_;
			pulse_ = recBeginPulse_;

			countInModeOn_ = false;
		}

		beat_ = 0;
		pulse_ = 0;
	}
}