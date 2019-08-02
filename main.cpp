#include "LogPrinter.h"
#include <chrono>

int main()
{
	CLogPrinter log;
	log.SetIO(stdout);
	std::chrono::time_point<std::chrono::steady_clock> beginTime, endTime;
	char strBuf[30];
	beginTime = std::chrono::steady_clock::now();
	for (size_t i = 0; i < 1000; i++)
	{
		sprintf_s(strBuf, "print: %d\n", i);
		log << strBuf;
	}
	endTime = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
	log.~CLogPrinter();
	endTime = std::chrono::steady_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
	printf("%d ms\n%d ms\n", duration.count(), duration2.count());
	return 0;
}