#include "HAL/transport/FileEndpoint.h"
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

FileEndpoint::FileEndpoint(const std::string &filename) : mFileName(filename) { }

void FileEndpoint::open()
{
	assert(getState() == CLOSED);

	fileDesc = ::open(mFileName.c_str(), O_RDWR);

	if(fileDesc >= 0)
	{
		readthread = thread(&FileEndpoint::readthread_run, this);
		//writethread = thread(&FileEndpoint::writethread_run, this);
		setState(OPEN);
	}
	else
	{
		setState(ERROR, "Unable to open file " + mFileName);
	}
}

void FileEndpoint::close()
{
	if(fileDesc >= 0)
	{
		::close(fileDesc);
		fileDesc = -1;
	}

	cout << "Join" << endl;
	readthread.join();
	writethread.join();
	setState(CLOSED);
}

void FileEndpoint::write(ByteVec::const_iterator begin, ByteVec::const_iterator end)
{
	lock_guard<mutex> lock(writemutex);
	writebuf.insert(writebuf.end(), begin, end);
	writecond.notify_one();
}

void FileEndpoint::readthread_run()
{
	ByteVec recvbuf(4096);

	while(true)
	{
		int got = ::read(fileDesc, (char *)&recvbuf[0], recvbuf.size());

		if(got == -1)
		{
			break;
		}

		if(got > 0)
			callReadCallback(recvbuf.begin(), recvbuf.begin() + got);
	}

	if (fileDesc >= 0)
		setState(ERROR, "Read thread terminated: ");
}

void FileEndpoint::writethread_run()
{
/*	ByteVec outbuf;

	while (true)
	{
		unique_lock<mutex> lock(writemutex);
		while (writebuf.size() == 0)
			writecond.wait(lock);

		swap(writebuf, outbuf); // zero-copy is really easy to achieve with swap() on vectors
		lock.unlock();

		size_t wrote=0;
		while (wrote < outbuf.size())
			::write(fileDesc, (const char*)&outbuf[wrote], outbuf.size() - wrote);
		outbuf.clear();
	}

	if (fileDesc >= 0)
		setState(ERROR, "Write thread terminated: ");*/
}

