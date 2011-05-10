#include <boost/asio.hpp>
#include "SubMain/Workers/SubPDWorker.h"

int main()
{
	boost::asio::io_service io;

	subjugator::PDWorker w(io, 40);

	io.run();

	return 0;
}
