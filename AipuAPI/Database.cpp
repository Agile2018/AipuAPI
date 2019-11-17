#include "Database.h"

Database::Database()
{
	ObserverError();
	MongoAccess::instance().ObserverError();
}

Database::~Database()
{
}

void Database::Configure() {
	mongocxx::uri uri(configuration->GetConnectString().c_str()); //"mongodb://localhost:27017"
	class NoopLogger : public mongocxx::logger {
	public:
		virtual void operator()(mongocxx::log_level,
			bsoncxx::stdx::string_view,
			bsoncxx::stdx::string_view) noexcept {}
	};

	auto instance =
		bsoncxx::stdx::make_unique<mongocxx::instance>(bsoncxx::stdx::make_unique<NoopLogger>());

	MongoAccess::instance().Configure(std::move(instance),
		bsoncxx::stdx::make_unique<mongocxx::pool>(std::move(uri)));

}

void Database::InsertNewUser(User* user) {
	std::thread(&Database::AddImageUser, this,
		user->GetCropImageData(), user->GetMoldCropHeight(), user->GetMoldCropWidth(),
		user->GetUserIdIFace()).join();
	std::thread(&Database::AddUser, this, user).join();
	BuildNewUser(user);

}

//void Database::AddRecordsUser(User* user) {
//	AddImageUser(user->GetCropImageData(), user->GetMoldCropHeight(), 
//		user->GetMoldCropWidth(), user->GetUserIdIFace());		
//	AddUser(user);
//	BuildNewUser(user);
//}

void Database::AddUser(User* user) {

	auto clientConnection = MongoAccess::instance().GetConnection();
	mongocxx::database database = (*clientConnection)[configuration->GetNameDatabase().c_str()];

	mongocxx::collection collection = database[COLLECTION_USER.c_str()];

	lastUserId = user->GetUserIdIFace();
	lastClient = user->GetClient();
	bsoncxx::document::value builder = make_document(
		kvp("id_face", user->GetUserIdIFace()),
		kvp("name", user->GetNameUser().c_str()),
		kvp("lastname", user->GetLastNameUser().c_str()),
		kvp("identification", user->GetIdentificationUser().c_str()));
	try
	{
		bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(std::move(builder));

	}
	catch (const mongocxx::exception& e)
	{
		error->CheckError(ERROR_DATABASE,
			error->medium, e.what());
	}


}

void Database::AddImageUser(vector<unsigned char> image, int rows, int cols, int idUser) {
	auto clientConnection = MongoAccess::instance().GetConnection();
	mongocxx::database database = (*clientConnection)[configuration->GetNameDatabase().c_str()];
	mongocxx::collection collection = database[COLLECTION_IMAGE.c_str()];

	string imageBase64 = FileImageToStringBase64(image, rows, cols);
	bsoncxx::document::value builder = make_document(
		kvp("id_face", idUser),
		kvp("data_64", imageBase64.c_str()),
		kvp("data_64_aux", "empty"));
	try
	{

		bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(std::move(builder));

	}
	catch (const mongocxx::exception& e)
	{
		error->CheckError(ERROR_DATABASE,
			error->medium, e.what());
	}

}

void Database::BuildNewUser(User* user) {
	int idFace = user->GetUserIdIFace();
	int client = user->GetClient();
	std::vector<std::string> values;
	values.push_back(to_string(idFace));
	values.push_back(user->GetNameUser());
	values.push_back(user->GetLastNameUser());
	values.push_back(user->GetIdentificationUser());
	values.push_back("1");
	values.push_back(to_string(client));
	BuildJSONUser(values);
}

string Database::FileImageToStringBase64(vector<unsigned char> image, int rows, int cols) {
	string encodedPng;
	vector<uchar> bufferImage;
	//Mat img = imread(path, IMREAD_COLOR);
	Mat img = Mat(rows, cols, CV_8UC3);
	img.data = &image[0];

	int params[3] = { 0 };
	params[0] = IMWRITE_JPEG_QUALITY;
	params[1] = 100;

	if (!img.empty())
	{
		bool code = cv::imencode(".jpg", img,
			bufferImage, std::vector<int>(params, params + 2));
		uchar* buffToBase64 = reinterpret_cast<uchar*> (&bufferImage[0]);

		encodedPng = base64->base64_encode(buffToBase64,
			(unsigned int)bufferImage.size());
		//std::thread(&Database::DeleteFileTempCropImage, this, path).detach();
	}
	return encodedPng;
}

void Database::DeleteFileTempCropImage(string filePath) {
	file->DeleteFile(filePath);
}

void Database::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});

	auto observerErrorDatabase = MongoAccess::instance().observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorDatabase = observerErrorDatabase.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
	auto observerConfiguration = configuration->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionConfiguration = observerConfiguration.subscribe([this](Either* either) {
		shootError.on_next(either);
	});



}

void Database::FindUserByIdFace(int idFaceUser, vector<unsigned char> image,
	int rows, int cols, int client) {
	std::thread(&Database::QueryUserByFace, this, idFaceUser, client).detach();
	std::thread(&Database::UpdateImageUser, this, idFaceUser, image, rows, cols).detach();
}

void Database::QueryUserByFace(int idFaceUser, int client) {
	if (lastUserId != idFaceUser || lastClient != client)
	{
		auto clientConnection = MongoAccess::instance().GetConnection();
		mongocxx::database database = (*clientConnection)[configuration->GetNameDatabase().c_str()];
		mongocxx::collection collection = database[COLLECTION_USER.c_str()];
		lastUserId = idFaceUser;
		lastClient = client;
		boost::optional<bsoncxx::v_noabi::document::value> cursor = collection
			.find_one(make_document(kvp("id_face", idFaceUser)));

		auto view = (*cursor).view();
		int idFace = view["id_face"].get_int32().value;
		std::vector<std::string> values;
		values.push_back(to_string(idFace));
		values.push_back(view["name"].get_utf8().value.to_string());
		values.push_back(view["lastname"].get_utf8().value.to_string());
		values.push_back(view["identification"].get_utf8().value.to_string());
		values.push_back("0");
		values.push_back(to_string(client));
		BuildJSONUser(values);

	}

}

void Database::UpdateImageUser(int idFaceUser, vector<unsigned char> image, int rows, int cols) {
	auto clientConnection = MongoAccess::instance().GetConnection();
	mongocxx::database database = (*clientConnection)[configuration->GetNameDatabase().c_str()];
	mongocxx::collection collection = database[COLLECTION_IMAGE.c_str()];
	string imageBase64 = FileImageToStringBase64(image, rows, cols);
	try
	{

		bsoncxx::stdx::optional<mongocxx::v_noabi::result::update> result = collection
			.update_one(make_document(kvp("id_face", idFaceUser)),
				make_document(kvp("$set", make_document(kvp("data_64_aux", imageBase64)))));
	}
	catch (const mongocxx::exception& e)
	{
		error->CheckError(ERROR_DATABASE,
			error->medium, e.what());
	}
}


void Database::BuildJSONUser(vector<std::string> values) {
	string stringJSON;
	Json::Value jsonParams;
	Json::Value jsonBody;

	std::map<std::string, std::string> params;
	params.insert(std::pair<std::string, std::string>(FIELD_USER_ID_FACE, values[0]));
	params.insert(std::pair<std::string, std::string>(FIELD_USER_NAME, values[1]));
	params.insert(std::pair<std::string, std::string>(FIELD_USER_LAST_NAME, values[2]));
	params.insert(std::pair<std::string, std::string>(FIELD_USER_IDENTIFICATION, values[3]));
	params.insert(std::pair<std::string, std::string>(FIELD_USER_REGISTER, values[4]));
	params.insert(std::pair<std::string, std::string>(FIELD_CLIENT, values[5]));
	std::map<std::string, std::string>::const_iterator it = params.begin(),
		end = params.end();
	for (; it != end; ++it) {
		jsonParams[it->first] = it->second;

	}

	jsonBody[COLLECTION_USER] = DATASOURCE;
	jsonBody[PARAMS] = jsonParams;

	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "    ";
	stringJSON = Json::writeString(builder, jsonBody);
	shootUserJSON.on_next(stringJSON);

}

string Database::QueryImageOfUser(int idFaceUser) {
	auto clientConnection = MongoAccess::instance().GetConnection();
	mongocxx::database database = (*clientConnection)[configuration->GetNameDatabase().c_str()];
	mongocxx::collection collection = database[COLLECTION_IMAGE.c_str()];
	boost::optional<bsoncxx::v_noabi::document::value> cursor = collection
		.find_one(make_document(kvp("id_face", idFaceUser)));
	auto view = (*cursor).view();
	string image = view["data_64"].get_utf8().value.to_string();
	return image;
}
