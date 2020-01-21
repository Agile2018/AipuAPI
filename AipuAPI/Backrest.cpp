#include "Backrest.h"
#include "FaceModel.h"
#include "FaceIdentify.h"
#include "ConfigurationFile.h"
#include "Database.h"

FaceModel* faceModel = new FaceModel();
FaceIndentify* faceIdentify = new FaceIndentify();
ConfigurationFile* configurationFile = new ConfigurationFile();
Database* database = new Database();

Backrest::Backrest()
{
	SetDirectoryConfiguration();
	ObserverTemplateImage();
	ObserverIdentifyFace();
	ObserverDatabase();
	ObserverError();
}

Backrest::~Backrest()
{
	delete faceModel;
	delete faceIdentify;
	delete database;
	delete configurationFile;
}

void Backrest::ProcessImageInBack(char* data, int size, int client) {
	if (isFree)
	{
		isFree = false;
		unsigned char* ucharData = reinterpret_cast<unsigned char*> (data);
		std::vector<uchar> buffer(ucharData, ucharData + size);		
		int count = faceModel->ModelOneToOne(buffer, client);
		buffer.clear();
		isFree = true;
	}
	
}

void Backrest::RecognitionFaceFiles(string file, int client) {
	if (faceModel->GetIsFinishLoadFiles() == true)
	{
		faceModel->SetIsFinishLoadFiles(false);
		faceModel->RecognitionFaceFiles(file, client);
		faceModel->SetIsFinishLoadFiles(true);
	}
	
}

void Backrest::ObserverError() {
	auto faceModelError = faceModel->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionFaceModelError = faceModelError.subscribe([this](Either* either) {
		if (either->GetLabel() != "OK")
		{
			shootError.on_next(either);
		}

	});

	auto configurationFileError = configurationFile->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionConfigurationFileError = configurationFileError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});

	auto identifyError = faceIdentify->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionIdentifyError = identifyError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});

	auto databaseError = database->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionDatabaseError = databaseError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}

void Backrest::ObserverTemplateImage()
{
	auto templateObservable = faceModel->observableTemplate.map([](Molded* modelImage) {
		return modelImage;
	});

	auto subscriptionTemplate = templateObservable.subscribe([this](Molded* modelImage) {
		
		faceIdentify->EnrollUser(modelImage);
		
		delete modelImage;
		
	});
	
	subscriptionTemplate.clear();
}

void Backrest::ObserverIdentifyFace() {
	auto identifyUser = faceIdentify->observableUser.map([](User* user) {
		return user;
	});

	auto subscriptionIdentifyUser = identifyUser.subscribe([this](User* user) {

		if (user->GetIsNew())
		{
			string number = to_string(user->GetUserIdIFace());
			string name = "Person " + number;
			string lastName = "LasName " + number;
			string identification = "0000000";
			user->SetNameUser(name);
			user->SetLastNameUser(lastName);
			user->SetIdentificationUser(identification);			
			database->InsertNewUser(user);
			delete user;
		}
		else {
			database->FindUserByIdFace(user->GetUserIdIFace(),
				user->GetCropImageData(), user->GetMoldCropHeight(),
				user->GetMoldCropWidth(), user->GetClient());
			delete user;
		}

	});
	subscriptionIdentifyUser.clear();
}

void Backrest::ObserverDatabase() {
	auto databaseObservable = database->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionDatabase = databaseObservable.subscribe([this](string jsonUser) {
		
		shootUserJSON.on_next(jsonUser);
	});
	subscriptionDatabase.clear();
}

void Backrest::SetDirectoryConfiguration() {
	faceModel->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
	faceIdentify->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
	configurationFile->SetNameDirectory(DIRECTORY_CONFIGURATION);
	database->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
}

void Backrest::LoadConfiguration(string nameFile) {
	configurationFile->SetNameFileConfiguration(nameFile);
	configurationFile->ParseJSONToObject();
	SetNameFileConfigurationFace(configurationFile->GetNameFileConfigurationFaceModel());
	SetNameFileConfigurationIdentify(configurationFile->GetNameFileConfigurationIdentify());
	faceModel->SetNameDirectory(configurationFile->GetNameDirectoryTempWork());
	SetNameFileConfigurationDatabase(configurationFile->GetNameFileConfigurationDatabase());
	database->Configure();
}

void Backrest::SetNameFileConfigurationFace(string name) {
	faceModel->configuration->SetNameFileConfiguration(name);
	faceModel->configuration->ParseJSONToObject();
	faceModel->InitHandle();
}

void Backrest::SetNameFileConfigurationIdentify(string name) {
	faceIdentify->configuration->SetNameFileConfiguration(name);
	faceIdentify->configuration->ParseJSONToObject();
	faceIdentify->LoadConnection();
}

void Backrest::LoadConnectionIdentify() {
	faceIdentify = new FaceIndentify();
	ObserverIdentifyFace();
	SetNameFileConfigurationIdentify(configurationFile->GetNameFileConfigurationIdentify());
}

void Backrest::SetNameFileConfigurationDatabase(string name) {
	database->configuration->SetNameFileConfiguration(name);
	database->configuration->ParseJSONToObject();
}

void Backrest::SetIsRegister(bool option) {
	faceIdentify->SetIsRegister(option);
}

void Backrest::ReloadRecognitionFace() {
	faceModel->configuration->ParseJSONToObject();
	faceModel->TerminateHandle();
	faceModel->InitHandle();
	faceIdentify->configuration->ParseJSONToObject();
	faceIdentify->LoadConnection();
}

void Backrest::SetConfigurationDatabase() {
	database->configuration->ParseJSONToObject();
}

void Backrest::SetIsFinishLoadFiles(bool value) {
	faceModel->SetIsFinishLoadFiles(value);
}

bool Backrest::GetIsFinishLoadFiles() {
	return faceModel->GetIsFinishLoadFiles();
}

void Backrest::ResetLowScore() {
	faceModel->ResetLowScore();
}

int Backrest::GetCountLowScore() {
	return faceModel->GetCountLowScore();
}

void Backrest::ResetCountNotDetect() {
	faceModel->ResetCountNotDetect();
}

int Backrest::GetCountNotDetect() {
	return faceModel->GetCountNotDetect();
}

void Backrest::ResetCountRepeatUser() {
	faceIdentify->ResetCountRepeatUser();
}

int Backrest::GetCountRepeatUser() {
	return faceIdentify->GetCountRepeatUser();
}