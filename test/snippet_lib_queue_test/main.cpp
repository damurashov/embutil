#define OHDEBUG_PORT_ENABLE 1
#define OHDEBUG_TAGS_ENABLE "Trace"

#include "utility/container/FixedSizeQueue.hpp"
#include "utility/OhDebug.hpp"
#include <cassert>

constexpr std::size_t queueSize()
{
	return 4;
}

using QueueType = typename Ut::Ct::FixedSizeQueue<int, queueSize()>;

OHDEBUG_TEST("Simple push, pop, count")
{
	QueueType queue{};
	assert(queue.count() == 0);

	for (int iAttempt = 0; iAttempt < 2; ++iAttempt) {
		auto currentValue =
			[iAttempt](int i)
			{
				return i + iAttempt * queueSize() + 1;
			};

		for (int i = 0; i < queueSize(); ++i) {
			assert(queue.tryPush(currentValue(i)));
			assert(queue.count() == i + 1);
			OHDEBUG("Trace", "pushing", currentValue(i), "into queue, current size", queue.count());
		}

		assert(!queue.tryPush(0));

		for (int i = 0; i < queueSize(); ++i) {
			int value = 0;
			assert(queue.tryPop(value));
			OHDEBUG("Trace", "Reading from queue, current value =", value, "expected value =", currentValue(i));
			assert(value == currentValue(i));
			assert(queue.count() == queueSize() - i - 1);
		}
	}
}

OHDEBUG_TEST("Queue iteration")
{
	constexpr auto kSize = 16U;
	Ut::Ct::FixedSizeQueue<int, kSize> queue;

	for (int i = 0; i < kSize; ++i) {
		queue.forcePush(i);
	}

	OHDEBUG("Trace", "queue.count()", queue.count());
	assert(queue.count() == kSize);
	int buffer;
	queue.tryPop(buffer);
	OHDEBUG("Trace", "popped element", buffer);
	queue.tryPush(kSize);

	int currentElement = 1;

	for (auto &item : queue) {
		OHDEBUG("Trace", "current item", item);
		assert(currentElement++ == item);
	}

	currentElement = 1;

	for (const auto &item : queue) {
		OHDEBUG("Trace", "current item", item);
		assert(currentElement++ == item);
	}
}

int main(void)
{
	OHDEBUG_RUN_TESTS();
	return 0;
}
