#include "BMPImageData.h"

BMPImageData::BMPImageData()
{
	bytes = nullptr;
	this->width = NULL;
	this->height = NULL;
	this->length = NULL;
}

BMPImageData::BMPImageData(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	this->length = width * height * 3;
	bytes = nullptr;
}

BMPImageData::~BMPImageData()
{
	if (bytes != nullptr) delete[] bytes;
}

unsigned int BMPImageData::getLength() const
{
	return this->length;
};

std::istream& operator>>(std::istream& istr, BMPImageData& image_data) {
	if (image_data.bytes != nullptr) delete[] image_data.bytes;
	image_data.bytes = new uint8_t[image_data.length];
	istr.read((char*)image_data.bytes, image_data.length);
	return istr;
};