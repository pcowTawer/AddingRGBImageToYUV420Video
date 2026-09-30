// AddingRGBImageToYUV420Video.cpp: определяет точку входа для приложения.
//

#include "AddingRGBImageToYUV420Video.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include "BitmapYUV420.h"

using namespace std;

struct StreamData {
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

		string streamHeader = "";
		getline(file, streamHeader);
		vector<string> streamTags = split(streamHeader, " ");
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
			string frameHeader;
			getline(file, frameHeader);
			data.frames.emplace_back(data.width, data.height);
			file >> data.frames.back();
			file.peek(); //Need to make eofbit correct
		}

		file.close();
		return true;
	}
	
	//TODO
	//serialize()
};


int main()
{
	string filepath = "C:\\Users\\pcow\\Documents\\my-cpp-projects\\AddingRGBImageToYUV420Video\\out\\build\\x64-debug\\AddingRGBImageToYUV420Video\\flower_cif.y4m";
	
	YUV4MPEG2Serializer serializer;
	StreamData data;
	serializer.deserialize(filepath, data);
	return 0;
}