/*
    MIT License

    Copyright (c) 2016-2024, Alexandr Kuznetsov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <cmath>
#include <bitset>

#include "lib/bconv.h"
#include "lib/lodepng.h"

using namespace std;
using namespace filesystem;
using namespace lodepng;
using namespace bconv;

e_system_numeration sn_mode = E_SYSTEM_NUMERATION_HEX;
bool show_info = true;

void displayImageLightness(const vector<unsigned char>& image, unsigned w, unsigned h)
{
  if(w > 0 && h > 0) {
	cout << "[ PREVIEW ]" << endl;
	
    unsigned w2 = 80;
	
    if(w < w2) w2 = w;
	
    unsigned h2 = h * w2 / w;

    cout << '+';
    for(unsigned x = 0; x < w2 * 2; x++) 
		cout << '-';
		
    cout << '+' << endl;
	
    for(unsigned y = 0; y < h2; y++) {
		
      cout << "|";
	  
      for(unsigned x = 0; x < w2; x++) {
        unsigned x2 = x * w / w2;
        unsigned y2 = y * h / h2;
		int index = y2 * w * 4 + x2 * 4;
		
        int r = image[index + 0];
        int g = image[index + 1];
        int b = image[index + 2];
        int a = image[index + 3];
        int lightness = ((r + g + b) / 3) * a / 255;
		
		int symbol = ' ';
				        
        if(lightness > 128)
			symbol = 0x30;
		
        cout << (char)symbol << (char)symbol;
      }
	  
      cout << "|";
      cout << endl;
    }
	
    cout << '+';
	
    for(unsigned x = 0; x < w2 * 2; x++)
		cout << '-';
	
    cout << '+' << endl << endl;
  }
}

void displayIntCode(unsigned int b)
{
	switch (sn_mode) {
		case E_SYSTEM_NUMERATION_BIN:
			cout << "0b" << (bitset<8>)b;
		break;
		case E_SYSTEM_NUMERATION_DEC:
			cout << b;
		break;
		case E_SYSTEM_NUMERATION_HEX:		
			cout << "0x";
			if (b < 16) cout << "0";
			cout << hex << uppercase << b;	
		break;		
	}
}

void displayArrayBegin(unsigned array_size)
{
	if (show_info) {
		cout << "[ ARRAY ]" << endl;
		cout << endl;				
		cout << "const uint8_t array_png[" << array_size << "] PROGMEM = {";
		cout << endl << endl;
	}
}

void displayArrayEnd()
{
	if (show_info) {
		cout << endl << endl;		
		cout << "};";
		cout << endl << endl;	
	}
}

void displayArray(const vector<unsigned char>& image, unsigned w, unsigned h, e_direction_array mode)
{
	if(w > 1 && h > 7) {
		unsigned int b = 0;
		unsigned int lightness = 0;				
		int total_pixels = w * h;
		int array_size = floor(total_pixels / 8);		
		
		displayArrayBegin(array_size);
		
		for (unsigned i = 0; i < array_size; i++) {
			unsigned int b = 0;
			unsigned int lightness = 0;

			for (unsigned j = 0; j < 8; j++) {
				int index;
				switch(mode) {
					case E_DIRECTION_ARRAY_VERTICAL:
						index = (floor(i / w * 8) + j) * w * 4 + i % w  * 4;
					break;
					case E_DIRECTION_ARRAY_HORISONTAL:
						index = ((i * 8) + j) * 4;
					break;
				}
				
				lightness = (image[index] + image[index + 1] + image[index + 2]) / 3; // RGB
				
				if (lightness > 127)
					b |= 1 << j;
			}
			
			displayIntCode(b);
			
						
			if (i < array_size - 1)
				cout << ", ";
			
			if ((i + 1) % 16 == 0)
				cout << endl;		
		}
		
		displayArrayEnd();
	}
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru_RU.UTF-8");
	
	s_prog_argument args[] = {
		{"b", "binary format.", false}, 
		{"d", "decimal format.", false},
		{"i", "no info format.", false},
		
		{"p", "preview image mode.", false}, 
		{"v", "vertical mode.", false},
		{"h", "horisont mode.", false}
	};
		
    if (argc < 2) {
		// No arguments - Usage help
		cout << "Usage: bconv.exe *.png";
		for (auto& a : args) {
			cout << " [/" << a.s_name << "]";
		}
		cout << endl << endl;

		// Notices
		for (auto& a : args) {
			cout << "/" << a.s_name << " " << a.s_notice << endl;
		}

		cout << endl;
    } else {	
		string s_file_name = argv[1];
		
		// Fill arguments struct
		for (int i = 2; i < argc; i++)
		{
			string s = strtolower(argv[i]);
			for (auto& a : args) {
				if ("/" + a.s_name == s) {
					a.b_on = true;
					
					if (a.s_name == "i")
						show_info = false;
				}
			}
		}		
		
		vector<unsigned char> buffer;
		vector<unsigned char> image;
		unsigned w, h;
		  
		load_file(buffer, s_file_name); //load the image file with given filename
		
		State state;
		state.decoder.ignore_crc = 1;
		state.decoder.zlibsettings.ignore_adler32 = 1;			
			
		unsigned error = decode(image, w, h, state, buffer);		

		if(error) {
			std::cout << "PNG Decoder error " << error << ": " << lodepng_error_text(error) << endl;
			return 0;
		}
		
		if (show_info) {
			cout << "BCONV by Alexandr Kuznecov" << endl;
			cout << "Version 1.20" << endl << endl;			
			
			cout << "[ ARGUMENTS ]" << endl;
			for (auto& a : args) {
				if (a.b_on) {
					cout << "[/" << a.s_name << "] " << a.s_notice << endl;
				}			
			}		
			
			cout << endl;
			
			cout << "[ PNG INFORMATION ]" << endl;
			cout << "Width: " << w << endl;
			cout << "Height: " << h << endl;
			cout << "Filesize: " << buffer.size() << " (" << buffer.size() / 1024 << "K)" << endl;
			cout << "Total pixels: " << w * h << endl;		
			cout << "Data Size: " << image.size() << endl;
			
			cout << endl;
		}
		
		for (auto& a : args) {
			if (a.b_on) {
				if (a.s_name == "b")
					sn_mode = E_SYSTEM_NUMERATION_BIN;
				if (a.s_name == "d")
					sn_mode = E_SYSTEM_NUMERATION_DEC;			
				if (a.s_name == "p")
					displayImageLightness(image, w, h);	
				if (a.s_name == "v")
					displayArray(image, w, h, E_DIRECTION_ARRAY_VERTICAL);
				if (a.s_name == "h")
					displayArray(image, w, h, E_DIRECTION_ARRAY_HORISONTAL);
			}
		}				
		
	}	
}