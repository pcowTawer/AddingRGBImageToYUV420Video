// AddingRGBImageToYUV420Video.cpp: определяет точку входа для приложения.
//

#include "AddingRGBImageToYUV420Video.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Source - https://stackoverflow.com/a/14266139
// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
// Retrieved 2026-09-27, License - CC BY-SA 4.0
vector<std::string> split(std::string s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	tokens.push_back(s);

	return tokens;
}

class BitmapYUV420
{
public:
	BitmapYUV420();
	BitmapYUV420(unsigned int width, unsigned int height);
	~BitmapYUV420();

	uint8_t& getY(unsigned int i) {
		return *(this->yuv + 3 * i);
	};
	uint8_t& getU(unsigned int i) {
		return *(this->yuv + 3 * i + 1);
	};
	uint8_t& getV(unsigned int i) {
		return *(this->yuv + 3 * i + 2);
	};

	unsigned int getLength() {
		return this->length;
	}

	friend istream& operator>>(istream& istr, BitmapYUV420& bitmap);
	friend ostream& operator<<(ostream& ostr, const BitmapYUV420& bitmap);
private:
	uint8_t* yuv;
	unsigned int width;
	unsigned int height;
	unsigned int length;
};

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
	this->length = width * height * 3 / 2; //As specified in https://wiki.multimedia.cx/index.php?title=YUV4MPEG2#Frame_data for 4:2:0
	yuv = new uint8_t[length]; 
}

BitmapYUV420::~BitmapYUV420()
{
	if (yuv != nullptr) delete[] yuv;
}

ostream& operator<<(ostream& ostr, const BitmapYUV420& bitmap) {
	for (size_t i = 0; i < bitmap.length; i++)
	{
		ostr << bitmap.yuv[i];
	};
};

istream& operator>>(istream& istr, BitmapYUV420& bitmap) {
	delete[] bitmap.yuv;
	bitmap.yuv = new uint8_t[bitmap.length];
	for (size_t i = 0; i < bitmap.length; i++)
	{
		istr >> bitmap.yuv[i];
	}
	return istr;
};


int main()
{
	fstream fstr;

	string filename = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\flower_cif.y4m";

	fstr.open(filename, fstream::in | fstream::out);

	if (!fstr.is_open())
	{
		fstr.close();
		return -1;
	}

	string streamHeader = "";
	getline(fstr, streamHeader);
	vector<string> streamHeaderParams = split(streamHeader, " ");

	unsigned int width, height = 0;

	for (size_t i = 0; i < streamHeaderParams.size(); i++)
	{
		switch (streamHeaderParams[i][0])
		{
			case 'W':
				width = std::stoi(streamHeaderParams[i].substr(1));
				break;
			case 'H':
				height = std::stoi(streamHeaderParams[i].substr(1));
				break;
			default:
				continue;
				break;
		}
	}

	BitmapYUV420 frame(width, height);
	
	cout << frame.getLength();


	string frameHeader;
	getline(fstr, frameHeader);
	cout << frameHeader << endl;

	fstr >> frame;

	getline(fstr, frameHeader);
	cout << frameHeader << endl;

	fstr >> frame;

	getline(fstr, frameHeader);
	cout << frameHeader << endl;

	fstr.close();
	return 0;
}