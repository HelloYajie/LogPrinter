#pragma once
#include <mutex>
#include <thread>
#include <queue>
#include <list>
class CLogPrinter
{
public:
	CLogPrinter();
	~CLogPrinter();

	int SetIO(FILE *pFile);

	//enum euWaitRet
	//{
	//	timeout,
	//	no_timeout
	//};
	//euWaitRet BeginPrint(unsigned long long waitMillSec);
	void operator << (const std::string str);
	//void EndPrint();

private:
	struct stMsgQueue
	{
		void push(std::string str);
		std::string wait();
		std::queue<std::string> queue;
		std::mutex mut, mut_wait;
		std::condition_variable cv;
	};
	struct stPrintPair
	{
		FILE* pFile;
		std::thread thread;//不可复制
		stMsgQueue queue;//不可复制
		unsigned int count;
		stPrintPair()
		{
			pFile = nullptr;
			count = 0;
		}
		stPrintPair(const stPrintPair & src)
		{
			pFile = src.pFile;
			count = src.count;
		}
	};
	static void funcForThread(stMsgQueue *queue, FILE *pFile);
	static std::mutex stc_mut_protect;
	static std::list<stPrintPair> stc_v_file;

private:
	std::list<stPrintPair>::iterator m_iter;
};
