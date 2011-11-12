#ifndef LIBSUB_DDS_CONVERSIONS_H
#define LIBSUB_DDS_CONVERSIONS_H

#include "LibSub/Messages/WorkerLogMessage.h"
#include "LibSub/Worker/WorkerLogger.h"
#include "LibSub/Messages/WorkerStateMessage.h"
#include "LibSub/State/State.h"
#include <ndds/ndds_cpp.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>

namespace subjugator {
	template <class MessageT, typename DataT>
	void to_dds(MessageT &msg, const DataT &data) { msg = static_cast<MessageT>(data); }

	template <class MessageT, typename DataT>
	MessageT to_dds(const DataT &data) { MessageT msg; from_dds(msg, data); return msg; }

	template <typename DataT, class MessageT>
	void from_dds(DataT &data, const MessageT &msg) { data = static_cast<DataT>(msg); }

	template <typename DataT, class MessageT>
	DataT from_dds(const MessageT &msg) { DataT data; from_dds(data, msg); return data; }

	// string

	template <>
	void to_dds(char *&msg, const std::string &data) { msg = const_cast<char *>(data.c_str()); }

	template <>
	void from_dds(std::string &data, char *const &msg) { data = std::string(msg); }

	// ptime

	template <>
	void to_dds(DDS_UnsignedLong &msg, const boost::posix_time::ptime &data) {
		tm t = boost::posix_time::to_tm(data);
		msg = static_cast<DDS_UnsignedLong>(mktime(&t));
	}

	template <>
	void from_dds(boost::posix_time::ptime &data, const DDS_UnsignedLong &msg) {
		data = boost::posix_time::from_time_t(static_cast<time_t>(msg));
	}

	// WorkerLogMessage
	template <>
	void to_dds(WorkerLogMessage &msg, const WorkerLogEntry &entry) {
		to_dds(msg.worker, entry.worker);
		to_dds(msg.type, entry.type);
		to_dds(msg.msg, entry.msg);
		to_dds(msg.time, entry.time);
	}

	template <>
	void from_dds(WorkerLogEntry &entry, const WorkerLogMessage &msg) {
		from_dds(entry.worker, msg.worker);
		from_dds(entry.type, msg.type);
		from_dds(entry.msg, msg.msg);
		from_dds(entry.time, msg.time);
	}

	// StateChangedSignal

	template <>
	void to_dds(StateMessage &msg, const State &state) {
		to_dds(msg.code, state.code);
		to_dds(msg.msg, state.msg);
	}

	template <>
	void from_dds(State &state, const StateMessage &msg) {
		from_dds(state.code, msg.code);
		from_dds(state.msg, msg.msg);
	}
}

#endif


