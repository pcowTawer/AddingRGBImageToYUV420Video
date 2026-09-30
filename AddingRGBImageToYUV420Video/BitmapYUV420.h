#pragma once
#include <iostream>

class BitmapYUV420
{
public:
	BitmapYUV420();
	BitmapYUV420(unsigned int width, unsigned int height);
	~BitmapYUV420();

	unsigned int getLength() const;

	friend std::istream& operator>>(std::istream& istr, BitmapYUV420& bitmap);
private:
	uint8_t* image_data;
	unsigned int width;
	unsigned int height;
	unsigned int length;
};
