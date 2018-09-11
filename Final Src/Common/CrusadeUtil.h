#pragma once

#include "pch.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "atlstr.h"
#include <SpriteFont.h>

using namespace std;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

namespace CrusadeUtil{
	/*Make sure you delete your wchar_t array after use. Its your responsibility*/
	inline void s_to_ws(_In_ std::string orig, _Out_ wchar_t **output){
		*output = new wchar_t[orig.length() + 1];
		size_t *result = new size_t;
		mbstowcs_s(result, *output, orig.length() + 1, orig.c_str(), orig.length() + 1);
		delete result;
		result = nullptr;
	}

	//Print to output
	inline void debug(string s){
		wchar_t *output;
		s_to_ws(s, &output);
		//wchar_t *output= new wchar_t;// 
		wsprintf((LPWSTR)output, L"%S", s.c_str());
		try{
			OutputDebugString((LPCWSTR)output);
		}
		catch (exception e){
			wchar_t *st = new wchar_t;
			wsprintf((LPWSTR)st, L"%S", e.what());
			OutputDebugString(st);
			delete st;
		}
		delete output;
	}

	inline std::string loadFileAsString(LPWSTR path){
		using namespace std;
		ifstream readFile;
		string output = "";
		string temp;
		readFile.open(path);
		if (readFile.is_open()){
			while (!readFile.eof()){
				getline(readFile, temp);
				output += temp + "\n";
			}
		}
		readFile.close();
		return output;
	}

	inline std::string toLowerCase(std::string input){
		std::transform(input.begin(), input.end(), input.begin(), tolower);
		return input;
	}

	inline void getRelativePathTo(_In_ const LPWSTR absolutePath, _Out_ LPWSTR relativePath){
		wchar_t myPath[300];
		int bytes = GetCurrentDirectoryW(300, myPath);
		PathRelativePathToW(relativePath, myPath, FILE_ATTRIBUTE_DIRECTORY, absolutePath, FILE_ATTRIBUTE_NORMAL);
	}

	inline bool isPointIntersecting(const Vector2 point, const Vector2 boxPos, const Vector2 boxSize){
		Vector2 vmax = Vector2::Max(boxPos, boxPos + boxSize);
		Vector2 vmin = Vector2::Min(boxPos, boxPos + boxSize);

		if (point.x < vmin.x) return false;
		if (point.x > vmax.x) return false;
		if (point.y < vmin.y) return false;
		if (point.y > vmax.y) return false;

		return true;
	}

	/*Requires a vector<string>parameter be passed to. This is updated by the function and is also returned
	 If two delimiters are consecutive with no content in between, will have a null string*/
	inline vector<string> &split(const string &s, char delim, vector<string> &elems) {
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	/*Splits the string on delim and returns the result as a new vector<string>
	 If two delimiters are consecutive with no content in between, will have a null string*/
	inline vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}

	/*Converts a 4 channel float rgba color to a 4 channel float hsla color*/
	inline Vector4 rgbToHsl(Vector4 rgba){
		float r = rgba.x;
		float g = rgba.y;
		float b = rgba.z;
		float a = rgba.w;

		float minRGB = min(min(r, g), min(g, b));
		float maxRGB = max(max(r, g), max(g, b));

		float lum = (minRGB + maxRGB) / 2;

		float sat = 0;
		float hue = 0;
		if (minRGB == maxRGB){
			return Vector4(hue, sat, lum, a);
		}

		if (lum < .5f)
		{
			sat = (maxRGB - minRGB) / (maxRGB + minRGB);
		}
		else
		{
			sat = (maxRGB - minRGB) / (2.0f - maxRGB - minRGB);
		}

		if (maxRGB == r)
		{
			hue = (g - b) / (maxRGB - minRGB);
		}
		else if (maxRGB == g)
		{
			hue = 2.0f + (b - r) / (maxRGB - minRGB);
		}
		else // maxRGB == b
		{
			hue = 4.0f + (r - g) / (maxRGB - minRGB);
		}

		hue *= 60;
		hue /= 360;

		return Vector4(hue, sat, lum, a);
	}

	/*Converts a 4 channel float hsla color to a 4 channel float rgba color*/
	inline Vector4 hslToRgb(Vector4 hsla){
		float hue = hsla.x;
		float sat = hsla.y;
		float lum = hsla.z;
		float a = hsla.w;

		if (sat == 0){
			return Vector4(lum, lum, lum, a);
		}

		float temp1;
		float temp2;
		if (lum < .5f)
		{
			temp1 = lum * (1.0f + sat);
		}
		else
		{
			temp1 = lum + sat - (lum * sat);
		}

		temp2 = 2 * lum - temp1;

		float r = hue + .333f;
		if (r < 0)
			r += 1;
		if (r > 1)
			r -= 1;

		float g = hue;

		float b = hue - .333f;
		if (b < 0)
			b += 1;
		if (b > 1)
			b -= 1;

		if (6 * r < 1)
			r = temp2 + (temp1 - temp2) * 6 * r;
		else if (2 * r < 1)
			r = temp1;
		else if (3 * r < 2)
			r = temp2 + (temp1 - temp2) * (.666f - r) * 6;
		else
			r = temp2;

		if (6 * g < 1)
			g = temp2 + (temp1 - temp2) * 6 * g;
		else if (2 * g < 1)
			g = temp1;
		else if (3 * g < 2)
			g = temp2 + (temp1 - temp2) * (.666f - g) * 6;
		else
			g = temp2;

		if (6 * b < 1)
			b = temp2 + (temp1 - temp2) * 6 * b;
		else if (2 * b < 1)
			b = temp1;
		else if (3 * b < 2)
			b = temp2 + (temp1 - temp2) * (.666f - b) * 6;
		else
			b = temp2;

		return Vector4(r, g, b, a);
	}

	/*An improvement on the default SpriteFont measure Text. This one calculates change according to scale too.*/
	inline Vector2 measureTextSize(LPWSTR text, Vector2 scale, SpriteFont* font){
		Vector2 x = font->MeasureString(text);
		return Vector2(x.x*scale.x, x.y*scale.y);
	}

	/*Draw a text within a bounding box making use of word wrap.
	Returns the remainder of text that was unable to fit within the bounding box (box)
	Params:
	text			- the text to display
	box				- Top and left define the top left corner of the bounding box, bottom and right define it's height and width, respectively
	textScale		- The scale at which the text should be rendered
	color			- The color to draw the text
	rotation		- Rotation of the text
	font			- The SpriteFont to draw this textBox
	spriteBatch		- The spriteBatch to render everything
	out_drawnText	- Gets set to the text that this function draws to the screen. Could be useful for optimization

	Lines 163 through 165 of MenuScreen (Commented out) have an example of usage.
	*/
	inline wstring drawTextBox( SpriteFont* font, SpriteBatch* spriteBatch,wstring text, RECT box, Vector2 textScale, DirectX::XMVECTOR color = Colors::White, float rotation = 0.0f, wstring* out_drawnText = nullptr){
		wstring draw = L"";
		wstring temp = L"";
		Vector2 size;

		size = measureTextSize((LPWSTR)text.c_str(), textScale, font);
		if (size.x < box.right){
			font->DrawString(spriteBatch, (LPWSTR)text.c_str(), Vector2((float)box.left, (float)box.top), color, rotation, Vector2(0.0f, 0.0f), textScale);
			return L"";
		}
		//size = measureTextSize((LPWSTR)draw.c_str(), textScale, font);
		int index;
		while (measureTextSize((LPWSTR)(draw + L"\n").c_str(), textScale, font).y < box.bottom && text.length() > 0){
			index = text.find_first_of(L" ");
			temp = text.substr(0, index);
			text = (index > 0) ? text.substr(index + 1) : L"";
			index = text.find_first_of(L" ");
			while (measureTextSize((LPWSTR)((temp + L" " + text.substr(0, index)).c_str()), textScale, font).x < box.right && text.length() > 0){
				temp = temp + L" " + text.substr(0, index);
				text = (index > 0)?text.substr(index+1): L"";
				index = text.find_first_of(L" ");
			}
			draw += (text.length() > 0)?(temp + L"\n") : temp;
			temp = L"";
			//size = measureTextSize((LPWSTR)(draw + L"\n").c_str(), textScale, font); //check if adding another line is posible
		}
		font->DrawString(spriteBatch, (LPWSTR)draw.c_str(), Vector2((float)box.left, (float)box.top), color, rotation, Vector2(0.0f, 0.0f), textScale);
		if (out_drawnText){
			*out_drawnText = draw;
		}
		return text;
	}

	inline int calcNumLines(int height, Vector2 scale, SpriteFont* font){
		wstring lines = L"M\n";
		int x = 0;
		float textY = measureTextSize((LPWSTR)lines.c_str(), scale, font).y;
		while (textY < height){
			x++;
			lines += L"M\n";
			textY = measureTextSize((LPWSTR)lines.c_str(), scale, font).y;
		}
		return x;
	}
}

