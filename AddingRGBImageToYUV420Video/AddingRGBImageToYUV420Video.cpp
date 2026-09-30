// AddingRGBImageToYUV420Video.cpp: определяет точку входа для приложения.
//

#include "AddingRGBImageToYUV420Video.h"
#include "YUV420ImageData.h"
#include "BMPImageData.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
//#define DEBUG

using namespace std;

struct BMPFileData {
	struct FileHeader {
		uint8_t magic[2];              // 'B' 'M'
		uint8_t file_size[4];
		uint8_t reserved[4];
		uint8_t offset[4];            // offset to bitmap data, relative to start of file
	};
	struct InfoHeader {
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

	FileHeader file_header;
	InfoHeader info_header;
	BMPImageData image_data;
};

struct Y4MFileData {
	string stream_header;
	vector<string> frame_headers;
	unsigned int width;
	unsigned int height;
	list<YUV420ImageData> frames;
};

class Y4MSerializer {
	// Source - https://stackoverflow.com/a/14266139
	// Posted by Vincenzo Pii, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-09-27, License - CC BY-SA 4.0
	vector<string> split(string s, const string& delimiter) {
		vector<string> tokens;
		size_t pos = 0;
		string token;
		while ((pos = s.find(delimiter)) != string::npos) {
			token = s.substr(0, pos);
			tokens.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		tokens.push_back(s);

		return tokens;
	}

public:
	bool deserialize(const string& filepath, Y4MFileData& data) {
		fstream file;
		file.open(filepath, fstream::in | ios::binary);
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
				data.width = stoi(tag.substr(1));
				break;
			case 'H':
				//Casting substring after 1st symbol to int
				data.height = stoi(tag.substr(1));
				break;
			default:
				continue;
				break;
			}
		}

		while (!file.eof()) {
			string frame_header;
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

class BMPSerializer {
	// Forms little endian uint32_t from array of uint8_t
	static constexpr uint32_t uint8x4_to_le_uint32(const uint8_t b[4]) noexcept {
		return  static_cast<uint32_t>(b[0])
			| static_cast<uint32_t>(b[1]) << 8
			| static_cast<uint32_t>(b[2]) << 16
			| static_cast<uint32_t>(b[3]) << 24;
	}

public:
	bool deserialize(const string& filepath, BMPFileData& data) {
		fstream file;
		file.open(filepath, fstream::in | ios::binary);
		file.read(reinterpret_cast<char*>(&data.file_header), sizeof(data.file_header));
		file.read(reinterpret_cast<char*>(&data.info_header), sizeof(data.info_header));

		uint32_t offset = uint8x4_to_le_uint32(data.file_header.offset);

#ifdef DEBUG
		cout << offset << endl;

		//BITMAPINFOHEADER size
		uint32_t header_size = uint8x4_to_le_uint32(data.info_header.header_size);

		cout << header_size << endl;
#endif
		uint32_t width = uint8x4_to_le_uint32(data.info_header.width);
		uint32_t height = uint8x4_to_le_uint32(data.info_header.height);

#ifdef DEBUG
		cout << width << endl;
		cout << height << endl;
#endif
		file.seekg(offset, ios::beg);

#ifdef DEBUG
		uint8_t rgb[3];
		for (size_t i = 0; !file.eof(); ++i)
		{
			file.read(reinterpret_cast<char*>(&rgb), sizeof(rgb));
			cout << i << ") " << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] << endl;
		}
		file.seekg(offset, ios::beg);
#endif
		BMPImageData image_data(width, height);
		file >> image_data;

		file.close();
		return true;
	}
};

int main()
{
	string y4m_filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\flower_cif.y4m";
	string bmp_filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\bmp-icon-64x64.bmp";

	BMPSerializer bmp_serializer;
	BMPFileData bpm_file_data;
	bmp_serializer.deserialize(bmp_filepath, bpm_file_data);

	Y4MSerializer y4m_serializer;
	Y4MFileData y4m_file_data;
	y4m_serializer.deserialize(y4m_filepath, y4m_file_data);

	return 0;
}