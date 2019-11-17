#ifndef Configuration_h
#define Configuration_h

#include <json/json.h>
#include "File.h"


class Configuration
{
public:
	void SetStringJSON(string json) {
		stringJSON = json;
	}

	string GetStringJSON() {
		return stringJSON;
	}

	void SetNameFileConfiguration(string name) {
		manageFile->SetNameFile(name);
	}
	void SetNameDirectory(string name) {
		manageFile->SetNameDirectory(name);
	}

	string GetNameDirectory() {
		return manageFile->GetNameDirectory();
	}
	virtual void ParseJSONToObject() {};
	virtual void ParseMapToJSON() {};

protected:
	string stringJSON;
	Json::Value jsonParams;
	Json::Value jsonBody;
	Json::Reader reader;
	File* manageFile = new File();

	void WriteJSONToString() {
		Json::StreamWriterBuilder builder;
		builder.settings_["indentation"] = "    ";
		stringJSON = Json::writeString(builder, jsonBody);

	}


	void SaveConfiguration() {
		if (manageFile->DeleteFile()) {};
		WriteJSONToString();
		manageFile->WriteFile(stringJSON);
	}

};


#endif // !Configuration_h

