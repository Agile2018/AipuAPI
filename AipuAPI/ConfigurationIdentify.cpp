#include "ConfigurationIdentify.h"

ConfigurationIdentify::ConfigurationIdentify()
{
	ObserverError();
}

ConfigurationIdentify::~ConfigurationIdentify()
{
}

void ConfigurationIdentify::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}

void ConfigurationIdentify::SetValueJSONToConfiguration() {
	const Json::Value params = jsonBody[PARAMS];
	for (Json::Value::const_iterator it = params.begin();
		it != params.end(); ++it) {

		if (it.key().asString() == AMINEYE) {
			minEyeDistance = it->asInt();
		}
		else if (it.key().asString() == AMAXEYE) {
			maxEyeDistance = it->asInt();
		}
		else if (it.key().asString() == IDENTIFICATIONSPEED) {
			identificationSpeed = it->asInt();
		}
		else if (it.key().asString() == FACEDETECTIONFORCED) {
			faceDetectionForced = it->asInt();
		}
	}

}

void ConfigurationIdentify::ParseJSONToObject() {
	jsonBody.clear();
	stringJSON = manageFile->ReadFileText();
	bool parsingSuccessful = reader.parse(stringJSON, jsonBody);
	if (parsingSuccessful)
	{
		SetValueJSONToConfiguration();
	}
	else {
		error->CheckError(ERROR_PARSING, error->medium);
	}

}

void ConfigurationIdentify::ParseMapToJSON() {
	jsonBody.clear();
	jsonParams.clear();
	std::map<std::string, std::int16_t> params;

	params.insert(std::pair<std::string, std::int16_t>(AMINEYE, minEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(AMAXEYE, maxEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(IDENTIFICATIONSPEED, identificationSpeed));
	params.insert(std::pair<std::string, std::int16_t>(FACEDETECTIONFORCED, faceDetectionForced));
	std::map<std::string, std::int16_t>::const_iterator it = params.begin(),
		end = params.end();

	for (; it != end; ++it) {
		jsonParams[it->first] = it->second;

	}

	jsonBody[CONFIGURATION] = IDENTIFY_CONFIGURATION;
	jsonBody[PARAMS] = jsonParams;

	SaveConfiguration();

}

string ConfigurationIdentify::ParseMapToJSONForIdentify() {
	jsonBody.clear();
	std::map<std::string, std::int16_t> params;
	params.insert(std::pair<std::string, std::int16_t>(IDENTIFICATIONSPEED, identificationSpeed));
	params.insert(std::pair<std::string, std::int16_t>(AMINEYE, minEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(AMAXEYE, maxEyeDistance));
	params.insert(std::pair<std::string, std::int16_t>(FACEDETECTIONFORCED, faceDetectionForced));

	std::map<std::string, std::int16_t>::const_iterator it = params.begin(),
		end = params.end();

	for (; it != end; ++it) {
		jsonBody[it->first] = it->second;

	}

	WriteJSONToString();

	return stringJSON;
}