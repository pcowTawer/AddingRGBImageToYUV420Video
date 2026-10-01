#pragma once
#include <iostream>

class BMPImageData
{
	uint8_t* bytes;
	unsigned int width;
	unsigned int height;
	unsigned int length;

	void set_length();
public:
	BMPImageData();
	BMPImageData(unsigned int width, unsigned int height);
	~BMPImageData();

	uint8_t* get_bytes() const;
	unsigned int get_width() const;
	unsigned int get_height() const;
	unsigned int get_length() const;

	void set_width(unsigned int width);
	void set_height(unsigned int height);

	void reinitialize();

	friend std::istream& operator>>(std::istream& istr, BMPImageData& bitmap);
};
