/**
\class subjugator::HAL
\headerfile HAL/HAL.h

\brief Top level HAL class, sending and receiving DataObjects to the hardware

HAL, or Hardware Abstraction Layer, is a class used by sensor workers to communicate
with the underlying hardware. To use the HAL, you must first create DataObjects and a
DataObjectConverter with together allow the HAL to convert DataObjects to and from
raw bytes. You must also provide a PacketFormatter to add correct header and checksum
information, and a Transport, to actually send the resulting packets to the device.

A single HAL provides multiple endpoints, which depending on the transport could represent
different IP addresses or different local serial ports. This means that one HAL object
can be used to communicate with multiple physical devices speaking the same protocol.

HAL communicates received data and errors to the Worker by use of read and error callbacks.
These callbacks are always invoked on a thread contained within the Transport known as the IO thread.
The HAL also contains start and stop methods to control the IO thread and Transport. It is important
to configure the HAL callbacks using configureCallbacks before calling start(), otherwise the Worker
could miss data or errors.
*/


#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "HAL/shared.h"
#include "HAL/Transport.h"
#include "HAL/RawWire.h"
#include "HAL/DataObjectFormatter.h"
#include <boost/scoped_ptr.hpp>
#include <memory>

namespace subjugator {
	class HAL {
		public:
			/** \brief Constructs the HAL

			Constructs a HAL object, given a DataObjectFormatter, Transport, and a factory for creating PacketFormatters.
			The HAL takes ownership of the DataObjectFormatter and Transport.
			*/
			HAL(DataObjectFormatter *dataobjectformat, Transport *transport, RawWire::PacketFormatterFactory packetformatfactory);

			/** \brief Type of a function used as a read callback

			The ReadCallback is called when a new DataObject comes across the wire. The callback can optionally take ownership
			of the DataObject by making a copy of the auto_ptr, otherwise when the HAL destructs its auto_ptr,
			the DataObject will also be freed.
			*/
			typedef boost::function<void (int endnum, std::auto_ptr<DataObject> &dobj)> ReadCallback;


			/** \brief Type of function required for an error callback.

			\a endnum can be -1 if the error doesn't involve a specific endpoint.
			*/
			typedef Transport::ErrorCallback ErrorCallback;


			/** \brief Set functions to be used as callbacks.

			\c configureCallbacks configures the function objects to be used as read and error callbacks, which will be invoked
			on the IO thread when new DataObjects are found or errors occur.
			\c configureCallbacks must be called before start(), or else the user could miss incoming data or errors. */
			void configureCallbacks(ReadCallback readcallback, ErrorCallback errorcallback);

			/** \brief Starts or resumes the HAL.

			\c start causes the HAL to initialize the communication mechanism and begin receiving or sending data.
			This also begins the IO thread, which is used to run callbacks.
			*/
			void start();

			/** \brief Stops the HAL.

			\c stop closes and releases any communication mechanisms, and stops the IO thread. No callbacks will be invoked
			after a call to stop().
			*/
			void stop();


			/** \brief Writes a DataObject.

			\c write causes a DataObject to be aynchronously sent to the specified endpoint. HAL makes a copy of the data needed
			internally, so the DataObject can be freed immediately after write returns.
			*/
			void write(int endnum, const DataObject &dataobject);

		private:
			boost::scoped_ptr<DataObjectFormatter> dataobjectformat;
			RawWire rawwire;

			ReadCallback readcallback;
			ErrorCallback errorcallback;

			void rawwireReadCallback(int endnum, const Packet &bytes);
			void rawwireErrorCallback(int endnum, const std::string &mgs);
	};
}

#endif

