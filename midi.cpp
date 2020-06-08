#include "midi.h"


#ifdef _WIN32

MidiInDeviceInfo Midi::getMidiInDeviceInfo(unsigned int number) // get info from the devices found during scanning
{
	MidiInDeviceInfo info;
	info.name = inDevices_[number].name;
	info.wPid = inDevices_[number].wPid;
	info.wMid = inDevices_[number].wMid;

	return info;
}

void Midi::scanMidiInDevices() // this scans for devices that are available at the time
{
	inDevices_.clear();
	unsigned int amount = midiInGetNumDevs();

	for (unsigned int index = 0; index < amount; index++)
	{
		MidiInDevice device;

		MIDIINCAPS caps;
		midiInGetDevCaps(index, &caps, sizeof(MIDIINCAPS));

		WCHAR *pointer = caps.szPname;

		for (unsigned int index2 = 0; index2 < 32; index2++)
		{
			WCHAR wchar = pointer[index2];
			
			device.name += (char)wchar;
		}

		device.wMid = caps.wMid;
		device.wPid = caps.wPid;

		inDevices_.push_back(device);
	}
}


bool Midi::deviceIsOpen(std::string name)
{
	for (unsigned int index = 0; index < openMidiInDevices_.size(); index++)
	{
		if (openMidiInDevices_[index].name == name) return true;
	}

	return false;
}

bool Midi::openMidiInDevice(std::string name)
{
	// test if device is already open 

	for (unsigned int index = 0; index < openMidiInDevices_.size(); index++)
	{
		if (openMidiInDevices_[index].name == name)
		{
			if (openMidiInDevices_[index].handle) return true;	// device is already open
		}
	}
	

	// find the device that has the same name
	unsigned int amount = midiInGetNumDevs();

	for (unsigned int index = 0; index < amount; index++)
	{
		MIDIINCAPS caps;
		midiInGetDevCaps(index, &caps, sizeof(MIDIINCAPS));

		WCHAR *pointer = caps.szPname;

		std::string devName;

		for (unsigned int index2 = 0; index2 < 32; index2++)
		{
			WCHAR wchar = pointer[index2];

			devName += (char)wchar;
		}

		if (devName == name)
		{
			HMIDIIN handle;

			if (midiInOpen(&handle, index, (DWORD_PTR)&Midi::midiInCallbackFunction_, (DWORD_PTR)this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
			{
				if (midiInStart(handle) == MMSYSERR_NOERROR)
				{
					// the device was opened succesfully
					// create an opened device handle-name-pair and return true
					MidiInHandleNamePair pair;
					pair.handle = handle;
					pair.name = name;

					openMidiInDevices_.push_back(pair);
					return true;
				}
				else return false; // error in starting the device. return false
			}
			else return false; // error in opening the device. return false
		}

	}
	
	return false;
}

bool Midi::closeMidiInDevice(std::string name)
{
	// test if the device is really opened

	unsigned int index = 0;
	
	while (index < openMidiInDevices_.size())
	{
		bool closed = false;
		bool found = false;

		if (openMidiInDevices_[index].name == name)
		{
			found = true;
			if (midiInClose(openMidiInDevices_[index].handle) == MMSYSERR_NOERROR) closed = true;
		}

		if (found) // found the opened device
		{
			if (closed) { openMidiInDevices_.erase(openMidiInDevices_.begin() + index); return true; } // delete the device from the open devices list and return true
			else return false; // for some reason couldn't close the device. return false
		}
		else index++;
	}


	return true; // return true if the device wasn't really opened
}



void Midi::midiInCallbackFunction(
	HMIDIIN   hMidiIn,
	UINT      wMsg,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
	)
{
	if (wMsg != MIM_DATA) return;

	Time time;
	time.microSeconds = 0;

	unsigned char midiStatus;

	midiStatus = (unsigned char)(dwParam1 & 0xFF);

	unsigned char channel = midiStatus & 0x0F;
	unsigned char upperPart = (midiStatus & 0xF0) >> 4;

	unsigned char lowByte = (dwParam1 & 0xFF00) >> 8;
	unsigned char highByte = (dwParam1 & 0xFF0000) >> 16;


	
	if (upperPart == 8) // note Off
	{
		
		sequencer_->noteOff(lowByte, time);
	}
	else if (upperPart == 9) // noteOn
	{
		if (highByte == 0) sequencer_->noteOff(lowByte, time);
		else
		{
			sequencer_->noteOn(lowByte, highByte, time);
		}

		
	}
}


void Midi::midiInCallbackFunction_(
	HMIDIIN   hMidiIn,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
	)
{

	Midi *pointer = (Midi*)dwInstance;

	pointer->midiInCallbackFunction(hMidiIn, wMsg, dwParam1, dwParam2);
}


#else
#error NOT_WINDOWS
#endif