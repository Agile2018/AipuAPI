#ifndef Molded_h
#define Molded_h

#include <iostream>
#include <vector>

using namespace std;

class Molded
{
public:
	Molded();
	~Molded();
	void SetMoldImage(char* mold) {
		moldImage = mold;
	}

	void SetCropImageData(unsigned char* moldCrop) {
		cropImageData.assign(moldCrop, moldCrop + cropLength);
	}

	vector<unsigned char> GetCropImageData() {
		return cropImageData;
	}

	void SetMoldSize(int size) {
		moldSize = size;
	}

	void SetMoldCropWidth(int size) {
		cropWidth = size;
	}

	void SetMoldCropHeight(int size) {
		cropHeight = size;
	}

	void SetMoldCropLength(int size) {
		cropLength = size;
	}

	char* GetMoldImage() {
		return moldImage;
	}

	int GetMoldSize() {
		return moldSize;
	}

	int GetMoldCropWidth() {
		return cropWidth;
	}

	int GetMoldCropHeight() {
		return cropHeight;
	}

	int GetMoldCropLength() {
		return cropLength;
	}

	void SetPathImage(string path) {
		pathImage = path;
	}

	string GetPathImage() {
		return pathImage;
	}

	int GetIdClient() {
		return idClient;
	}

	void SetIdClient(int id) {
		idClient = id;
	}


private:
	char* moldImage;
	vector<unsigned char> cropImageData;
	int moldSize, cropWidth, cropHeight, cropLength;
	string pathImage;
	int idClient;

};


#endif // !Molded_h

