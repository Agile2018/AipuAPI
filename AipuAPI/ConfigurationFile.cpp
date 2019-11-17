#include "ConfigurationFile.h"

ConfigurationFile::ConfigurationFile()
{
	ObserverError();
}

ConfigurationFile::~ConfigurationFile()
{
}

void ConfigurationFile::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});
}

void ConfigurationFile::ParseMapToJSON() {
	jsonBody.clear();
	jsonParams.clear();
	std::map<std::string, std::string> params;
	params.insert(std::pair<std::string, std::string>(FILEVIDEO, nameFileConfigurationVideo));
	params.insert(std::pair<std::string, std::string>(FILEFACE, nameFileConfigurationFaceModel));
	params.insert(std::pair<std::string, std::string>(DIRECTORYWORK, nameDirectoryTempWork));
	params.insert(std::pair<std::string, std::string>(DIRECTORYTRAIN, nameDirectoryTraining));
	params.insert(std::pair<std::string, std::string>(FILEDATABASE, nameFileConfigurationDatabase));
	params.insert(std::pair<std::string, std::string>(FILEIDENTIFY, nameFileConfigurationIdentify));
	std::map<std::string, std::string>::const_iterator it = params.begin(),
		end = params.end();
	for (; it != end; ++it) {
		jsonParams[it->first] = it->second;

	}

	jsonBody[CONFIGURATION] = FILE_CONFIGURATION;
	jsonBody[PARAMS] = jsonParams;

	SaveConfiguration();

}

void ConfigurationFile::SetValueJSONToConfiguration() {
	const Json::Value params = jsonBody[PARAMS];
	for (Json::Value::const_iterator it = params.begin();
		it != params.end(); ++it) {

		if (it.key().asString() == FILEVIDEO) {
			nameFileConfigurationVideo = it->asString();
		}
		else if (it.key().asString() == FILEFACE) {
			if (!it->asString().empty()) {
				nameFileConfigurationFaceModel = it->asString();
			}
		}
		else if (it.key().asString() == FILEIDENTIFY) {
			if (!it->asString().empty()) {
				nameFileConfigurationIdentify = it->asString();
			}
		}
		else if (it.key().asString() == DIRECTORYWORK) {
			if (!it->asString().empty()) {
				nameDirectoryTempWork = it->asString();
			}
		}		
		else if (it.key().asString() == FILEDATABASE) {
			if (!it->asString().empty()) {
				nameFileConfigurationDatabase = it->asString();
			}
		}
		else if (it.key().asString() == DIRECTORYTRAIN) {
			nameDirectoryTraining = it->asString();
		}
	}

}

void ConfigurationFile::ParseJSONToObject() {
	jsonBody.clear();
	stringJSON = manageFile->ReadFileText();
	bool parsingSuccessful = reader.parse(stringJSON, jsonBody);
	if (parsingSuccessful)
	{
		SetValueJSONToConfiguration();
	}
	else {
		error->CheckError(ERROR_PARSING, error->medium, LABEL_ERROR_PARSING);
	}
}

