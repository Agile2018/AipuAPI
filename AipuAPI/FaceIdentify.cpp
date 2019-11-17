#include "FaceIdentify.h"

FaceIndentify::FaceIndentify()
{
}

FaceIndentify::~FaceIndentify()
{
	
}

void FaceIndentify::LoadConnection() {
	int errorCode;
	std::string str = "iengine.db";
	const char *cstr = str.c_str();	

	SetParamsIdentify();
	errorCode = connectToDatabase(cstr);
	cout << "connectToDatabase returns " << errorCode << endl;	

}

void FaceIndentify::SetParamsIdentify() {
	int errorCode;
	//std::string paramsIdentify = configuration->ParseMapToJSONForIdentify();	
	std::string paramsIdentify = format->FormatString("{\"A_IdentificationSpeed\": %d, \"A_MinEyeDist\": %d, \"A_MaxEyeDist\": %d, \"A_FaceDetectionForced\": %d}",
		configuration->GetIdentificationSpeed(),
		configuration->GetMinEyeDistance(),
		configuration->GetMaxEyeDistance(), configuration->GetFaceDetectionForced());
	errorCode = setBiometricParameters(paramsIdentify.c_str());
	
	cout << "setBiometricParameters returns " << errorCode << endl;
	//error->CheckError(errorCode, error->medium);
	
}

void FaceIndentify::EnrollUser(Molded* modelImage) {
	flagEnroll = true;
	int errorCode, userID, score;
	//IENGINE_USER user = IEngine_InitUser();
	User* userForDatabase = new User();
	const unsigned char* templateData = reinterpret_cast<const unsigned char*>(modelImage->GetMoldImage());

	errorCode = identify(templateData, modelImage->GetMoldSize(), &userID, &score);
	//error->CheckError(errorCode, error->medium);
	if (userID == 0 && isRegister)
	{

		errorCode = addUserToDatabase(templateData, modelImage->GetMoldSize(), &userID);
		error->CheckError(errorCode, error->medium);
		if (errorCode == IENGINE_E_NOERROR) {
			userForDatabase->SetIsNew(true);
		}
		userForDatabase->SetIsNew(true);
	}


	if (errorCode == IENGINE_E_NOERROR && userID != 0) { 
		if (!userForDatabase->GetIsNew())
		{
			countRepeatUser++;
		}
		userForDatabase->SetUserIdIFace(userID);
		userForDatabase->SetClient(modelImage->GetIdClient());
		userForDatabase->SetCropImageData(modelImage->GetCropImageData());
		userForDatabase->SetMoldCropHeight(modelImage->GetMoldCropHeight());
		userForDatabase->SetMoldCropLength(modelImage->GetMoldCropLength());
		userForDatabase->SetMoldCropWidth(modelImage->GetMoldCropWidth());

		shootUser.on_next(userForDatabase);
	}
	flagEnroll = false;

}

void FaceIndentify::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}
