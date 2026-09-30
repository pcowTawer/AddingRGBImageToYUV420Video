#include "YUV420ImageData.h"
#include <string>

YUV420ImageData::YUV420ImageData()
{
	bytes = nullptr;
	this->width = NULL;
	this->height = NULL;
	this->length = NULL;
}

YUV420ImageData::YUV420ImageData(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	this->length = width * height * 3 / 2;
	bytes = nullptr;
}

YUV420ImageData::~YUV420ImageData()
{
	if (bytes != nullptr) delete[] bytes;
}

std::istream& operator>>(std::istream& istr, YUV420ImageData& image_data) {
	if (image_data.bytes != nullptr) delete[] image_data.bytes;
	image_data.bytes = new uint8_t[image_data.length];
	istr.read(reinterpret_cast<char*>(image_data.bytes), image_data.length);
	return istr;
};

unsigned int YUV420ImageData::getLength() const
{
	return this->length;
};
