#include "File.h"

File::File()
{
}

File::~File()
{
}

void File::SetNameDirectory(string name) {
	nameDirectory = name;
}

void File::SetNameFile(string name) {
	nameFile = name;
}

void File::WriteFile(string content) {
	CreateDirectory();
	string path = nameDirectory + "/" + nameFile;
	ofstream file_obj;
	file_obj.open(path, ios::app);
	file_obj.write(content.c_str(), sizeof(char)*content.size());
	file_obj.close();

}

bool File::DeleteFile() {
	string path = nameDirectory + "/" + nameFile;
	if (remove(path.c_str()) != 0) {
		return false;
	}
	else {
		return true;
	}

}

bool File::DeleteFile(string filePath) {

	if (remove(filePath.c_str()) != 0) {
		return false;
	}
	else {
		return true;
	}

}

void File::CreateDirectory() {
	_mkdir((char *)&nameDirectory);
}

string File::GetNameDirectory() {
	return nameDirectory;
}

string File::ReadFileText() {
	string result = "";
	string path = nameDirectory + "/" + nameFile;
	ifstream streamFile;
	streamFile.open(path);
	if (streamFile.good()) {
		stringstream streamContent;
		streamContent << streamFile.rdbuf();
		result = streamContent.str();
		streamFile.close();
	}
	return result;

}