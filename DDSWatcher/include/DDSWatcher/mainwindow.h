#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QTreeWidgetItem>
#include <QVector>
#include <QDebug>
#include <QString>

#include <Eigen/Dense>

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

		void remove(QTreeWidgetItem &parent, QTreeWidgetItem *child);

		void LPOSVSSDDSReadCallback(const LPOSVSSMessage &msg);
		void SetWaypointDDSReadCallback(const SetWaypointMessage &msg);
		void DepthDDSReadCallback(const DepthMessage &msg);
		void IMUDDSReadCallback(const IMUMessage &msg);
		void DVLDDSReadCallback(const DVLMessage &msg);
		void PDStatusDDSReadCallback(const PDStatusMessage &msg);

	private slots:
		void onLPOSVSSInfoReceived();
		void onSetWaypointInfoReceived();
		void onDepthInfoReceived();
		void onIMUInfoReceived();
		void onDVLInfoReceived();
		void onPDStatusInfoReceived();


		signals:
		void lposvssInfoReceived();
		void setWaypointInfoReceived();
		void depthInfoReceived();
		void imuInfoReceived();
		void dvlInfoReceived();
		void pdstatusInfoReceived();

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

		bool lposvssData;
		bool setwaypointData;
		bool depthData;
		bool imuData;
		bool dvlData;
		bool pdstatusData;
	};
}
#endif // MAINWINDOW_H
