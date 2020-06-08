#include "sample.h"

void Sample::releaseSample()
{
	if (data_)
	{
		delete[] data_;
		data_ = 0;
	}

	length_ = 0;
}