#pragma once
#include <iostream>

class YUV420ImageData
{
	uint8_t* bytes;
	unsigned int width;
	unsigned int height;
	unsigned int length;

	void set_length();
public:
	YUV420ImageData();
	YUV420ImageData(unsigned int width, unsigned int height);
	~YUV420ImageData();

	uint8_t* get_bytes() const;
	unsigned int get_width() const;
	unsigned int get_height() const;
	unsigned int get_length() const;

	void set_width(unsigned int width);
	void set_height(unsigned int height);

	void reinitialize();

	friend std::istream& operator>>(std::istream& istr, YUV420ImageData& bitmap);
};
