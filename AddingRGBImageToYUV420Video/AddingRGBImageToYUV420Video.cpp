// AddingRGBImageToYUV420Video.cpp: определяет точку входа для приложения.
//

#include "AddingRGBImageToYUV420Video.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include "BitmapYUV420.h"

using namespace std;

struct BitmapFileHeader {
	uint8_t magic[2];              // 'B' 'M'
	uint8_t file_size[4];
	uint8_t reserved[4];
	uint8_t offset[4];            // offset to bitmap data, relative to start of file
};

struct BitmapInfoHeader {
	uint8_t header_size[4];                // 40
	uint8_t width[4];
	uint8_t height[4];                     // if height > 0 then bitmap data is upside down (i.e. last scanline first)
	// else height = abs(height) and bitmap data is ordered normally
	uint8_t planes[2];                     // 1
	uint8_t bits_per_pixel[2];             // must be 0 (bpp is implied by embedded JPEG or PNG image), 1, 4, 8, 16, 24 or 32
	uint8_t compression[4];                // 0 = uncompressed, 1 = RLE8, 2 = RLE4, 3 = bitfields, 4 = JPEG, 5 = PNG
	uint8_t bitmap_data_size[4];
	uint8_t hor_pixels_per_meter[4];
	uint8_t vert_pixels_per_meter[4];
	uint8_t number_of_colors[4];           // only relevant for bits_per_pixel < 16; if set to zero, it's assumed to be 2^bits_per_pixel
	uint8_t number_of_important_colors[4]; // 0 means all colors are important
};

struct RGBTriple {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};


struct StreamData {
	string stream_header;
	vector<string> frame_headers;
	unsigned int width;
	unsigned int height;
	list<BitmapYUV420> frames;
};

class YUV4MPEG2Serializer {
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


public:
	bool deserialize(const string& filepath, StreamData& data) {
		fstream file;
		file.open(filepath, fstream::in | std::ios::binary);
		if (!file.is_open()) {
			file.close();
			return false;
		}

		getline(file, data.stream_header);
		vector<string> streamTags = split(data.stream_header, " ");
		streamTags.erase(streamTags.begin());
		for (auto &tag : streamTags) {
			switch (tag[0])
			{
			case 'W':
				//Casting substring after 1st symbol to int
				data.width = std::stoi(tag.substr(1));
				break;
			case 'H':
				//Casting substring after 1st symbol to int
				data.height = std::stoi(tag.substr(1));
				break;
			default:
				continue;
				break;
			}
		}

		while (!file.eof()) {
			std::string frame_header;
			getline(file, frame_header);
			data.frame_headers.push_back(frame_header);
			data.frames.emplace_back(data.width, data.height);
			file >> data.frames.back();
			file.peek(); //Need to make eofbit correct
		}

		file.close();
		return true;
	}

};

// Forms little endian uint32_t from array of uint8_t
static constexpr uint32_t uint8_to_le_uint32(const uint8_t b[4]) noexcept {
	return  static_cast<uint32_t>(b[0])
		| static_cast<uint32_t>(b[1]) << 8
		| static_cast<uint32_t>(b[2]) << 16
		| static_cast<uint32_t>(b[3]) << 24;
}

int main()
{
	string filepathYUV = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\flower_cif.y4m";
	string filepathBMP = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\bmp-icon-64x64.bmp";

	fstream file;
	file.open(filepathBMP, fstream::in | std::ios::binary);
	BitmapFileHeader file_header;
	BitmapInfoHeader info_header;
	RGBTriple rgb;
	file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
	file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

	uint32_t offset = uint8_to_le_uint32(file_header.offset);

	cout << offset << endl;
	
	// Тут я выяснил, что у меня BITMAPINFOHEADER
	uint32_t header_size = uint8_to_le_uint32(info_header.header_size);
	
	cout << header_size << endl;

	uint32_t width = uint8_to_le_uint32(info_header.width);
	uint32_t height = uint8_to_le_uint32(info_header.height);

	cout << width << endl;
	cout << height << endl;

	file.seekg(offset, std::ios::beg);
	
	for (size_t i = 0; !file.eof(); ++i)
	{
		file.read(reinterpret_cast<char*>(&rgb), sizeof(rgb));
		cout << i << ") " << (int)rgb.red << " " << (int)rgb.green << " " << (int)rgb.blue << endl;
	}

	//YUV4MPEG2Serializer serializer;
	//StreamData data;
	//serializer.deserialize(filepathYUV, data);

	return 0;
}