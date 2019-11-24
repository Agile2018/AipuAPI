#include "AipuAPI.h"
#include "Innovatrics.h"
#include "FlowVideo.h"

Innovatrics* innovatrics;
FlowVideo* flowVideo;
string userJson;
string messageError;

AipuAPI::AipuAPI()
{
	innovatrics = new Innovatrics();
	flowVideo = new FlowVideo();
	ObserverEvent();
}

AipuAPI::~AipuAPI()
{
	delete innovatrics;
	delete flowVideo;
}

void AipuAPI::Terminate() {
	innovatrics->Terminate();
}

void AipuAPI::InitLibrary()
{
	auto observerErrorInnovatrics = innovatrics->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorInnovatrics = observerErrorInnovatrics.subscribe([this](Either* either) {
		messageError = to_string(either->GetCode()) + ": " + either->GetLabel();

	});

	innovatrics->SetParamsLibrary();
	innovatrics->InitLibrary();
}

void AipuAPI::LoadConfiguration(string nameFile) {
	flowVideo->LoadConfiguration(nameFile);
}

void AipuAPI::SetFlagFlow(bool flag) {
	flowVideo->SetFlagFlow(flag);
}

void AipuAPI::SetFileVideo(string file) {
	flowVideo->SetFileVideo(file);
}

void AipuAPI::SetIsRegister(bool option) {
	flowVideo->SetIsRegister(option);
}

void AipuAPI::ReloadRecognitionFace() {
	innovatrics->SetParamsLibrary();
	innovatrics->InitLibrary();
	flowVideo->ReloadRecognitionFace();
}

void AipuAPI::SetConfigurationDatabase() {
	flowVideo->SetConfigurationDatabase();
}

void AipuAPI::SetWidthFrame(int value) {
	flowVideo->SetWidthFrame(value);
}

void AipuAPI::SetHeightFrame(int value) {
	flowVideo->SetHeightFrame(value);
}

void AipuAPI::CaptureFlow(int optionFlow) {
	flowVideo->CaptureFlow(optionFlow);
}

void AipuAPI::SetIpCamera(string file) {
	flowVideo->SetIpCamera(file);
}

void AipuAPI::SetFaceConfidenceThresh(int value) {
	flowVideo->SetFaceConfidenceThresh(value);
}

void AipuAPI::SetRefreshInterval(int value) {
	flowVideo->SetRefreshInterval(value);
}

void AipuAPI::SetMinEyeDistance(int minDistance) {
	flowVideo->SetMinEyeDistance(minDistance);
}

void AipuAPI::SetMaxEyeDistance(int maxDistance) {
	flowVideo->SetMaxEyeDistance(maxDistance);
}

void AipuAPI::SetSequenceFps(int fps) {
	flowVideo->SetSequenceFps(fps);
}

void AipuAPI::SetClient(int value) {
	flowVideo->SetClient(value);
}

void AipuAPI::ShowWindow(int option) {
	flowVideo->ShowWindow(option);
}

void AipuAPI::ObserverEvent() {
	auto observerError = flowVideo->observableError.map([](string message) {
		return message;
	});


	auto subscriptionError = observerError.subscribe([this](string message) {
		messageError = message;
		cout << messageError << endl;
	});

	auto observerUserDetect = flowVideo->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionUserDetect = observerUserDetect.subscribe([this](string jsonUser) {

		userJson = jsonUser;
		cout << userJson << endl;
	});
}

string AipuAPI::GetUserJSON() {	
	return userJson;
}

string AipuAPI::GetMessageError() {
	return messageError;
}

void AipuAPI::SetNameWindow(string name) {
	flowVideo->SetNameWindow(name);
}

void AipuAPI::RecognitionFaceFiles(string file, int client) {
	flowVideo->RecognitionFaceFiles(file, client);
}

void AipuAPI::SetIsFinishLoadFiles(bool value) {
	flowVideo->SetIsFinishLoadFiles(value);
}

bool AipuAPI::GetIsFinishLoadFiles() {
	return flowVideo->GetIsFinishLoadFiles();
}

void AipuAPI::ResetLowScore() {
	flowVideo->ResetLowScore();
}

int AipuAPI::GetCountLowScore() {
	return flowVideo->GetCountLowScore();
}

void AipuAPI::ResetCountNotDetect() {
	flowVideo->ResetCountNotDetect();
}

int AipuAPI::GetCountNotDetect() {
	return flowVideo->GetCountNotDetect();
}

void AipuAPI::SetDeviceVideo(string device) {
	flowVideo->SetDeviceVideo(device);
}

void AipuAPI::SetDeepTrack(string value) {
	flowVideo->SetDeepTrack(value);
}

void AipuAPI::ResetCountRepeatUser() {
	flowVideo->ResetCountRepeatUser();
}

int AipuAPI::GetCountRepeatUser() {
	return flowVideo->GetCountRepeatUser();
}