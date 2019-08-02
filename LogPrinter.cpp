#include "LogPrinter.h"

std::mutex CLogPrinter::stc_mut_protect;
std::list<CLogPrinter::stPrintPair> CLogPrinter::stc_v_file;

CLogPrinter::CLogPrinter()
{
	CLogPrinter::stc_mut_protect;
	CLogPrinter::stc_v_file;
	m_iter._Ptr = nullptr;
}
CLogPrinter::~CLogPrinter()
{
	if (m_iter._Ptr != nullptr)
	{
		stc_mut_protect.lock();
		m_iter->count--;
		if (m_iter->count == 0)
		{
			std::string str;
			m_iter->queue.push(str);
			if (m_iter->thread.joinable())
			{
				m_iter->thread.join();
			}
			stc_v_file.erase(m_iter);
		}
		stc_mut_protect.unlock();
	}
	m_iter._Ptr = nullptr;
}
int CLogPrinter::SetIO(FILE * pFile)
{
	this->~CLogPrinter();
	if (pFile)
	{
		stc_mut_protect.lock();
		for (m_iter = stc_v_file.begin(); m_iter != stc_v_file.end(); m_iter++)
		{
			if (pFile == m_iter->pFile)
			{
				m_iter->count++;
				break;
			}
		}
		if (m_iter == stc_v_file.end() || m_iter._Ptr == nullptr)
		{
			stPrintPair pp;
			stc_v_file.push_back(pp);
			m_iter = stc_v_file.end();
			m_iter--;
			m_iter->pFile = pFile;
			m_iter->thread = std::thread(funcForThread, &(m_iter->queue), pFile);
			m_iter->count = 1;
		}

		stc_mut_protect.unlock();
	}
	else
	{
		return 0;
	}
	return 1;
}
void CLogPrinter::operator<<(const std::string str)
{
	if (m_iter._Ptr != nullptr && str.size() > 0)
	{
		m_iter->queue.push(str);
	}
}

void CLogPrinter::funcForThread(stMsgQueue *queue, FILE *pFile)
{
	std::string str;
	while (true)
	{
		str = queue->wait();
		if (str.size() > 0)
		{
			fprintf(pFile, "%s", str.c_str());
		}
		else
		{
			break;
		}
	}
}

void CLogPrinter::stMsgQueue::push(std::string str)
{
	mut.lock();
	queue.push(str);
	cv.notify_one();
	mut.unlock();
}

std::string CLogPrinter::stMsgQueue::wait()
{
	std::string ret;
	bool bIsNotGet(true);
	std::unique_lock<std::mutex> ulk(mut_wait);
	do
	{
		mut.lock();
		if (queue.size() > 0)
		{
			ret = queue.front();
			queue.pop();
			mut.unlock();
			bIsNotGet = false;
		}
		else
		{
			mut.unlock();
			cv.wait(ulk);
		}
	} while (bIsNotGet);
	return ret;
}
