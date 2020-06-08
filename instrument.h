#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <vector>

#include "sample.h"

struct SampleData
{
	Sample *sample;
	std::string name;
	
	int rootNote, lowNote, highNote;
	int loopType;

	double attack, decay, sustain, release;
	int loopStart, loopEnd;

	bool poly, portamento;
	double portamentoRate;
};

class Instrument
{
private:
	
	std::vector <SampleData> sampleData_;
	std::string name_;
	unsigned int type_;
public:
	Instrument(std::string name) : name_(name), type_(0) { sampleData_.clear(); }
	~Instrument();


	void addSampleData(SampleData data) { sampleData_.push_back(data); }

	std::vector <SampleData> *getSampleData() { return &sampleData_; }
	
	std::string getName() { return name_; }
	void setName(std::string name) { name_ = name; }

	void setType(int type) { type_ = type; }
	int getType() { return type_; }


};

#endif