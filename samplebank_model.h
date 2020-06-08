#ifndef SAMPLEBANK_MODEL_H
#define SAMPLEBANK_MODEL_H

#include "sample.h"

class SampleBankModel
{
private:
	std::vector <Sample *> *samples_;
	unsigned int sampleNumber_;
public:
	SampleBankModel(std::vector <Sample *> *samples) : samples_(samples) {}
	void setSampleNumber(unsigned int number);
	unsigned int getSampleNumber();

	Sample *getSample();
	bool loadSample(std::string fileName);
	
};

#endif