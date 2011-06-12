#ifndef HAL_STREAMTRANSPORT_H
#define HAL_STREAMTRANSPORT_H

#include "HAL/transport/BaseEndpoint.h"
#include "HAL/IOThread.h"
#include <boost/function.hpp>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

namespace subjugator {
	template <class StreamType> // designed to be either a tcp::socket or a serial_port
	class BaseStreamEndpoint : public BaseEndpoint {
		public:
			BaseStreamEndpoint(IOThread &iothread) : iothread(iothread), stream(iothread.getIOService()) {
				pendingsendbuf.reserve(4096);
				outgoingsendbuf.reserve(4096);
				recvbuf.resize(4096);
			}

			virtual void write(ByteVec::const_iterator begin, ByteVec::const_iterator end) {
				iothread.run(boost::bind(&BaseStreamEndpoint<StreamType>::appendSendBufCallback, this, ByteVec(begin, end)));
			}

			virtual void close() {
				stream.close();
				clearBufs();
				setState(CLOSED);
			}

		protected:
			StreamType stream;

			void startAsyncSendReceive() { // called by subclass when stream is ready and when we want to start sending and receiving
				startAsyncReceive(); // start the first async receive

				if (!pendingsendbuf.empty()) { // if there is stuff in the pending send buf
					outgoingsendbuf = pendingsendbuf; // copy it to the outgoing send buffer
					pendingsendbuf.clear();
					startAsyncSend(); // start the first async send as well
				}
			}

			void clearBufs() {
				outgoingsendbuf.clear();
				pendingsendbuf.clear();
			}

		private:
			IOThread &iothread;

			ByteVec pendingsendbuf; // data waiting to be sent
			ByteVec outgoingsendbuf; // data currently being sent asynchronously by asio
			ByteVec recvbuf;

			void appendSendBufCallback(const ByteVec &bytes) {
    			if (outgoingsendbuf.empty() && getState() == OPEN) { // if there's currently no async send, we need to start one
    				outgoingsendbuf = bytes; // copy the data to the outgoing send buf
    				startAsyncSend(); // start a send
    			} else { // there is a pending send, we need to hold on to this data so it can go out on the next async send
    				pendingsendbuf.insert(pendingsendbuf.end(), bytes.begin(), bytes.end()); // copy the data to the pending send buffer
				}
			}

			void sendCallback(const boost::system::error_code& error, std::size_t bytes) { // called when an async send completes
				if (error) { // if an error occured
					setState(ERROR, "BaseStreamEndpoint received error while sending: " + error.message()); // set an error state
					stream.close();
					return;
				}

				outgoingsendbuf.erase(outgoingsendbuf.begin(), outgoingsendbuf.begin() + bytes); // erase the sent bytes from the outgoing send buffer
				outgoingsendbuf.insert(outgoingsendbuf.end(), pendingsendbuf.begin(), pendingsendbuf.end()); // copy any data from the pending send buffer to the outgoing send buffer
				pendingsendbuf.clear(); // clear the pending send buffer

				if (!outgoingsendbuf.empty()) // if there's data in the outgoing send buffer
					startAsyncSend(); // start another send
			}

			void receiveCallback(const boost::system::error_code& error, std::size_t bytes) {
				if (error) {
					setState(ERROR, "BaseStreamEndpoint received error while receiving: " + error.message()); // call the error callback
					stream.close();
					return;
				}

				callReadCallback(recvbuf.begin(), recvbuf.begin() + bytes); // call the read callback
				startAsyncReceive(); // start another receive
			}

			void startAsyncSend() {
				assert(!outgoingsendbuf.empty());
				stream.async_write_some(boost::asio::buffer(outgoingsendbuf), boost::bind(&BaseStreamEndpoint<StreamType>::sendCallback, this, _1, _2));
			}

			void startAsyncReceive() {
				stream.async_read_some(boost::asio::buffer(recvbuf), boost::bind(&BaseStreamEndpoint<StreamType>::receiveCallback, this, _1, _2));
			}
	};
}

#endif
