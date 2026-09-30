#pragma once
#include <iostream>

class BitmapYUV420
{
public:
	BitmapYUV420();
	BitmapYUV420(unsigned int width, unsigned int height);
	~BitmapYUV420();

	uint8_t& getY(unsigned int i);
	uint8_t& getU(unsigned int i);
	uint8_t& getV(unsigned int i);
	unsigned int getLength() const;

	friend std::istream& operator>>(std::istream& istr, BitmapYUV420& bitmap);
	friend std::ostream& operator<<(std::ostream& ostr, const BitmapYUV420& bitmap);
private:
	uint8_t* yuv;
	unsigned int width;
	unsigned int height;
	unsigned int length;
};
