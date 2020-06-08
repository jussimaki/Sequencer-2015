#include "wrapper.h"
#include "window.h"
#include "printer.h"

#include <sstream>
#include <fstream>
#include "sample.h"
#include "instrument.h"

#include "instrument_window.h"
#include "instrument_controller.h"

#include "sound_system.h"
#include "sequencer.h"
#include "midi.h"

#undef main

int debugInt;

int main(int argc, char *argv[])
{
	Wrapper wrapper;

	Lock instrumentSampleLock, noteOnLock, sequencerModeLock, trackLock;

	wrapper.initLock(&instrumentSampleLock);
	wrapper.initLock(&noteOnLock);
	wrapper.initLock(&sequencerModeLock);
	wrapper.initLock(&trackLock);

	unsigned int sampleRate = 44100;

	SoundSystem soundSystem(sampleRate);

	
	Sequencer sequencer(&wrapper, &instrumentSampleLock, &noteOnLock, &sequencerModeLock, &trackLock, 64, 128);

	

	Instrument *metronome =  new Instrument("Metronome");

	Sample *metronomeSample = new Sample;
	Wave metronomeWave;

	wrapper.loadWave(&metronomeWave, "metronome.wav");

	metronomeSample->setLength(metronomeWave.samplesPerChannel);
	metronomeSample->setSampleRate(metronomeWave.sampleRate);
	metronomeSample->setData(metronomeWave.buffer);
	metronomeSample->setName("metronomesample");

	SampleData metronomeSampleData;
	metronomeSampleData.sample = metronomeSample;
	metronomeSampleData.attack = 0.0001;
	metronomeSampleData.decay = 0.1;
	metronomeSampleData.sustain = 1;
	metronomeSampleData.release = 0.01;

	metronomeSampleData.loopStart = 0;
	metronomeSampleData.loopEnd = metronomeWave.samplesPerChannel - 1;
	metronomeSampleData.loopType = 0;

	metronomeSampleData.name = "metronomesampledata";
	metronomeSampleData.rootNote = 0;
	metronomeSampleData.lowNote = 0;
	metronomeSampleData.highNote = 127;

	metronomeSampleData.poly = true;
	metronomeSampleData.portamento = false;
	metronomeSampleData.portamentoRate = 1.1;

	metronome->addSampleData(metronomeSampleData);


	sequencer.setMetronomeInstument(metronome);



	sequencer.newTrack();



	Midi midi(&sequencer);

	std::ifstream file;
	file.open("midi.txt");

	int midiNumber;

	file >> midiNumber;

	file.close();

	midi.scanMidiInDevices();

	std::string midiInName = "empty";

	if (midi.getMidiInDeviceAmount() > midiNumber)
	{
		midiInName = midi.getMidiInDeviceInfo(midiNumber).name;

		midi.openMidiInDevice(midiInName);
	}



	soundSystem.setSequencer(Sequencer::callback_, &sequencer);

	std::vector <Instrument *> instruments;
	std::vector <Sample *> samples;
	

	
	file.open("resoluutio.txt");

	unsigned int xRes, yRes;

	file >> xRes;
	file >> yRes;

	file.close();

	wrapper.setSoundSystem(SoundSystem::callback_, &soundSystem);
	wrapper.initialize(xRes, yRes, sampleRate);
	//wrapper.initialize(1920, 1080);
	//wrapper.initialize(1680,1050);
	//wrapper.initialize(1024,768);

	int mouse_x = wrapper.getXResolution()/2;
	int mouse_y = wrapper.getYResolution()/2;

	
	Sprite alphabet;
	wrapper.loadSprite(&alphabet, "alphabet.bmp");


	#include "basic_alphabet_table.h"
	Printer basicPrinter((int *)&basic_alphabet_table, 32, 126, &wrapper, &alphabet);
	Windows system(&wrapper, &basicPrinter);



	InstrumentController instrumentController(&system, &sequencer, &instruments, &samples, &instrumentSampleLock, &sequencerModeLock);
	InstrumentLayout instrumentLayout(&system, &instrumentController);

	instrumentController.openController();
	instrumentLayout.openInstrumentWindow();





	unsigned int octave = 3;
	bool MIDIKEYS[128];

	for (int index = 0; index < 128; index++) MIDIKEYS[index] = 0;

	Timer timer;
	wrapper.startTimer(&timer);

	while(true)
	{
		Track *track = sequencer.getTrack(0);

		wrapper.clearScreen();
		wrapper.readInput();

		if(wrapper.getInputData()->KEY_DOWN[KEY_ESCAPE]) break;

		
		system.logic();
		system.draw();

		Input input;

		inputFunctionality(&wrapper, &input);

		char KEYS[17] = { KEY_q, KEY_2, KEY_w, KEY_3, KEY_e, KEY_r, KEY_5, KEY_t, KEY_6, KEY_y, KEY_7, KEY_u, KEY_i, KEY_9, KEY_o, KEY_0, KEY_p };

		Time time;

		time.microSeconds = 0;

		if (input.KEY_PRESSED[KEY_MINUS])
		{
			if (octave > 0)
			{
				for (int index = 0; index < 128; index++)
				{
					if (MIDIKEYS[index]) sequencer.noteOff(index, time);

					MIDIKEYS[index] = false;
				}
				octave--;
			}
		}
		if (input.KEY_PRESSED[KEY_EQUALS]) 
		{
			if (octave < 10)
			{
				for (int index = 0; index < 128; index++)
				{
					if (MIDIKEYS[index]) sequencer.noteOff(index, time);

					MIDIKEYS[index] = false;
				}
				octave++;
			}
		}
		
		for (int index = 0; index < 17; index++)
		{
			if (input.KEY_PRESSED[KEYS[index]])
			{
				if (octave * 12 + index < 128)
				{
					if (!MIDIKEYS[octave * 12 + index]) sequencer.noteOn(octave * 12 + index, 127, time);

					MIDIKEYS[octave * 12 + index] = true;
					wrapper.startTimer(&timer);
				}
			}

			if (input.KEY_RELEASED[KEYS[index]])
			{
				if (octave * 12 + index < 128)
				{
					if (MIDIKEYS[octave * 12 + index]) sequencer.noteOff(octave * 12 + index, time);

					MIDIKEYS[octave * 12 + index] = false;
				}
			}
		}

		if (input.KEY_PRESSED[KEY_SPACE])
			sequencer.setPlay(!sequencer.getPlay());

		if (input.KEY_PRESSED[KEY_LCTRL])
			sequencer.setRec(!sequencer.getRec());

		if (input.KEY_PRESSED[KEY_RCTRL])
		{
			track->phrases.clear();
		}


		std::stringstream stream;

		stream << octave;

		std::string string;

		stream >> string;

		basicPrinter.print((char*)string.c_str(), 1000, 500, 0);



		double seconds = wrapper.getTimerSeconds(&timer);

		stream.clear();
		string.clear();

		stream << seconds;

		stream >> string;

		basicPrinter.print((char*)string.c_str(), 1000, 600, 0);


		

		if (midi.deviceIsOpen(midiInName))
		{
			std::string string;

			string = "Device ";

			unsigned int index = 0;
			while (midiInName[index] != '\0')
			{
				string += midiInName[index];
				index++;
			}

			string += " is open!";

			basicPrinter.print((char*)string.c_str(), 1200, 100, 0);
		}

		unsigned int amount = midi.getMidiInDeviceAmount();

		for (unsigned int index = 0; index < amount; index++)
		{
			MidiInDeviceInfo device = midi.getMidiInDeviceInfo(index);
			basicPrinter.print((char*)device.name.c_str(), 1000, 700 + index*30, 0);

			stream.clear();
			string.clear();


			stream << device.wMid;
			stream >> string;

			std::string string2;
			string2 = "wMid: " + string;

			basicPrinter.print((char*)string2.c_str(), 1200, 700 + index * 30, 0);

			stream.clear();
			string.clear();

			stream << device.wPid;
			stream >> string;

			string2 = "wPid: " + string;

			basicPrinter.print((char*)string2.c_str(), 1300, 700 + index * 30, 0);

			


			
		}
		
		std::string string2;

		stream.clear();
		string.clear();

		stream << sequencer.getRec();
		stream >> string;

		string2 = "Rec: " + string;

		basicPrinter.print((char*)string2.c_str(), 1000, 50, 0);

		stream.clear();
		string.clear();

		stream << sequencer.getBar();
		stream >> string;

		string2 = "Bar: " + string;

		basicPrinter.print((char*)string2.c_str(), 1000, 100, 0);

		stream.clear();
		string.clear();

		stream << sequencer.getBeat();
		stream >> string;

		string2 = "Beat: " + string;

		basicPrinter.print((char*)string2.c_str(), 1000, 120, 0);

		stream.clear();
		string.clear();

		stream << sequencer.getPulse();
		stream >> string;

		string2 = "Pulse: " + string;

		basicPrinter.print((char*)string2.c_str(), 1000, 140, 0);


		stream.clear();
		string.clear();

		stream << track->recording;
		stream >> string;

		string2 = "trackrec: " + string;

		basicPrinter.print((char*)string2.c_str(), 1000, 160, 0);



		stream.clear();
		string.clear();

		
		string2 = "debugdumber: ";

		basicPrinter.print((char*)string2.c_str(), 1000, 450, 0);
		



		stream.clear();
		string.clear();

		stream << debugInt;



		stream >> string;

		string2 = "pulses: " + string;

		basicPrinter.print((char*)string2.c_str(), 0, 0, 0);

		// visualise phrases
		
		Rect dest;
		dest.x = 250 + sequencer.INTEGER_pulseAmount(-1, 0, 0, sequencer.getBar(), sequencer.getBeat(), sequencer.getPulse()) / 15;
		dest.width = 1;

		dest.y = 280;
		dest.height = 20;

		wrapper.drawRectangle(&dest, 0, 0, 255, 0, 255);

		for (unsigned int index = 0; index < track->phrases.size(); index++)
		{
			MidiEvent begin;
			begin.bar = 0;
			begin.beat = 0;
			begin.pulse = 0;


			Phrase *phrase = &track->phrases[index];

			MidiEvent phraseBegin;
			phraseBegin.bar = phrase->beginBar;
			phraseBegin.beat = phrase->beginBeat;
			phraseBegin.pulse = phrase->beginPulse;

			MidiEvent phraseEnd;
			phraseEnd.bar = phrase->endBar;
			phraseEnd.beat = phrase->endBeat;
			phraseEnd.pulse = phrase->endPulse;

			Rect dest;
			dest.x = 250 + sequencer.INTEGER_pulseAmount(-1, 0, 0, phrase->beginBar, phrase->beginBeat, phrase->beginPulse) / 15;
			dest.width = sequencer.INTEGER_pulseAmount(phrase->beginBar, phrase->beginBeat, phrase->beginPulse, phrase->endBar, phrase->endBeat, phrase->endPulse) / 15;

			dest.y = 300 +40 * index;
			dest.height = 40;


			unsigned char greenBlue = 255;
			if (phrase->recording) greenBlue = 0;
			wrapper.drawRectangle(&dest, 0, 255, greenBlue, greenBlue, 128);


			int smallest = 127;
			int largest = 0;

			for (unsigned int noteIndex = 0; noteIndex < phrase->notes.size(); noteIndex++)
			{
				Note *note = &phrase->notes[noteIndex];

				if (note->value > largest) largest = note->value;
				if (note->value < smallest) smallest = note->value;
			}

			int difference = largest - smallest;

			int medium = (largest + smallest) / 2;

			if (difference < 0) difference = -difference;

			if (difference == 0) difference = 1;

			for (unsigned int noteIndex = 0; noteIndex < phrase->notes.size(); noteIndex++)
			{
				Note *note = &phrase->notes[noteIndex];


				dest.x = 250 + sequencer.INTEGER_pulseAmount(-1, 0, 0, phrase->beginBar, phrase->beginBeat, phrase->beginPulse) / 15 + note->position/15;
				dest.width = note->duration/15;


				dest.y = 300 + 20 - (((int)note->value - medium)*(20 - 2) * 2 / difference) + 40 * index;
				dest.height = 1;

				wrapper.drawRectangle(&dest, 0, 255, 0, 0, 255);
			}

		}

		/*
		int zeroCrossingAmount = sequencer.getZeroCrossingAmount();
		int samplesPerZeroCrossing = sequencer.getSamplesPerZeroCrossing();

		for (int index = 0; index <= samplesPerZeroCrossing*zeroCrossingAmount; index++)
		{
			Rect dest;

			dest.x = 100 + index*10;
			dest.y = 500 - sequencer.getSinc()[index] * 200;

			dest.height = 500 - dest.y;

			dest.width = 1;

			wrapper.drawRectangle(&dest, 0, 255, 128, 0, 255);


		}
		*/

		wrapper.flip();
	}
	

	wrapper.releaseSprite(&alphabet);

	
	
	sequencer.shutDown();
	
	wrapper.shutDownSound(); // shuts down sound system before the locks are deleted


	for (unsigned int index = 0; index < samples.size(); index++)
	{
		(*samples[index]).releaseSample();
		delete samples[index];
	}

	samples.clear();


	wrapper.deleteLock(&instrumentSampleLock);
	wrapper.deleteLock(&noteOnLock);
	wrapper.deleteLock(&sequencerModeLock);
	wrapper.deleteLock(&trackLock);

	wrapper.shutDown();
	
	return 0;
}