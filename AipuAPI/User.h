#ifndef User_h
#define User_h

#include <iostream>
#include <vector>

using namespace std;
class User
{
public:
	User();
	~User();
	const string fieldUserIdFace = "id_face";
	const string fieldNameUser = "name";
	const string fieldAddressUser = "address";
	const string fieldId = "_id";
	const string fieldClient = "client";

	void SetIdDb(string id) {
		idDb = id;
	}
	void SetUserIdIFace(int id) {
		userIdIFace = id;
	}

	void SetNameUser(string name) {
		nameUser = name;
	}

	void SetLastNameUser(string lastName) {
		lastNameUser = lastName;
	}
	string GetIdDb() {
		return idDb;
	}
	int GetUserIdIFace() {
		return userIdIFace;
	}

	string GetNameUser() {
		return nameUser;
	}

	string GetLastNameUser() {
		return lastNameUser;
	}

	void SetPathImageTemp(string path) {
		pathImageTemp = path;
	}

	string GetPathImageTemp() {
		return pathImageTemp;
	}

	void SetIsNew(bool response) {
		isNew = response;
	}

	bool GetIsNew() {
		return isNew;
	}

	void SetClient(int id) {
		client = id;
	}
	int GetClient() {
		return client;
	}

	void SetIdentificationUser(string identification) {
		identificationUser = identification;
	}

	string GetIdentificationUser() {
		return identificationUser;
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

	void SetCropImageData(vector<unsigned char> moldCrop) {
		copy(moldCrop.begin(), moldCrop.end(), back_inserter(cropImageData));

	}

	vector<unsigned char> GetCropImageData() {
		return cropImageData;
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

private:
	string idDb;
	int userIdIFace, cropWidth, cropHeight, cropLength;;
	string nameUser;
	string lastNameUser;
	string identificationUser;
	string pathImageTemp;
	bool isNew = false;
	int client;

	vector<unsigned char> cropImageData;

};


#endif // !User_h

