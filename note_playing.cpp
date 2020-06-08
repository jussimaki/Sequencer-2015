#include "sequencer.h"
#include "sample.h"
#include "instrument.h"


extern int debugInt;

int Sequencer::roof(double number)
{
	if ((double)((int)number) < number) return ((int)number) + 1;
	else return (int)number;
}

int Sequencer::floor(double number)
{
	if ((double)((int)number) > number) return ((int)number) - 1;
	else return (int)number;
}

double Sequencer::abs(double number)
{
	if (number < (double)0) return -number;
	else return number;
}

void Sequencer::createPlayingNote(std::vector <PlayingNote> *playingNotes, Instrument *instrument, unsigned int value, unsigned int velocity)
{
	if (!instrument) return;

	PlayingNote newPlayingNote;
		
	newPlayingNote.value = value;
	newPlayingNote.countersFinished = false;
	newPlayingNote.released = false;
	newPlayingNote.velocity = velocity;

	std::vector <SampleData> *dataVector = instrument->getSampleData();


	// create sample layers for the new playing note
	for (unsigned int dataIndex = 0; dataIndex < dataVector->size(); dataIndex++)
	{
		SampleData data = (*dataVector)[dataIndex];

		if (newPlayingNote.value < data.lowNote) continue;
		if (newPlayingNote.value > data.highNote) continue;

		SampleCounter newCounter;

		newCounter.destinationFrequency = getFrequency(data.sample->getSampleRate(), data.rootNote, newPlayingNote.value);
		newCounter.frequency = newCounter.destinationFrequency;
		newCounter.adsrMode = ADSR_ATTACK;

		newCounter.direction = 0;
		newCounter.finished = false;
		newCounter.position = 0;
		newCounter.sampleDataIndex = dataIndex;
		newCounter.volume = 0;
		newCounter.silenced = false;

		// check if the existing playing notes contains the same sample layer
		for (unsigned int noteIndex = 0; noteIndex < playingNotes->size(); noteIndex++)
		{
			PlayingNote *note = &(*playingNotes)[noteIndex];

			for (unsigned int counterIndex = 0; counterIndex < note->counters.size(); counterIndex++)
			{
				if ((note->counters[counterIndex].sampleDataIndex == dataIndex) // same layer

					&& (!note->counters[counterIndex].finished)	// is this line needed?

					&& (!note->counters[counterIndex].silenced)
					)
				{
					// found same sound layer on another note

					if (!data.poly) // mono-mode
					{
						if (data.portamento)
						{

							// set the older layer to silenced mode since a new
							// layer carries on from it
							note->counters[counterIndex].silenced = true;

							if (note->released) note->counters[counterIndex].finished = true;


							newCounter.frequency = note->counters[counterIndex].frequency;

							newCounter.direction = note->counters[counterIndex].direction;

							if (note->counters[counterIndex].adsrMode != ADSR_SUSTAIN) newCounter.adsrMode = ADSR_ATTACK;
							else newCounter.adsrMode = ADSR_SUSTAIN;

							newCounter.position = note->counters[counterIndex].position;

							
							newCounter.volume = note->counters[counterIndex].volume;
						}
						else
						{
							// the old layer is set to fade and the new layer starts from
							// attack mode
							note->counters[counterIndex].adsrMode = ADSR_RELEASE;
						}
					}
					else
					{	// poly-mode

						if (data.portamento)
						{
							newCounter.frequency = note->counters[counterIndex].frequency;
						}
						else
						{

						}
					}
				}
			}
		}

		newPlayingNote.counters.push_back(newCounter);
	}

	playingNotes->push_back(newPlayingNote);
}

void Sequencer::releasePlayingNote(std::vector <PlayingNote> *playingNotes, Instrument *instrument, unsigned int value)
{
	if (!instrument) return;

	std::vector <SampleData> *dataVector = instrument->getSampleData();

	// find the playing note to stop
	for (unsigned int noteIndex = 0; noteIndex < playingNotes->size(); noteIndex++)
	{
		PlayingNote *note = &(*playingNotes)[noteIndex];

		// the note's value is NOT equal to the noteoff event's value
		// so skip to the next note
		if (note->value != value) continue;

		
		// found the right note!
		note->released = true;
		
		// set all the note's samplelayers to fade
		for (unsigned int counterIndex = 0; counterIndex < note->counters.size(); counterIndex++)
		{
			note->counters[counterIndex].adsrMode = ADSR_RELEASE;
			

			if (note->counters[counterIndex].silenced)
				note->counters[counterIndex].finished = true;

			SampleData data = (*dataVector)[counterIndex];


			// proceed ONLY in MONO-PORTA-MODE!!!
			// no older notes are needed to be awaked in other modes.
			if (data.poly || (!data.portamento)) continue;

			
			// find out if there are older silenced note layers that are needed to be continued
			// but first check if some newer layers are playing

			bool newerLayerPlaying = false;

			// check the notes that have started playing after the released note
			for (unsigned int newerNoteIndex = noteIndex + 1; newerNoteIndex < playingNotes->size(); newerNoteIndex++)
			{
				PlayingNote *newerNote = &(*playingNotes)[newerNoteIndex];

				if (newerNote->released) continue;

				// if a newer note is not released, check its sample layers

				for (unsigned int newerCounterIndex = 0; newerCounterIndex < newerNote->counters.size(); newerCounterIndex++)
				{
					// skip to next if not the same sampledata index
					if (note->counters[counterIndex].sampleDataIndex != newerNote->counters[newerCounterIndex].sampleDataIndex) continue;

					newerLayerPlaying = true;
				}
			}

			if (newerLayerPlaying)
			{
				// let's finish this note
				note->counters[counterIndex].finished = true;

				// no need to awake older notes
				continue;
			}

			// scan through older playing notes

			
			bool olderNotePlaying = false;
			for (int olderNoteIndex = noteIndex-1; olderNoteIndex >= 0; olderNoteIndex--)
			{
				PlayingNote *olderNote = &(*playingNotes)[(unsigned int)olderNoteIndex];

				// don't glide back to notes that are already released
				if (olderNote->released) continue;

				olderNotePlaying = true;

								
				for (unsigned int olderCounterIndex = 0; olderCounterIndex < olderNote->counters.size(); olderCounterIndex++)
				{
					// skip to next if not the same sampledata index
					if (note->counters[counterIndex].sampleDataIndex != olderNote->counters[olderCounterIndex].sampleDataIndex) continue;
					

					// samplelayer is not finished but is silenced
					if ((!olderNote->counters[olderCounterIndex].finished) && (olderNote->counters[olderCounterIndex].silenced))
					{
						olderNote->counters[olderCounterIndex].silenced = false;

						olderNote->counters[olderCounterIndex].direction = note->counters[counterIndex].direction;
						olderNote->counters[olderCounterIndex].frequency = note->counters[counterIndex].frequency;
						olderNote->counters[olderCounterIndex].position = note->counters[counterIndex].position;
					}
				}

				break;
			}

			// finish the note if found older notes to be glided into.
			// REMEMBER: this code is executed only in mono-porta-mode!
			if(olderNotePlaying) note->counters[counterIndex].finished = true;
		}
	}
}

void Sequencer::playNote(PlayingNote *note, Instrument *instrument, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate)
{
	if (!instrument) return;

	for (unsigned int index = 0; index < note->counters.size(); index++)
	{
			
		unsigned int dataIndex = note->counters[index].sampleDataIndex;

		// WARNING: access to intrument data without lock, could be changed at the same time?

		if ((*instrument->getSampleData()).size() <= dataIndex)  // test if there's actually sampledata for this layer
		{
			note->counters[index].finished = true;
			
			continue;
		}

		SampleData sampleData = (*instrument->getSampleData())[dataIndex];

		short *sampleBuffer = sampleData.sample->getData();
		int sampleLength = sampleData.sample->getLength();
		

		double resampleFrequency = note->counters[index].frequency;
		double waveSampleRate = sampleData.sample->getSampleRate();
		int resampledSampleLength = (double)sampleLength*resampleFrequency / waveSampleRate;

		if (note->counters[index].position >= resampledSampleLength) note->counters[index].finished = true;

		if (note->counters[index].finished) continue;
		if (note->counters[index].silenced) continue;


		

				
		double amplitude = 0.5;
		amplitude *= (double)note->velocity / (double)127;
		
		
		
		int destinationIndex = 0;
		
		

		double attackRatio = (double)1 / (double)sampleRate / sampleData.attack;
		double decayRatio = (double)1 / (double)sampleRate / sampleData.decay;
		double releaseRatio = (double)sampleData.sustain / (double)sampleRate / sampleData.release;

		double portaRatio = ((double)sampleRate / (double)1000000) / sampleData.portamentoRate;

		portaRatio = 1.00003;


		
		
		double lastSincPoint = ((double)(zeroCrossingAmount_*samplesPerZeroCrossing_)-1) / ((double)samplesPerZeroCrossing_);
		

		
		int i = note->counters[index].position;
		

		while (destinationIndex < sampleAmount)
		{
			switch (note->counters[index].adsrMode)
			{
			case ADSR_ATTACK:
				note->counters[index].volume += attackRatio;
				if (note->counters[index].volume >= 1.0)
				{
					note->counters[index].volume = 1.0;
					note->counters[index].adsrMode = ADSR_DECAY;
				}
				break;
			case ADSR_DECAY:
				note->counters[index].volume -= decayRatio;
				if (note->counters[index].volume <= sampleData.sustain)
				{
					note->counters[index].volume = sampleData.sustain;
					note->counters[index].adsrMode = ADSR_SUSTAIN;
				}
				break;
			case ADSR_SUSTAIN:
				break;
			case ADSR_RELEASE:
				note->counters[index].volume -= releaseRatio;
				if (note->counters[index].volume <= 0)
				{
					note->counters[index].volume = 0;
					note->counters[index].finished = true;
				}
				break;
			default: break;
			}

			double fVolume = amplitude * note->counters[index].volume;


			double sincFactor;

			if (resampleFrequency < waveSampleRate) sincFactor = resampleFrequency / waveSampleRate;
			else sincFactor = 1.0;

			
			double sincFrequency;

			if (resampleFrequency < waveSampleRate) sincFrequency = resampleFrequency;
			else sincFrequency = waveSampleRate;

			sincFrequency -= 20;

			
			
			double firstTerm = (double)i * waveSampleRate / resampleFrequency;
			double secondTerm = lastSincPoint*waveSampleRate / sincFrequency;

			int beginN = roof(firstTerm - secondTerm);
			if (beginN < 0) beginN = 0;
			

			int endN = floor(firstTerm + secondTerm);
			if (endN > sampleLength - 1) endN = sampleLength - 1;

					

			if (endN >= beginN)
			{

				




				
				double sum = 0;

				double first = ((double)i)*sincFrequency / resampleFrequency;

				double secondFactor = sincFrequency / waveSampleRate;


				

							

				double sincIndex = (first - ((double)beginN)*secondFactor)*(double)samplesPerZeroCrossing_;
				double sincRatio = sincIndex - floor(sincIndex);


				double sincRatioStep = secondFactor*((double)samplesPerZeroCrossing_);
				sincRatioStep = sincRatioStep - floor(sincRatioStep);



				double sincStep = secondFactor*((double)samplesPerZeroCrossing_);

				for (int n = beginN; n <= endN; n++)
				{
					double value = 0;

					if (sincIndex >= 0)
					{
						value = sinc_[(int)sincIndex] * (1 - sincRatio) + sinc_[(int)sincIndex + 1] * sincRatio;
						//value = sinc_[(int)sincIndex];
					}
					else
					{
						value = sinc_[-(int)sincIndex] * sincRatio + sinc_[-(int)sincIndex + 1] * (1 - sincRatio);
						//value = sinc_[-(int)sincIndex];
					}

					


					sincIndex -= sincStep;

					sincRatio -= sincRatioStep;

					if (sincRatio < 0) sincRatio += 1;
										
					sum += ((double)sampleBuffer[n]) * value;

				}

				

				sum *= sincFactor;


				leftBuffer[destinationIndex] += (short)(sum * fVolume);
				rightBuffer[destinationIndex] += (short)(sum * fVolume);

			}

						
			/*
			if (note->counters[index].frequency < note->counters[index].destinationFrequency)
			{
				note->counters[index].frequency *= portaRatio;
				resampleFrequency = note->counters[index].frequency;
			}

			if (note->counters[index].frequency > note->counters[index].destinationFrequency)
			{
				note->counters[index].frequency /= portaRatio;
				resampleFrequency = note->counters[index].frequency;
			}

			*/
			
			
			if (i >= resampledSampleLength)
			{
				note->counters[index].finished = true;
				break;
			}

			

			i++;
			destinationIndex++;
		}


		note->counters[index].position = i;
	
	}

	
	note->countersFinished = true;

	for (unsigned int index = 0; index < note->counters.size(); index++)
	{
		if (!note->counters[index].finished)
		{
			note->countersFinished = false;
			continue;
		}
	}
}