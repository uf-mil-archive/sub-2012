#include "HAL/transport/FileEndpoint.h"
#include <iostream>

using namespace subjugator;
using namespace boost;
using namespace std;

FileEndpoint::FileEndpoint(const std::string &filename) : mFileName(filename) { }

void FileEndpoint::open()
{
	assert(getState() == CLOSED);

	cout << "in filendpoint open" << endl;
	cout << "Trying to open: " << mFileName << endl;

	mFileStream.open(mFileName.c_str(), ios::binary | ios::in | ios::out);

	if(!mFileStream.fail())
	{
		readthread = thread(&FileEndpoint::readthread_run, this);
		writethread = thread(&FileEndpoint::writethread_run, this);
		setState(OPEN);
	}
	else
	{
		setState(ERROR, "Unable to open file " + mFileName);
	}
}

void FileEndpoint::close()
{
	if(mFileStream.is_open())
	{
		mFileStream.close();
	}
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

	while (true)
	{
		mFileStream.read((char *)&recvbuf[0], recvbuf.size());
		size_t got = mFileStream.gcount();

		cout << "rthread got: " << got << endl;

		if(mFileStream.fail())
		{
			break;
		}

		callReadCallback(recvbuf.begin(), recvbuf.begin() + got);
	}

	if (mFileStream.is_open())
		setState(ERROR, "Read thread terminated: ");
}

void FileEndpoint::writethread_run()
{
	ByteVec outbuf;

	while (true)
	{
		unique_lock<mutex> lock(writemutex);
		while (writebuf.size() == 0)
			writecond.wait(lock);

		swap(writebuf, outbuf); // zero-copy is really easy to achieve with swap() on vectors
		lock.unlock();

		size_t wrote=0;
		while (wrote < outbuf.size())
			mFileStream.write((const char*)&outbuf[wrote], outbuf.size() - wrote);
		outbuf.clear();
	}

	if (mFileStream.is_open())
		setState(ERROR, "Write thread terminated: ");
}

