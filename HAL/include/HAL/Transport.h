/**
\class subjugator::Transport
\brief HAL Abstraction for a communication medium (serial, udp, tcp)

The Transport interface represent a communication medium that the HAL
can use, and allows the sending and receiving of ByteVecs.
A single Transport provides multiple independent bidirectional
endpoints, which are numbered from zero. The class uses a read callback
and an error callback to the RawWire to signal the reception of new data
or if an error occurs. A Transport also has start() and stop() methods
to initialize itself and begin the IO thread.

A Transport may either be stream oriented or packet oriented. Stream oriented
transports make no guarantees as to how the data is received. Data may come in
large groups, or as individual characters. Packet oriented transports guarantee
to receive data in individual packets, split up exactly in the way they were sent.
*/


#ifndef HAL_TRANSPORT_H
#define HAL_TRANSPORT_H

#include "HAL/shared.h"
#include <boost/function.hpp>
#include <string>

namespace subjugator {
	class Transport {
		public:
			virtual ~Transport() { }

			/** \brief Type of a function used as a read callback

			The ReadCallback is called when new data is received on an endpoint, and is given
			both the number of the endpoint and the data received. In the case of stream oriented
			Transports, the data given can be of any size and can encompass portions of a write or
			multiple writes joined together. Packet oriented Transports always call the ReadCallback
			with a single packet, and will call it multiple times in the case that multiple packets arrive.
			*/
			typedef boost::function<void (int endnum, const ByteVec &bytes)> ReadCallback;

			/** \brief Type of function required for an error callback.
			\a endnum can be -1 if the error doesn't involve a specific endpoint.
			*/
			typedef boost::function<void (int endnum, const std::string &msg)> ErrorCallback;

			/** \brief Set functions to be used as callbacks.

			\c configureCallbacks is called by the RawWire early on to provide the Transport with RawWire's desired callback functions.
			\c configureCallbacks must be called before start(), or else the user could miss incoming data or errors. */
			virtual void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback) = 0;

			/** \brief Starts or resumes a Transport.

			\c start causes the Transport to initialize the communication mechanism and begin receiving or sending data.
			This also begins the Transport's IO thread, which is used to run callbacks.
			*/
			virtual void start() = 0;

			/** \brief Stops a Transport.

			\c stop closes and releases any communication mechanisms, and stops the IO thread. No callbacks will be invoked
			after a call to stop().
			*/
			virtual void stop() = 0;

			/** \brief Returns the number of endpoints. */
			virtual int getEndpointCount() const = 0;

			/** \brief Writes data to an endpoint.

			write() sends the specified bytes to the specified endpoint, asynchronously. Depending on whether
			the Transport implementation is packet oriented or stream oriented, the data will either be sent
			as a single complete packet, or appended to the end of a stream.
			*/
			virtual void write(int endnum, const ByteVec &bytes) = 0;
	};
}

#endif

