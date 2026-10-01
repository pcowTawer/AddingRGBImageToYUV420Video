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
	bool serialize(const string& filepath, const Y4MFileData& data) {
		fstream file;
		file.open(filepath, fstream::out | ios::binary);
		if (!file.is_open()) {
			file.close();
			return false;
		}

		file << data.stream_header << "\n";
		for (size_t i = 0; i < data.frames.size(); ++i)
		{
			file << data.frame_headers[i] << "\n";
			auto it = data.frames.begin();
			std::advance(it, i);
			file.write(reinterpret_cast<const char*>((*it).get_bytes()), (*it).get_length());
		}
		file.close();
		return true;
	}

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
	static constexpr uint32_t uint8arr4_2_uint32le(const uint8_t b[4]) noexcept {
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

		uint32_t offset = uint8arr4_2_uint32le(data.file_header.offset);

#ifdef DEBUG
		cout << offset << endl;

		//BITMAPINFOHEADER size
		uint32_t header_size = uint8arr4_2_uint32le(data.info_header.header_size);

		cout << header_size << endl;
#endif
		data.image_data.set_width(uint8arr4_2_uint32le(data.info_header.width));
		data.image_data.set_height(uint8arr4_2_uint32le(data.info_header.height));

#ifdef DEBUG
		cout << data.image_data.get_width() << endl;
		cout << data.image_data.get_height() << endl;
#endif
		file.seekg(offset, ios::beg);
		
		file >> data.image_data;
#ifdef DEBUG
		uint8_t* bytes = data.image_data.get_bytes();
		for (size_t i = 0; i < data.image_data.get_length(); i+=3)
			cout << i/3 + 1 << ") " << (int)bytes[i] << " " << (int)bytes[i+1] << " " << (int)bytes[i+2] << endl;
#endif
		file.close();
		return true;
	}
};

class BMPtoYUVConverter {
	// Source - https://stackoverflow.com/q/9465815
	// Posted by Sambatyon, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-10-01, License - CC BY-SA 3.0
	public:
	void convert(uint8_t* destination, uint8_t* rgb, size_t width, size_t height)
	{
		size_t image_size = width * height;
		size_t upos = image_size;
		size_t vpos = upos + upos / 4;
		size_t i = 0;

		for (size_t line = 0; line < height; ++line)
		{
			if (!(line % 2))
			{
				for (size_t x = 0; x < width; x += 2)
				{
					uint8_t r = rgb[3 * i];
					uint8_t g = rgb[3 * i + 1];
					uint8_t b = rgb[3 * i + 2];

					destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

					destination[upos++] = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
					destination[vpos++] = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

					r = rgb[3 * i];
					g = rgb[3 * i + 1];
					b = rgb[3 * i + 2];

					destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
				}
			}
			else
			{
				for (size_t x = 0; x < width; x += 1)
				{
					uint8_t r = rgb[3 * i];
					uint8_t g = rgb[3 * i + 1];
					uint8_t b = rgb[3 * i + 2];

					destination[i++] = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
				}
			}
		}
	}
};


int main()
{
	
	string y4m_filepath;
	string bmp_filepath;
	string new_y4m_filepath;

	cout << "y4m filepath: ";
	getline(std::cin, y4m_filepath);
	if (y4m_filepath.empty()) y4m_filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\AddingRGBImageToYUV420Video\\flower_cif.y4m";
	cout << "bmp filepath: ";
	getline(std::cin, bmp_filepath);
	if (bmp_filepath.empty()) bmp_filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\AddingRGBImageToYUV420Video\\bmp-icon-64x64.bmp";
	cout << "new y4m name: ";
	getline(std::cin, new_y4m_filepath);
	if (new_y4m_filepath == "") new_y4m_filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\AddingRGBImageToYUV420Video\\flower_cif_with_icon.y4m";
	else new_y4m_filepath += ".y4m";

	try {
		BMPSerializer bmp_serializer;
		BMPFileData bpm_file_data;
		if (!bmp_serializer.deserialize(bmp_filepath, bpm_file_data)) throw std::runtime_error("Can open bmp file at " + bmp_filepath);

		Y4MSerializer y4m_serializer;
		Y4MFileData y4m_file_data;
		if (!y4m_serializer.deserialize(y4m_filepath, y4m_file_data)) 
			throw std::runtime_error("Can open bmp file at " + y4m_filepath);

		if (bpm_file_data.image_data.get_height() > y4m_file_data.height || bpm_file_data.image_data.get_width() > y4m_file_data.width)
			throw std::runtime_error("Error: Image size is larger than frame size");
		BMPtoYUVConverter converter;
		BMPImageData & bmp = bpm_file_data.image_data;
		YUV420ImageData converted_bmp(bmp.get_width(), bmp.get_height());
		converted_bmp.reinitialize();
		converter.convert(converted_bmp.get_bytes(), bmp.get_bytes(), bmp.get_width(), bmp.get_height());

		size_t upos1 = y4m_file_data.width * y4m_file_data.height;
		size_t vpos1 = upos1 + upos1 / 4;
		size_t upos2 = converted_bmp.get_width() * converted_bmp.get_height();
		size_t vpos2 = upos2 + upos2 / 4;

		for (auto& frame : y4m_file_data.frames) {
			for (size_t i = 0; i < converted_bmp.get_height(); ++i)
			{
				memcpy(
					frame.get_bytes() + i * frame.get_width(),
					converted_bmp.get_bytes() + i * converted_bmp.get_width(),
					converted_bmp.get_width()
				);
			};
			for (size_t i = 0; i < converted_bmp.get_height() / 2; ++i)
			{
				memcpy(
					frame.get_bytes() + upos1 + i * frame.get_width() / 2,
					converted_bmp.get_bytes() + upos2 + i * converted_bmp.get_width() / 2,
					converted_bmp.get_width() / 2
				);
			};
			for (size_t i = 0; i < converted_bmp.get_height() / 2; ++i)
			{
				memcpy(
					frame.get_bytes() + vpos1 + i * frame.get_width() / 2,
					converted_bmp.get_bytes() + vpos2 + i * converted_bmp.get_width() / 2,
					converted_bmp.get_width() / 2
				);
			};
		}

		if (!y4m_serializer.serialize(new_y4m_filepath, y4m_file_data)) 
			throw std::runtime_error("Can open file at " + new_y4m_filepath);
	}
	catch (exception& ex) { 
		cout << ex.what() << endl;
	}
	cout << "File saved at " << new_y4m_filepath;
	return 0;
}