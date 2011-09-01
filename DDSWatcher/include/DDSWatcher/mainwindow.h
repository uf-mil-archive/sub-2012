#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTreeWidgetItem>
#include <QVector>
#include <QDebug>
#include <QString>
#include <QFileDialog>

#include <fstream>

#include <Eigen/Dense>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "DDSListeners/DDSSender.h"
#include <ndds/ndds_cpp.h>

#include "DDSCommanders/LPOSVSSDDSReceiver.h"
#include "DDSMessages/LPOSVSSMessage.h"
#include "DDSMessages/LPOSVSSMessageSupport.h"

#include "DDSCommanders/SetWaypointDDSReceiver.h"
#include "DDSMessages/SetWaypointMessage.h"
#include "DDSMessages/SetWaypointMessageSupport.h"

#include "DDSCommanders/DepthDDSReceiver.h"
#include "DDSMessages/DepthMessage.h"
#include "DDSMessages/DepthMessageSupport.h"

#include "DDSCommanders/IMUDDSReceiver.h"
#include "DDSMessages/IMUMessage.h"
#include "DDSMessages/IMUMessageSupport.h"

#include "DDSCommanders/DVLDDSReceiver.h"
#include "DDSMessages/DVLMessage.h"
#include "DDSMessages/DVLMessageSupport.h"

#include "DDSCommanders/PDStatusDDSReceiver.h"
#include "DDSMessages/PDStatusMessage.h"
#include "DDSMessages/PDStatusMessageSupport.h"

#include "DDSCommanders/HydrophoneDDSReceiver.h"
#include "DDSMessages/HydrophoneMessage.h"
#include "DDSMessages/HydrophoneMessageSupport.h"

#include "DDSCommanders/FinderMessageListReceiver.h"
#include "DDSMessages/FinderMessageList.h"
#include "DDSMessages/FinderMessageListSupport.h"
#include "DDSMessages/Finder2DMessage.h"
#include "DDSMessages/Finder2DMessageSupport.h"
#include "DDSMessages/Finder3DMessage.h"
#include "DDSMessages/Finder3DMessageSupport.h"

#include "DDSCommanders/TrajectoryDDSReceiver.h"
#include "DDSMessages/TrajectoryMessage.h"
#include "DDSMessages/TrajectoryMessageSupport.h"

#include "DDSCommanders/TrackingControllerLogDDSReceiver.h"
#include "DDSMessages/TrackingControllerLogMessage.h"
#include "DDSMessages/TrackingControllerLogMessageSupport.h"

namespace Ui {
    class MainWindow;
}

namespace subjugator
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit MainWindow(DDSDomainParticipant *participant, QWidget *parent = 0);
		~MainWindow();

		void logData();

		void LPOSVSSDDSReadCallback(const LPOSVSSMessage &msg);
		void SetWaypointDDSReadCallback(const SetWaypointMessage &msg);
		void DepthDDSReadCallback(const DepthMessage &msg);
		void IMUDDSReadCallback(const IMUMessage &msg);
		void DVLDDSReadCallback(const DVLMessage &msg);
		void PDStatusDDSReadCallback(const PDStatusMessage &msg);
		void HydrophoneDDSReadCallback(const HydrophoneMessage &msg);
		void FinderMessageListDDSReadCallback(const FinderMessageList &msg);
		void TrajectoryDDSReadCallback(const TrajectoryMessage &msg);
		void TrackingControllerLogDDSReadCallback(const TrackingControllerLogMessage &msg);

	private slots:
		void onLPOSVSSInfoReceived();
		void onSetWaypointInfoReceived();
		void onDepthInfoReceived();
		void onIMUInfoReceived();
		void onDVLInfoReceived();
		void onPDStatusInfoReceived();
		void onHydrophoneInfoReceived();
		void onFinderMessageListInfoReceived();
		void onTrajectoryInfoReceived();
		void onTrackingControllerLogInfoReceived();

		void on_btnBrowse_clicked();
		void on_btnStartLog_clicked();
		void on_btnStopLog_clicked();

		signals:
		void lposvssInfoReceived();
		void setWaypointInfoReceived();
		void depthInfoReceived();
		void imuInfoReceived();
		void dvlInfoReceived();
		void pdstatusInfoReceived();
		void hydrophoneInfoReceived();
		void findermessagelistInfoReceived();
		void trajectoryInfoReceived();
		void trackingControllerLogInfoReceived();

	private:
		Ui::MainWindow *ui;

		LPOSVSSDDSReceiver lposvssreceiver;
		LPOSVSSMessage lposvssmsg;
		SetWaypointDDSReceiver setwaypointreceiver;
		SetWaypointMessage setwaypointmsg;
		DepthDDSReceiver depthreceiver;
		DepthMessage depthmsg;
		IMUDDSReceiver imureceiver;
		IMUMessage imumsg;
		DVLDDSReceiver dvlreceiver;
		DVLMessage dvlmsg;
		PDStatusDDSReceiver pdstatusreceiver;
		PDStatusMessage pdstatusmsg;
		HydrophoneDDSReceiver hydrophonereceiver;
		HydrophoneMessage hydrophonemsg;
		FinderMessageListReceiver findermessagelistreceiver;
		FinderMessageList findermessagelistmsg;
		TrajectoryDDSReceiver trajectoryreceiver;
		TrajectoryMessage trajectorymsg;
		TrackingControllerLogDDSReceiver trackingcontrollerlogreceiver;
		TrackingControllerLogMessage trackingcontrollerlogmsg;

		bool lposvssData;
		bool setwaypointData;
		bool depthData;
		bool imuData;
		bool dvlData;
		bool pdstatusData;
		bool hydrophoneData;
		bool findermessagelistData;
		bool trajectoryData;
		bool trackingcontrollerlogData;

		std::ofstream logstream;
		bool logging;
	};
}
#endif // MAINWINDOW_H
