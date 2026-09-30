#pragma once
#include <iostream>

class YUV420ImageData
{
public:
	YUV420ImageData();
	YUV420ImageData(unsigned int width, unsigned int height);
	~YUV420ImageData();

	unsigned int getLength() const;

	friend std::istream& operator>>(std::istream& istr, YUV420ImageData& bitmap);
private:
	uint8_t* bytes;
	unsigned int width;
	unsigned int height;
	unsigned int length;
};
