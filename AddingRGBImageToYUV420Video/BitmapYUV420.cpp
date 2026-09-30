#include "BitmapYUV420.h"
#include <string>

BitmapYUV420::BitmapYUV420()
{
	image_data = nullptr;
	this->width = NULL;
	this->height = NULL;
	this->length = NULL;
}

BitmapYUV420::BitmapYUV420(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	this->length = width * height * 3 / 2;
	image_data = new uint8_t[length];
}

BitmapYUV420::~BitmapYUV420()
{
	if (image_data != nullptr) delete[] image_data;
}

std::istream& operator>>(std::istream& istr, BitmapYUV420& bitmap) {
	if (bitmap.image_data != nullptr) delete[] bitmap.image_data;
	bitmap.image_data = new uint8_t[bitmap.length];
	istr.read(reinterpret_cast<char*>(bitmap.image_data), bitmap.length);
	return istr;
};

unsigned int BitmapYUV420::getLength() const
{
	return this->length;
};
