#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "wrapper.h"
#include <vector>

class Sample;
#include "instrument.h"

#define sinelength 65

#define ADSR_ATTACK 0
#define ADSR_DECAY 1
#define ADSR_SUSTAIN 2
#define ADSR_RELEASE 3

#define MIDIEVENT_NOTEON 0
#define MIDIEVENT_NOTEOFF 1
#define MIDIEVENT_PITCHBEND 2
#define MIDIEVENT_MODWHEEL 3

struct SampleCounter
{
	unsigned int sampleDataIndex;
	unsigned int adsrMode;
	double volume;

	unsigned int direction;
	int position; // samples

	double frequency;
	double destinationFrequency;

	bool silenced; // because of another note in mono mode
	bool finished;
};



struct NoteOn
{
	unsigned int velocity;
	unsigned int value;

	Time time;
};

struct CompletedNote
{
	unsigned int value;
	unsigned int velocity;

	unsigned int start; //  microSeconds
	unsigned int duration; //  microSeconds
};


struct Note			// A recorded note in a song
{
	unsigned int velocity;
	unsigned int value; // midi value

	unsigned int position; // pulses
	unsigned int duration; // pulses
	
};

struct PlayingNote
{
	unsigned int velocity;
	unsigned int value;
	
	//Note *note;

	std::vector <SampleCounter> counters; // one counter for each sample layer
	bool released; // key released
	bool countersFinished;
};

struct Phrase
{
	unsigned int beginBar, beginBeat, beginPulse;
	unsigned int endBar, endBeat, endPulse;
	
	std::vector <Note> notes;
	std::vector <Note> recordingNotes;
	bool recording;
};


struct TimeSignature
{
	unsigned short numerator, denominator;
};

struct MidiEvent
{
	unsigned int type;
	int value;
	unsigned int velocity;
	int bar;
	unsigned int beat, pulse;
};

struct Track
{
	unsigned int eventLoopIndex;
	bool eventLoop;

	bool recording;

	double volume, pan;
	std::vector <Phrase> phrases;

	std::vector <MidiEvent> events;
	std::vector <PlayingNote> playingNotes;

	Instrument *instrument;
};

#define bufferLength 2048



class Sequencer
{
private:

	Wrapper *wrapper_;
	bool done_;
	
	
	unsigned short ppq_;
	const double semiToneRatio_ = 1.0594630943592952645618252949463;


	int zeroCrossingAmount_;
	int samplesPerZeroCrossing_;

	short int leftBuffer_[bufferLength], rightBuffer_[bufferLength];
	short int sine_[sinelength];
	unsigned int sineIndex_ = 0;

	const double Pi_ = 3.1415926535897932384626433832795;
	//const double Pi_ = 3.14159265;
	
	double *sinc_;
	double *blackman_;
	
	// Access to instruments and samples
	std::vector <PlayingNote> playingNotes_;

	Lock *instrumentSampleLock_;



	// Sequencers internal mode

	
	int  bar_;
	unsigned int beat_;
	double pulse_;


	Instrument *metronomeInstrument_;

	bool instrumentPlayingOn_;
	Instrument *playingInstrument_;
	bool play_, rec_;
	
	bool countInModeOn_;
	bool recCountIn_;
	bool metronome_;

	int metronomeBeginBar_;
	unsigned int metronomeBeginBeat_, metronomeBeginPulse_;

	unsigned int metronomeTicks_;


	int recBeginBar_;
	unsigned int recBeginBeat_;
	unsigned int recBeginPulse_;
	TimeSignature recBeginSignature_;

	double tempo_;
	bool loop_;


	int loopBeginBar_, loopEndBar_;

	std::vector <TimeSignature> signatures_;
	TimeSignature currentSignature_;
	unsigned int measures_;

	Lock *sequencerModeLock_;
	

	// noteons
	std::vector <NoteOn> newNoteOns_; // 
	std::vector <NoteOn> noteOns_;
	std::vector <CompletedNote> completedNotes_;

	Lock *noteOnLock_;



	// Tracks
	std::vector <Track> tracks_;

	Track metronomeTrack_;
	
	Lock *trackLock_;



public:

	Sequencer(Wrapper *wrapper, Lock *instrumentSampleLock, Lock *noteOnLock, Lock *sequencerModeLock, Lock *trackLock, int zeroCrossingAmount, int samplesPerZeroCrossing) : wrapper_(wrapper), instrumentSampleLock_(instrumentSampleLock),
		noteOnLock_(noteOnLock), sequencerModeLock_(sequencerModeLock), trackLock_(trackLock),
		zeroCrossingAmount_(zeroCrossingAmount), samplesPerZeroCrossing_(samplesPerZeroCrossing),

		done_(false), metronome_(true), recCountIn_(true), countInModeOn_(false),
		bar_(0), beat_(0), pulse_(0), loopBeginBar_(0), loopEndBar_(2), play_(false), rec_(false), tempo_(90), loop_(true), ppq_(960), measures_(2), instrumentPlayingOn_(false), playingInstrument_(0),
		metronomeInstrument_(0), metronomeBeginBar_(0), metronomeBeginBeat_(0), metronomeBeginPulse_(0)
	{
		currentSignature_.numerator = 4;
		currentSignature_.denominator = 4;

		signatures_.clear();

		for (int index = 0; index < measures_; index++)
			signatures_.push_back(currentSignature_);
		

		recBeginSignature_ = currentSignature_;

		sinc_ = new double[(zeroCrossingAmount*samplesPerZeroCrossing) + 1];
		blackman_ = new double[(zeroCrossingAmount*samplesPerZeroCrossing) + 1];

		sinc_[0] = 1;

		for (unsigned int index = 1; index <= samplesPerZeroCrossing*zeroCrossingAmount; index++)
		{
			sinc_[index] = std::sin(Pi_*((double)index) / (double)samplesPerZeroCrossing) / (Pi_*(double)index / (double)samplesPerZeroCrossing);

		}

		double a0 = 0.42659;
		double a1 = 0.49656;
		double a2 = 0.076849;

		double NMinusOne = samplesPerZeroCrossing*zeroCrossingAmount;

		for (unsigned int index = 0; index <= samplesPerZeroCrossing*zeroCrossingAmount; index++)
		{
			blackman_[index] = a0 - a1*std::cos(Pi_ + 1 * Pi_*((double)index) / NMinusOne) + a2*std::cos(2*Pi_ + 2 * Pi_*((double)index) / NMinusOne);
		}

		for (unsigned int index = 0; index <= samplesPerZeroCrossing*zeroCrossingAmount; index++)
		{
			sinc_[index] = sinc_[index] * blackman_[index];
		}




		for (unsigned int index = 0; index < sinelength; index++)
		{
			sine_[index] = sin((double)index * 2 * Pi_ / (double)sinelength) * 10000;
		}

	}

	~Sequencer()
	{
		if (!done_)
			shutDown();
	}

	void shutDown()
	{
		if (wrapper_)
		{
			
		}

		delete[] sinc_;
		sinc_ = 0;

		delete[] blackman_;
		blackman_ = 0;
		
		done_ = true;
	}

	double getFrequency(double frequency, unsigned int rootNote, unsigned int value);

	TimeSignature getRecBeginSignature() { return recBeginSignature_; }
	int getBar() { return bar_; }
	unsigned int getBeat() { return beat_; }
	double getPulse() { return pulse_; }
	unsigned int getTrackAmount() { return tracks_.size(); }
	Track *getTrack(unsigned int number) { return &tracks_[number]; }

	bool getPlay() { return play_; }
	void setPlay(bool play);
	bool getRec() { return rec_; }
	void setRec(bool rec) { rec_ = rec; }
	
	// Playing instrument
	void setInstrumentPlayingOn(Instrument *instrument);
	void setInstrumentPlayingOff();

	void setMetronomeInstument(Instrument *metronome) { metronomeTrack_.instrument = metronomeInstrument_ = metronome; }

	// midi interface
	void noteOn(unsigned int value, unsigned int velocity, Time time);
	void noteOff(unsigned int value, Time time);

	// callback stuff
	static void callback_(void *sequencerObject, unsigned char *buffer, int len, int sampleRate);
	void callback(unsigned char *buffer, int len, int sampleRate);

	//
	double *getBlackman() { return blackman_; }
	double *getSinc() { return sinc_; }
	int roof(double number);
	int floor(double number);
	double abs(double number);
	int getSamplesPerZeroCrossing() { return samplesPerZeroCrossing_; }
	int getZeroCrossingAmount() { return zeroCrossingAmount_;  }

	void playNote(PlayingNote *note, Instrument *instrument, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate);

	// Track functions

	void newTrack();

	double pulsesPerMeasure(TimeSignature signature);
	
	double getBarPulse(double pulse, unsigned int beat);
	unsigned int INTEGER_getBarPulse(unsigned int pulse, unsigned int beat);

	unsigned int beatsPerMeasure(TimeSignature signature);
	unsigned int lastPulse(double pulsesPerMeasure);
	
	bool eventSmallerThan(MidiEvent event1, MidiEvent event2);
	void calculateNewPosition(double pulsesAdvanced, int bar, unsigned int beat, double pulse, int *endBar, unsigned int *endBeat, double *endPulse, bool *looped);
	void insertEvent(Track *track, MidiEvent event, bool looped);

	double samplesPerPulse(unsigned int sampleRate);
	double pulsesPerSample(unsigned int sampleRate);
	double pulseAmount(int beginBar, unsigned int beginBeat, double beginPulse, int endBar, unsigned int endBeat, double endPulse);

	int INTEGER_pulseAmount(int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse);


	int INTEGER_pulsesPerMeasure(TimeSignature signature);
	void INTEGER_calculateNewPosition(unsigned int pulsesAdvanced, int bar, unsigned int beat, unsigned int pulse, int *endBar, unsigned int *endBeat, unsigned int *endPulse, bool *looped);

	void roofFunction(int bar, unsigned int beat, double pulse, int *endBar, unsigned int *endBeat, unsigned int *endPulse, bool allowLooping);
	

	
	
	
	void trackNoteOn(Track *track, unsigned int value, unsigned int velocity);
	void trackNoteOff(Track *track, unsigned int value);


	void deleteEvents(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse);
	void deleteAllEvents(Track *track);

	void scanNotes(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse);
	void scanMetronome(Track *track, int beginBar, unsigned int beginBeat, unsigned int beginPulse, int endBar, unsigned int endBeat, unsigned int endPulse);
	void playNotes(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse);
	void playMetronome(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse);

	void createPlayingNote(std::vector <PlayingNote> *playingNotes, Instrument *instrument, unsigned int value, unsigned int velocity);
	void releasePlayingNote(std::vector <PlayingNote> *playingNotes, Instrument *instrument, unsigned int value);


	void stopRec(Track *track);
	void finishRecordingNote(Phrase *phrase, unsigned int index);
	Phrase *searchForPhrase(Track *track, unsigned int bar, unsigned int beat, unsigned int pulse);
	void continuePhrases(Track *track, unsigned int beginBar, unsigned int beginBeat, unsigned int beginPulse, unsigned int endBar, unsigned int endBeat, unsigned int endPulse, bool looped);
	void joinPhrases(Phrase *phrase1, Phrase *phrase2);

	void stopNotes(Track *track);

	void playTrack(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse, bool looped);
	void playMetronomeTrack(Track *track, short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate, unsigned int beginBar, unsigned int beginBeat, double beginPulse, unsigned int endBar, unsigned int endBeat, double endPulse, bool looped);

	void playSong(short int *leftBuffer, short int *rightBuffer, int sampleAmount, int sampleRate);
	void getNoteTime(unsigned int noteIndex, Phrase *phrase, int *bar, unsigned int *beat, unsigned int *pulse);
};

#endif