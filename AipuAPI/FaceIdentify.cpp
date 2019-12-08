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

	/*errorCode = IEngine_InitModule();
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_IDENTIFICATION_SPEED, 4);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_MAX_ROTATION, 90);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_Connect(cstr);
	error->CheckError(errorCode, error->medium);*/


	/*errorCode = IEngine_ClearDatabase();
	error->CheckError(errorCode, error->medium);*/
}

void FaceIndentify::SetParamsIdentify() {
	int errorCode;
	//std::string paramsIdentify = configuration->ParseMapToJSONForIdentify();	
	std::string paramsIdentify = format->FormatString("{\"A_IdentificationSpeed\": %d, \"A_SimilarityThreshold\": %d, \"A_BestMatchedCandidates\": %d}",
		configuration->GetIdentificationSpeed(), 
		configuration->GetSimilarityThreshold(),
		configuration->GetBestMatchedCandidates());
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


	//errorCode = IEngine_AddFaceTemplate(user, templateData, modelImage->GetMoldSize());
	//if (errorCode == IENGINE_E_NOERROR) {
	//	User* userForDatabase = new User();

	//	errorCode = IEngine_Connect("iengine.db");
	//	error->CheckError(errorCode, error->medium);

	//	errorCode = IEngine_FindUser(user, &userID, &score);
	//	if (userID == 0 && isRegister)
	//	{

	//		errorCode = IEngine_RegisterUser(user, &userID);
	//		error->CheckError(errorCode, error->medium);
	//		if (errorCode == IENGINE_E_NOERROR) {
	//			userForDatabase->SetIsNew(true);
	//		}
	//	}
	//	if (errorCode == IENGINE_E_NOERROR && userID != 0) {
	//		/*userForDatabase->SetUserIdIFace(userID);
	//		userForDatabase->SetPathImageTemp(modelImage->GetPathImage());
	//		userForDatabase->SetClient(modelImage->GetIdClient());*/
	//		if (!userForDatabase->GetIsNew())
	//		{
	//			countRepeatUser++;
	//		}
	//		userForDatabase->SetUserIdIFace(userID);
	//		userForDatabase->SetClient(modelImage->GetIdClient());
	//		userForDatabase->SetCropImageData(modelImage->GetCropImageData());
	//		userForDatabase->SetMoldCropHeight(modelImage->GetMoldCropHeight());
	//		userForDatabase->SetMoldCropLength(modelImage->GetMoldCropLength());
	//		userForDatabase->SetMoldCropWidth(modelImage->GetMoldCropWidth());
	//		shootUser.on_next(userForDatabase);
	//	}
	//}
	//else {
	//	error->CheckError(errorCode, error->medium);
	//}

	//errorCode = IEngine_FreeUser(user);
	//error->CheckError(errorCode, error->medium);
}

void FaceIndentify::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}
