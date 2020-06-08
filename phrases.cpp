#include "sequencer.h"
#include <sstream>

Phrase *Sequencer::searchForPhrase(Track *track, unsigned int bar, unsigned int beat, unsigned int pulse)
{
	for (unsigned int index = 0; index < track->phrases.size(); index++)
	{
		MidiEvent cursor;

		cursor.bar = bar;
		cursor.beat = beat;
		cursor.pulse = pulse;

		Phrase phrase = track->phrases[index];

		MidiEvent begin;
		begin.bar = phrase.beginBar;
		begin.beat = phrase.beginBeat;
		begin.pulse = phrase.beginPulse;

		MidiEvent end;
		end.bar = phrase.endBar;
		end.beat = phrase.endBeat;
		end.pulse = phrase.endPulse;


		if ((!eventSmallerThan(cursor, begin)) && !eventSmallerThan(end, cursor)) return &track->phrases[index];
	}

	return 0;
}


void Sequencer::continuePhrases(Track *track, unsigned int beginBar, unsigned int beginBeat, unsigned int beginPulse, unsigned int endBar, unsigned int endBeat, unsigned int endPulse, bool looped)
{
	bool doneDeleting = false;
	std::string testString;
	for (unsigned int index = 0; index < track->phrases.size(); index++)
	{
		std::stringstream stream;
		stream.clear();
		stream << index;

		std::string i1;
		stream >> i1;

		testString += "i1:" + i1 + ",";
		Phrase *phrase = &track->phrases[index];

		if (!phrase->recording) continue;

		MidiEvent phraseBegin;
		phraseBegin.bar = phrase->beginBar;
		phraseBegin.beat = phrase->beginBeat;
		phraseBegin.pulse = phrase->beginPulse;

		MidiEvent phraseEnd;
		phraseEnd.bar = phrase->endBar;
		phraseEnd.beat = phrase->endBeat;
		phraseEnd.pulse = phrase->endPulse;


		MidiEvent continueBegin;
		continueBegin.bar = beginBar;
		continueBegin.beat = beginBeat;
		continueBegin.pulse = beginPulse;

		MidiEvent continueEnd;
		continueEnd.bar = endBar;
		continueEnd.beat = endBeat;
		continueEnd.pulse = endPulse;

		if ((!eventSmallerThan(phraseEnd, continueBegin)) && eventSmallerThan(phraseEnd, continueEnd))
		{
			phrase->endBar = endBar;
			phrase->endBeat = endBeat;
			phrase->endPulse = endPulse;

			phraseEnd.bar = phrase->endBar;
			phraseEnd.beat = phrase->endBeat;
			phraseEnd.pulse = phrase->endPulse;

			unsigned int index2 = 0;

			while (index2 < track->phrases.size())
			{
				std::stringstream stream;
				stream.clear();
				stream << index2;

				std::string i2;
				stream >> i2;

				testString += "i2:" + i2 + ",";

				Phrase *phrase2 = &track->phrases[index2];

				if (phrase == phrase2) { testString += " skip "; index2++; continue; }

				MidiEvent phraseBegin2;
				phraseBegin2.bar = phrase2->beginBar;
				phraseBegin2.beat = phrase2->beginBeat;
				phraseBegin2.pulse = phrase2->beginPulse;

				MidiEvent phraseEnd2;
				phraseEnd2.bar = phrase2->endBar;
				phraseEnd2.beat = phrase2->endBeat;
				phraseEnd2.pulse = phrase2->endPulse;

				if (eventSmallerThan(phraseBegin2, phraseEnd) && eventSmallerThan(phraseBegin, phraseEnd2))
				{
					doneDeleting = true;
					joinPhrases(phrase, phrase2);
					track->phrases.erase(track->phrases.begin() + index2);

					testString += " delete(" + i2 + ")";

					if (index > index2) phrase = &track->phrases[index - 1];
				}
				else index2++;


			}

		}
	}

	//if (doneDeleting) debugString += testString + "x";
}

void Sequencer::joinPhrases(Phrase *phrase1, Phrase *phrase2)
{
	MidiEvent phraseBegin1;
	phraseBegin1.bar = phrase1->beginBar;
	phraseBegin1.beat = phrase1->beginBeat;
	phraseBegin1.pulse = phrase1->beginPulse;

	MidiEvent phraseEnd1;
	phraseEnd1.bar = phrase1->endBar;
	phraseEnd1.beat = phrase1->endBeat;
	phraseEnd1.pulse = phrase1->endPulse;

	MidiEvent phraseBegin2;
	phraseBegin2.bar = phrase2->beginBar;
	phraseBegin2.beat = phrase2->beginBeat;
	phraseBegin2.pulse = phrase2->beginPulse;

	MidiEvent phraseEnd2;
	phraseEnd2.bar = phrase2->endBar;
	phraseEnd2.beat = phrase2->endBeat;
	phraseEnd2.pulse = phrase2->endPulse;


	MidiEvent *smallerBegin, *biggerBegin;
	Phrase *bigger;

	if (!eventSmallerThan(phraseBegin2, phraseBegin1))
	{
		bigger = phrase2;
		smallerBegin = &phraseBegin1;
		biggerBegin = &phraseBegin2;
	}
	else
	{
		bigger = phrase1;
		smallerBegin = &phraseBegin2;
		biggerBegin = &phraseBegin1;
	}

	MidiEvent *biggerEnd;

	if (!eventSmallerThan(phraseEnd1, phraseEnd2)) biggerEnd = &phraseEnd1;
	else biggerEnd = &phraseEnd2;

	int pulses = INTEGER_pulseAmount(smallerBegin->bar, smallerBegin->beat, smallerBegin->pulse, biggerBegin->bar, biggerBegin->beat, biggerBegin->pulse);


	// move the notes' positions

	for (unsigned int index = 0; index < bigger->notes.size(); index++)
	{
		bigger->notes[index].position += pulses;
	}

	for (unsigned int index = 0; index < bigger->recordingNotes.size(); index++)
	{
		bigger->recordingNotes[index].position += pulses;
	}


	// move the notes from phrase2 to phrase1
	for (unsigned int index = 0; index < phrase2->notes.size(); index++)
	{
		phrase1->notes.push_back(phrase2->notes[index]);
	}

	phrase2->notes.clear();

	for (unsigned int index = 0; index < phrase2->recordingNotes.size(); index++)
	{
		phrase1->recordingNotes.push_back(phrase2->recordingNotes[index]);
	}

	phrase2->recordingNotes.clear();

	// resize phrase1

	phrase1->beginBar = smallerBegin->bar;
	phrase1->beginBeat = smallerBegin->beat;
	phrase1->beginPulse = smallerBegin->pulse;

	phrase1->endBar = biggerEnd->bar;
	phrase1->endBeat = biggerEnd->beat;
	phrase1->endPulse = biggerEnd->pulse;





}


void Sequencer::finishRecordingNote(Phrase *phrase, unsigned int index)
{
	Note note = phrase->recordingNotes[index];


	int noteBar;
	unsigned int noteBeat, notePulse;
	bool looped;

	INTEGER_calculateNewPosition(note.position, phrase->beginBar, phrase->beginBeat, phrase->beginPulse, &noteBar, &noteBeat, &notePulse, &looped);

	MidiEvent noteBegin;
	noteBegin.bar = noteBar;
	noteBegin.beat = noteBeat;
	noteBegin.pulse = notePulse;

	MidiEvent noteEnd;
	noteEnd.bar = bar_;
	noteEnd.beat = beat_;
	noteEnd.pulse = (int)pulse_;

	if (!countInModeOn_)
	{
		note.duration = INTEGER_pulseAmount(noteBar, noteBeat, notePulse, bar_, beat_, (int)pulse_);

		if (note.duration > 0) phrase->notes.push_back(note);
	}

	phrase->recordingNotes.erase(phrase->recordingNotes.begin() + index);
}

void Sequencer::stopRec(Track *track)
{
	if (!track->recording) return;

	for (unsigned int index = 0; index < track->phrases.size(); index++)
	{
		Phrase *phrase = &track->phrases[index];

		if (!phrase->recording) continue;

		phrase->recording = false;

		// finish notes
		while (phrase->recordingNotes.size())
		{
			finishRecordingNote(phrase, 0);
		}

	}
}