#ifndef HAL_STREAMTRANSPORT_H
#define HAL_STREAMTRANSPORT_H

#include "HAL/TransportBase.h"
#include <boost/function.hpp>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>

namespace subjugator {
	template <class StreamType> // designed to be either a tcp::socket or a serial_port
	class StreamTransport : public ASIOTransportBase {
		protected:
			StreamTransport(int size) : streamdatavec(size) {
				for (int endnum=0; endnum<size; endnum++) // for each endpoint
					streamdatavec.push_back(new StreamData(ioservice)); // allocate stream data
			}

			struct StreamData {
				StreamData(boost::asio::io_service &ioservice) : stream(ioservice), recvbuf(4096) {
					sendbuf.reserve(4096);
				}

				StreamType stream;
				ByteVec sendbuf;
				ByteVec recvbuf;
			};

			boost::ptr_vector<StreamData> streamdatavec;

		public:
			virtual void write(int endnum, const ByteVec &bytes) {
				// io thread owns all of the endpointdatavec, we can't touch it here
				// so we run a callback in the io thread to do our work
				runCallbackOnIOThread(boost::bind(&StreamTransport<StreamType>::asioAppendSendBufCallback, this, endnum, bytes));
			}

			virtual int getEndpointCount() const {
				return streamdatavec.size();
			}

		private:
			void asioAppendSendBufCallback(int endnum, const ByteVec &bytes) {
  				StreamData &sdata = streamdatavec[endnum];
    			bool sendpending = !sdata.sendbuf.empty(); // if the send buffer isn't empty, a send must be pending
    			sdata.sendbuf.insert(sdata.sendbuf.end(), bytes.begin(), bytes.end()); // append new data to the end of the send buffer

    			if (!sendpending) // if there was no send pending
    				startAsyncSend(endnum); // start one now
			}

			void asioSendCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
				StreamData &sdata = streamdatavec[endnum];

				if (error) {
					if (errorcallback)
						errorcallback(endnum, "SerialTransport received error while sending: " + boost::lexical_cast<std::string>(error)); // call the error callback
					sdata.stream.close();
				}

				sdata.sendbuf.erase(sdata.sendbuf.begin(), sdata.sendbuf.begin() + bytes); // erase the bytes from the send buffer
				if (!sdata.sendbuf.empty()) // if there is still data in the send buffer
					startAsyncSend(endnum); // start another send
			}

			void asioReceiveCallback(int endnum, const boost::system::error_code& error, std::size_t bytes) {
				StreamData &sdata = streamdatavec[endnum];

				if (error) {
					if (errorcallback)
						errorcallback(endnum, "TCPTransport received error while receiving: " + boost::lexical_cast<std::string>(error)); // call the error callback
					sdata.stream.close();
				}

				sdata.recvbuf.resize(bytes); // resize the buffer to the number of bytes asio put in there (asio never resizes vectors)
				if (readcallback)
					readcallback(endnum, sdata.recvbuf); // call the read callback
				sdata.recvbuf.resize(sdata.recvbuf.capacity()); // resize the buffer back to its maximum capacity

				// finally, initiate another asynchronous receive
				startAsyncReceive(endnum);
			}

		protected:
			void startAsyncSend(int endnum) {
				StreamData &sdata = streamdatavec[endnum];
				assert(!sdata.sendbuf.empty());

				sdata.stream.async_write_some(boost::asio::buffer(sdata.sendbuf), boost::bind(&StreamTransport<StreamType>::asioSendCallback, this, endnum, _1, _2));
			}

			void startAsyncReceive(int endnum) {
				StreamData &sdata = streamdatavec[endnum];
				sdata.stream.async_read_some(boost::asio::buffer(sdata.recvbuf), boost::bind(&StreamTransport<StreamType>::asioReceiveCallback, this, endnum, _1, _2));
			}
	};
}

#endif
