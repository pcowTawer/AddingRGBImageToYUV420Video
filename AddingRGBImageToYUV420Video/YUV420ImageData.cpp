#include "YUV420ImageData.h"

YUV420ImageData::YUV420ImageData()
{
	bytes = nullptr;
	this->width = NULL;
	this->height = NULL;
	set_length();
};

YUV420ImageData::YUV420ImageData(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	set_length();
	bytes = nullptr;
};

YUV420ImageData::~YUV420ImageData()
{
	if (bytes != nullptr) delete[] bytes;
};

uint8_t* YUV420ImageData::get_bytes() const
{
	return bytes;
}

unsigned int YUV420ImageData::get_width() const
{
	return width;
};

unsigned int YUV420ImageData::get_height() const
{
	return height;
};

unsigned int YUV420ImageData::get_length() const
{
	return length;
}

void YUV420ImageData::set_width(unsigned int width)
{
	this->width = width;
	set_length();
}

void YUV420ImageData::set_height(unsigned int height)
{
	this->height = height;
	set_length();
}

void YUV420ImageData::set_length()
{
	length = width * height * 3 / 2;
}

void YUV420ImageData::reinitialize()
{
	if (bytes != nullptr) delete[] bytes;
	bytes = new uint8_t[length];
};

std::istream& operator>>(std::istream& istr, YUV420ImageData& image_data) {
	if (image_data.bytes != nullptr) delete[] image_data.bytes;
	image_data.bytes = new uint8_t[image_data.length];
	istr.read(reinterpret_cast<char*>(image_data.bytes), image_data.length);
	return istr;
};
