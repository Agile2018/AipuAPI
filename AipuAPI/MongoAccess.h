#ifndef MongoAccess_h
#define MongoAccess_h

#include <cstdlib>
#include <memory>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include "ErrorAipuLib.h"

class MongoAccess
{
public:

	static MongoAccess& instance() {
		static MongoAccess instance;
		return instance;
	}

	void Configure(std::unique_ptr<mongocxx::instance> inputInstance,
		std::unique_ptr<mongocxx::pool> inputPool) {
		try
		{
			instanceMain = std::move(inputInstance);
			poolMain = std::move(inputPool);

		}
		catch (const mongocxx::exception& e)
		{
			error->CheckError(ERROR_DATABASE_CONNECT,
				error->medium, e.what());

		}
	}
	using connection = mongocxx::pool::entry;

	connection GetConnection() {
		return poolMain->acquire();
	}

	bsoncxx::stdx::optional<connection> TryGetConnection() {
		return poolMain->try_acquire();
	}

	void ObserverError() {
		auto observerError = error->observableError.map([](Either* either) {
			return either;
		});

		auto subscriptionError = observerError.subscribe([this](Either* either) {
			shootError.on_next(either);
		});
	}


	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	const int ERROR_DATABASE_CONNECT = 405;
	MongoAccess() = default;
	std::unique_ptr<mongocxx::instance> instanceMain = nullptr;
	std::unique_ptr<mongocxx::pool> poolMain = nullptr;
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorAipuLib* error = new ErrorAipuLib();
};

#endif // !MongoAccess_h

