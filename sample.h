#ifndef SAMPLE_H
#define SAMPLE_H

#include <string>

class Sample
{
private:
	std::string name_;
	short *data_;
	unsigned int length_;
	unsigned int sampleRate_;
public:
	Sample() : name_(""), data_(0), length_(0), sampleRate_(0) {}

	void setName(std::string name) { name_ = name; }
	void setData(signed short *data) { data_ = data; }
	void setLength(unsigned int length) { length_ = length; }
	void setSampleRate(unsigned int sampleRate) { sampleRate_ = sampleRate; }
	void releaseSample();

	std::string getName() { return name_; }
	signed short *getData() { return data_; }
	unsigned int getSampleRate() { return sampleRate_; }
	unsigned int getLength() { return length_; }
	
};
#endif