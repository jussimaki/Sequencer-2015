#include "sequencer.h"

void Sequencer::scanMetronome(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse)
{
	MidiEvent scanBegin;
	scanBegin.bar = beginBar;
	scanBegin.beat = beginBeat;
	scanBegin.pulse = beginPulse;

	MidiEvent scanEnd;
	scanEnd.bar = endBar;
	scanEnd.beat = endBeat;
	scanEnd.pulse = endPulse;


	MidiEvent recBegin;
	recBegin.bar = recBeginBar_;
	recBegin.beat = recBeginBeat_;
	recBegin.pulse = recBeginPulse_;

	unsigned int ticks;
	int bar;
	unsigned int beat, pulse;
	unsigned int partNumber;


	TimeSignature signature;

	bool localCountInModeOn = countInModeOn_;

	bar = beginBar;
	beat = 0;
	pulse = 0;

	unsigned char velocity = 32;

	if (localCountInModeOn)
	{


		partNumber = 0;

		signature = recBeginSignature_;
		ticks = (ppq_ * 4) / signature.denominator;

		bar = metronomeBeginBar_;
		beat = 0;
		pulse = 0;





		while (true)
		{
			MidiEvent clickBegin;

			clickBegin.bar = bar;
			clickBegin.beat = beat;
			clickBegin.pulse = pulse;


			if (!eventSmallerThan(clickBegin, recBegin))
			{
				bar = recBeginBar_;
				beat = 0;
				pulse = 0;

				localCountInModeOn = false;
				break;
			}



			if (!eventSmallerThan(clickBegin, scanEnd)) break;
			else if (!eventSmallerThan(clickBegin, scanBegin))
			{
				clickBegin.type = MIDIEVENT_NOTEON;
				if (partNumber == 0) clickBegin.value = 2;
				else clickBegin.value = 0;
				clickBegin.velocity = velocity;

				insertEvent(track, clickBegin, false);


				MidiEvent clickEnd;

				clickEnd.type = MIDIEVENT_NOTEOFF;
				if (partNumber == 0) clickEnd.value = 2;
				else clickEnd.value = 0;


				bool looped;

				INTEGER_calculateNewPosition(ppq_ / 10, clickBegin.bar, clickBegin.beat, clickBegin.pulse, &clickEnd.bar, &clickEnd.beat, &clickEnd.pulse, &looped);

				insertEvent(track, clickEnd, looped);


			}

			partNumber++;

			if (partNumber == signature.numerator) partNumber = 0;

			bool looped;
			INTEGER_calculateNewPosition(ticks, bar, beat, pulse, &bar, &beat, &pulse, &looped);

			if (looped) break;

		}

		if (!localCountInModeOn)
		{
			partNumber = 0;
			while (true)
			{
				MidiEvent clickBegin;

				clickBegin.bar = bar;
				clickBegin.beat = beat;
				clickBegin.pulse = pulse;

				if (bar < 0) signature = signatures_[0];
				else if (bar >= signatures_.size()) signature = currentSignature_;
				else signature = signatures_[bar];

				ticks = (ppq_ * 4) / signature.denominator;


				if (!eventSmallerThan(clickBegin, scanEnd)) break;

				else if ((!eventSmallerThan(clickBegin, recBegin)) && (!eventSmallerThan(clickBegin, scanBegin)))
				{
					clickBegin.type = MIDIEVENT_NOTEON;
					if (partNumber == 0) clickBegin.value = 2;
					else clickBegin.value = 0;
					clickBegin.velocity = velocity;

					insertEvent(track, clickBegin, false);


					MidiEvent clickEnd;

					clickEnd.type = MIDIEVENT_NOTEOFF;
					if (partNumber == 0) clickEnd.value = 2;
					else clickEnd.value = 0;


					bool looped;

					INTEGER_calculateNewPosition(ppq_ / 10, clickBegin.bar, clickBegin.beat, clickBegin.pulse, &clickEnd.bar, &clickEnd.beat, &clickEnd.pulse, &looped);

					insertEvent(track, clickEnd, looped);


				}

				partNumber++;
				if (partNumber == signature.numerator) partNumber = 0;

				bool looped;
				INTEGER_calculateNewPosition(ticks, bar, beat, pulse, &bar, &beat, &pulse, &looped);

				if (looped) break;
			}
		}

	}
	else
	{
		partNumber = 0;
		while (true)
		{
			MidiEvent clickBegin;

			clickBegin.bar = bar;
			clickBegin.beat = beat;
			clickBegin.pulse = pulse;

			if (bar < 0) signature = signatures_[0];
			else if (bar >= signatures_.size()) signature = currentSignature_;
			else signature = signatures_[bar];

			ticks = (ppq_ * 4) / signature.denominator;


			if (!eventSmallerThan(clickBegin, scanEnd)) break;

			else if (!eventSmallerThan(clickBegin, scanBegin))
			{

				clickBegin.type = MIDIEVENT_NOTEON;
				if (partNumber == 0) clickBegin.value = 2;
				else clickBegin.value = 0;
				clickBegin.velocity = velocity;

				insertEvent(track, clickBegin, false);


				MidiEvent clickEnd;

				clickEnd.type = MIDIEVENT_NOTEOFF;
				if (partNumber == 0) clickEnd.value = 2;
				else clickEnd.value = 0;


				bool looped;

				INTEGER_calculateNewPosition(ppq_ / 10, clickBegin.bar, clickBegin.beat, clickBegin.pulse, &clickEnd.bar, &clickEnd.beat, &clickEnd.pulse, &looped);

				insertEvent(track, clickEnd, looped);


			}

			partNumber++;
			if (partNumber == signature.numerator) partNumber = 0;

			bool looped;
			INTEGER_calculateNewPosition(ticks, bar, beat, pulse, &bar, &beat, &pulse, &looped);
			if (looped) break;
		}
	}
}


void Sequencer::playMetronomeTrack(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse, bool looped)
{
	MidiEvent scanBegin;
	roofFunction(beginBar, beginBeat, beginPulse, &scanBegin.bar, &scanBegin.beat, &scanBegin.pulse, false);

	MidiEvent scanEnd;
	roofFunction(endBar, endBeat, endPulse, &scanEnd.bar, &scanEnd.beat, &scanEnd.pulse, false);

	if (looped)
	{

		double pulsesBeforeLoopEnd = pulseAmount(beginBar, beginBeat, beginPulse, loopEndBar_, 0, 0);

		unsigned int samplesBeforeLoopEnd = (unsigned int)(pulsesBeforeLoopEnd*samplesPerPulse(sampleRate));

		scanMetronome(track, scanBegin.bar, scanBegin.beat, scanBegin.pulse, loopEndBar_, 0, 0);

		playNotes(track, leftBuffer, rightBuffer, samplesBeforeLoopEnd, sampleRate, beginBar, beginBeat, beginPulse, loopEndBar_, 0, 0);



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

			scanMetronome(track, loopBeginBar_, 0, 0, scanEnd.bar, scanEnd.beat, scanEnd.pulse);
			playNotes(track, leftBuffer + samplesBeforeLoopEnd, rightBuffer + samplesBeforeLoopEnd, sampleAmount - samplesBeforeLoopEnd, sampleRate, loopBeginBar_, 0, 0, endBar, endBeat, endPulse);
		}

	}
	else
	{

		deleteEvents(track, scanBegin.bar, scanBegin.beat, scanBegin.pulse);

		scanMetronome(track, scanBegin.bar, scanBegin.beat, scanBegin.pulse, scanEnd.bar, scanEnd.beat, scanEnd.pulse);

		playNotes(track, leftBuffer, rightBuffer, sampleAmount, sampleRate, beginBar, beginBeat, beginPulse, endBar, endBeat, endPulse);
	}
}