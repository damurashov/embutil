#define OHDEBUG_PORT_ENABLE
#define OHDEBUG_TAGS_ENABLE \
	"Ut::Sn::StaticInstanceStorage", \
	"Ut::Sn::LongRequestQueue", \
	"Ut::Sn::StaticInstanceRegistry", \
	"Trace", \
	"Test"

#include <iostream>
#include <mutex>
#include <thread>
#include "utility/snippet/NothrowFuture.hpp"
#include "utility/snippet/LongRequestQueue.hpp"

using PromiseType = Ut::Sn::Promise<int, std::mutex>;
using FutureType = Ut::Sn::Future<int, std::mutex>;

void producerTask(PromiseType &&aPromise)
{
	std::cout << "producer thread" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	aPromise.fulfill(42);
	// aPromise.fail();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "end of producer thread" << std::endl;
}

void consumerTask(FutureType &&aFuture)
{
	std::cout << "consumer task" << std::endl;
	int val = 0;

	// first attempt
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	auto state = aFuture.retrieve(val);
	assert(state == Ut::Sn::FutureState::Pending);
	std::cout << val << std::endl;
	std::cout << (int)state << std::endl;

	// second attempt
	std::this_thread::sleep_for(std::chrono::milliseconds(1600));
	state = aFuture.retrieve(val);
	std::cout << val << std::endl;
	std::cout << (int)state << std::endl;
	assert(state == Ut::Sn::FutureState::Fulfilled);
	assert(val == 42);

	std::cout << "end of consumer task" << std::endl;
}

OHDEBUG_TEST("Run future")
{
	FutureType future{};
	// auto alreadyExistingFuture = promise.makeFuture();
	std::thread producerThread{producerTask, future.makePromise()};
	consumerTask(std::move(future));
	producerThread.join();
}

struct Request {
	std::size_t identifier;
};

struct RequestHandler : public Ut::Sn::RequestHandler<Request> {
	void retryRequest(const Request &aRequest)
	{
		std::cout << "retryRequest" << aRequest.identifier << std::endl;
	}

	void onRequestExpired(const Request &aRequest)
	{
		std::cout << "onRequestExpired" << aRequest.identifier << std::endl;
	}
};

using MutexType = std::mutex;
using TimeType = unsigned long;
using LongRequestQueueType = Ut::Sn::LongRequestQueue<Request, MutexType, TimeType>;


OHDEBUG_TEST("Long request queue")
{
	LongRequestQueueType longRequestQueue{};
	RequestHandler requestHandler;
	longRequestQueue.setRequestHandler(requestHandler);
	longRequestQueue.push({42}, 1000, 3, 0U);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(900);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(1200);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(1400);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(2300);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(3400);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(4100);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(5100);
	std::cout << "---" << std::endl;
	longRequestQueue.onTick(6100);
	std::cout << "Done" << std::endl;
}

int main(void)
{
	OHDEBUG_RUN_TESTS();

	return 0;
}
