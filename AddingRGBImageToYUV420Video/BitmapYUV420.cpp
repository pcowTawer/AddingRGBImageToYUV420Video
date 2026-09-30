#include "BitmapYUV420.h"
#include <string>

BitmapYUV420::BitmapYUV420()
{
	yuv = nullptr;
	this->width = NULL;
	this->height = NULL;
	this->length = NULL;
}

BitmapYUV420::BitmapYUV420(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	this->length = width * height * 3 / 2;
	yuv = new uint8_t[length];
}

BitmapYUV420::~BitmapYUV420()
{
	if (yuv != nullptr) delete[] yuv;
}

std::ostream& operator<<(std::ostream& ostr, const BitmapYUV420& bitmap) {
	for (size_t i = 0; i < bitmap.length; ++i)
	{
		ostr << bitmap.yuv[i];
	};

	return ostr;
};

std::istream& operator>>(std::istream& istr, BitmapYUV420& bitmap) {
	if (bitmap.yuv != nullptr) delete[] bitmap.yuv;
	bitmap.yuv = new uint8_t[bitmap.length];
	istr.read(reinterpret_cast<char*>(bitmap.yuv), bitmap.length);
	return istr;
};

uint8_t& BitmapYUV420::getY(unsigned int i) {
	return *(this->yuv + 3 * i);
};
uint8_t& BitmapYUV420::getU(unsigned int i) {
	return *(this->yuv + 3 * i + 1);
};
uint8_t& BitmapYUV420::getV(unsigned int i) {
	return *(this->yuv + 3 * i + 2);
}
unsigned int BitmapYUV420::getLength() const
{
	return this->length;
};
