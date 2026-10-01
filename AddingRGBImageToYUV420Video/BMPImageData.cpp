#include "BMPImageData.h"

BMPImageData::BMPImageData()
{
	bytes = nullptr;
	this->width = NULL;
	this->height = NULL;
	set_length();
};

BMPImageData::BMPImageData(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	set_length();
	bytes = nullptr;
};

BMPImageData::~BMPImageData()
{
	if (bytes != nullptr) delete[] bytes;
};

uint8_t* BMPImageData::get_bytes() const
{
	return bytes;
}

unsigned int BMPImageData::get_width() const
{
	return width;
};

unsigned int BMPImageData::get_height() const
{
	return height;
};

unsigned int BMPImageData::get_length() const
{
	return length;
}

void BMPImageData::set_width(unsigned int width)
{
	this->width = width;
	set_length();
}

void BMPImageData::set_height(unsigned int height)
{
	this->height = height;
	set_length();
}

void BMPImageData::set_length()
{
	length = width * height * 3;
}

void BMPImageData::reinitialize()
{
	if (bytes != nullptr) delete[] bytes;
	bytes = new uint8_t[length];
};

std::istream& operator>>(std::istream& istr, BMPImageData& image_data) {
	if (image_data.bytes != nullptr) delete[] image_data.bytes;
	image_data.bytes = new uint8_t[image_data.length];
	//Read BMP in reverser order
	for (int i = image_data.height - 1; i >= 0; i--)
		istr.read(reinterpret_cast<char*>(image_data.bytes + i * image_data.width * 3), image_data.width * 3);
		
	return istr;
};
