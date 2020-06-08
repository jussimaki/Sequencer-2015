#ifndef MIDI_H
#define MIDI_H

#include <string>
#include <vector>

#ifdef _WIN32

#include <windows.h>

#include <mmsystem.h>

struct MidiInDeviceInfo
{
	std::string name;
	short int wMid;
	short int wPid;
};

struct MidiInDevice
{
	short int wMid;
	short int wPid;

	std::string name;
};

struct MidiInHandleNamePair
{
	HMIDIIN handle;

	std::string name;	
};

#else
#error NOT_WINDOWS
#endif


#include "sequencer.h"






class Midi
{
private:
	std::vector <MidiInDevice> inDevices_;
	std::vector <MidiInHandleNamePair> openMidiInDevices_;
	Sequencer *sequencer_;

public:
	Midi(Sequencer *sequencer) : sequencer_(sequencer) {}

	void scanMidiInDevices();


	bool deviceIsOpen(std::string name);
	bool openMidiInDevice(std::string name);
	bool closeMidiInDevice(std::string name);


	void midiInCallbackFunction(
		HMIDIIN   hMidiIn,
		UINT      wMsg,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2
		);

	static void midiInCallbackFunction_(
		HMIDIIN   hMidiIn,
		UINT      wMsg,
		DWORD_PTR dwInstance,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2
		);

	unsigned int getMidiInDeviceAmount() { return inDevices_.size(); }
	MidiInDeviceInfo getMidiInDeviceInfo(unsigned int number);
};



#endif