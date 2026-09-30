#pragma once
#include <iostream>

class BMPImageData
{
public:
	BMPImageData();
	BMPImageData(unsigned int width, unsigned int height);
	~BMPImageData();

	unsigned int getLength() const;

	friend std::istream& operator>>(std::istream& istr, BMPImageData& bitmap);
private:
	uint8_t* bytes;
	unsigned int width;
	unsigned int height;
	unsigned int length;
};