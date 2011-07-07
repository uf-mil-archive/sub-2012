#include "DDSWatcher/mainwindow.h"
#include "ui_mainwindow.h"

using namespace subjugator;
using namespace std;
using namespace Eigen;

MainWindow::MainWindow(DDSDomainParticipant *participant, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lposvssreceiver(participant, "LPOSVSS", bind(&MainWindow::LPOSVSSDDSReadCallback, this, _1)),
    setwaypointreceiver(participant, "SetWaypoint", bind(&MainWindow::SetWaypointDDSReadCallback, this, _1)),
    depthreceiver(participant, "Depth", bind(&MainWindow::DepthDDSReadCallback, this, _1)),
    imureceiver(participant, "IMU", bind(&MainWindow::IMUDDSReadCallback, this, _1)),
    dvlreceiver(participant, "DVL", bind(&MainWindow::DVLDDSReadCallback, this, _1)),
    pdstatusreceiver(participant, "PDStatus", bind(&MainWindow::PDStatusDDSReadCallback, this, _1)),
    lposvssData(false),
	setwaypointData(false),
	depthData(false),
	imuData(false),
	dvlData(false),
	pdstatusData(false)
{    
    ui->setupUi(this);
    this->setWindowTitle(tr("DDSWatcher"));

    // CONNECT DDS SIGNALS
    connect(this, SIGNAL(lposvssInfoReceived()), this, SLOT(onLPOSVSSInfoReceived()));
    connect(this, SIGNAL(setWaypointInfoReceived()), this, SLOT(onSetWaypointInfoReceived()));
    connect(this, SIGNAL(depthInfoReceived()), this, SLOT(onDepthInfoReceived()));
    connect(this, SIGNAL(imuInfoReceived()), this, SLOT(onIMUInfoReceived()));
    connect(this, SIGNAL(dvlInfoReceived()), this, SLOT(onDVLInfoReceived()));
    connect(this, SIGNAL(pdstatusInfoReceived()), this, SLOT(onPDStatusInfoReceived()));

    ui->treeDDS->setColumnCount(2);

    QStringList headers;
    headers << tr("Topic/Variables") << tr("Value");
    ui->treeDDS->setHeaderLabels(headers);
}

void MainWindow::remove(QTreeWidgetItem &parent, QTreeWidgetItem *child)
{
    parent.removeChild(child);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//***************************************************************************
// On Received Functions
//***************************************************************************
void MainWindow::onLPOSVSSInfoReceived()
{
	// Edit LPOSVSS data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("LPOSVSS"), Qt::MatchWildcard);

	QTreeWidgetItem *lposvss = found.first();

	lposvss->child(0)->setText(0,tr("Timestamp"));
	lposvss->child(0)->setText(1,QString::number(lposvssmsg.timestamp));

	lposvss->child(1)->setText(0,tr("State"));
	lposvss->child(1)->setText(1,QString::number(lposvssmsg.state));

	lposvss->child(2)->setText(0,tr("Position_NED"));
	lposvss->child(2)->setText(1,QString::number(lposvssmsg.position_NED[0]));
	lposvss->child(3)->setText(1,QString::number(lposvssmsg.position_NED[1]));
	lposvss->child(4)->setText(1,QString::number(lposvssmsg.position_NED[2]));

	lposvss->child(2)->setText(0,tr("Quaternion_NED_B"));
	lposvss->child(2)->setText(1,QString::number(lposvssmsg.quaternion_NED_B[0]));
	lposvss->child(3)->setText(1,QString::number(lposvssmsg.quaternion_NED_B[1]));
	lposvss->child(4)->setText(1,QString::number(lposvssmsg.quaternion_NED_B[2]));
	lposvss->child(5)->setText(1,QString::number(lposvssmsg.quaternion_NED_B[3]));

	lposvss->child(6)->setText(0,tr("Velocity_NED"));
	lposvss->child(6)->setText(1,QString::number(lposvssmsg.velocity_NED[0]));
	lposvss->child(7)->setText(1,QString::number(lposvssmsg.velocity_NED[1]));
	lposvss->child(8)->setText(1,QString::number(lposvssmsg.velocity_NED[2]));

	lposvss->child(9)->setText(0,tr("AngularRate_Body"));
	lposvss->child(9)->setText(1,QString::number(lposvssmsg.angularRate_BODY[0]));
	lposvss->child(10)->setText(1,QString::number(lposvssmsg.angularRate_BODY[1]));
	lposvss->child(10)->setText(1,QString::number(lposvssmsg.angularRate_BODY[2]));

	lposvss->child(11)->setText(0,tr("Acceleration_BODY"));
	lposvss->child(11)->setText(1,QString::number(lposvssmsg.acceleration_BODY[0]));
	lposvss->child(12)->setText(1,QString::number(lposvssmsg.acceleration_BODY[1]));
	lposvss->child(13)->setText(1,QString::number(lposvssmsg.acceleration_BODY[2]));
}

void MainWindow::onSetWaypointInfoReceived()
{
	// Edit SetWaypoint data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("SetWaypoint"), Qt::MatchWildcard);

	QTreeWidgetItem *setwaypoint = found.first();

	setwaypoint->child(0)->setText(0,tr("isRelative"));
	setwaypoint->child(0)->setText(1,QString::number(setwaypointmsg.isRelative));

	setwaypoint->child(1)->setText(0,tr("Position_NED"));
	setwaypoint->child(1)->setText(1,QString::number(setwaypointmsg.position_ned[0]));
	setwaypoint->child(2)->setText(1,QString::number(setwaypointmsg.position_ned[1]));
	setwaypoint->child(3)->setText(1,QString::number(setwaypointmsg.position_ned[2]));

	setwaypoint->child(4)->setText(0,tr("RPY"));
	setwaypoint->child(4)->setText(1,QString::number(setwaypointmsg.rpy[0]));
	setwaypoint->child(5)->setText(1,QString::number(setwaypointmsg.rpy[1]));
	setwaypoint->child(6)->setText(1,QString::number(setwaypointmsg.rpy[2]));
}

void MainWindow::onDepthInfoReceived()
{
	// Edit SetWaypoint data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("Depth"), Qt::MatchWildcard);

	QTreeWidgetItem *depth = found.first();

	depth->child(0)->setText(0,tr("Timestamp"));
	depth->child(0)->setText(1,QString::number(depthmsg.timestamp));

	depth->child(1)->setText(0,tr("Depth"));
	depth->child(1)->setText(1,QString::number(depthmsg.depth));

	depth->child(2)->setText(0,tr("humidity"));
	depth->child(2)->setText(1,QString::number(depthmsg.humidity));

	depth->child(3)->setText(0,tr("ThermisterTemp"));
	depth->child(3)->setText(1,QString::number(depthmsg.thermistertemp));

	depth->child(4)->setText(0,tr("HumidityTemp"));
	depth->child(4)->setText(1,QString::number(depthmsg.humiditytemp));
}

void MainWindow::onIMUInfoReceived()
{
	// Edit SetWaypoint data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("IMU"), Qt::MatchWildcard);

	QTreeWidgetItem *imu = found.first();

	imu->child(0)->setText(0,tr("Timestamp"));
	imu->child(0)->setText(1,QString::number(imumsg.timestamp));

	imu->child(1)->setText(0,tr("Flags"));
	imu->child(1)->setText(1,QString::number(imumsg.flags));

	imu->child(2)->setText(0,tr("Temp"));
	imu->child(2)->setText(1,QString::number(imumsg.temp));

	imu->child(3)->setText(0,tr("Supply"));
	imu->child(3)->setText(1,QString::number(imumsg.supply));

	imu->child(4)->setText(0,tr("Acceleration"));
	imu->child(4)->setText(1,QString::number(imumsg.acceleration[0]));
	imu->child(5)->setText(1,QString::number(imumsg.acceleration[1]));
	imu->child(6)->setText(1,QString::number(imumsg.acceleration[2]));

	imu->child(7)->setText(0,tr("Angular_Rate"));
	imu->child(7)->setText(1,QString::number(imumsg.angular_rate[0]));
	imu->child(8)->setText(1,QString::number(imumsg.angular_rate[1]));
	imu->child(8)->setText(1,QString::number(imumsg.angular_rate[2]));

	imu->child(9)->setText(0,tr("Mag_Field"));
	imu->child(9)->setText(1,QString::number(imumsg.mag_field[0]));
	imu->child(10)->setText(1,QString::number(imumsg.mag_field[1]));
	imu->child(11)->setText(1,QString::number(imumsg.mag_field[2]));
}

void MainWindow::onDVLInfoReceived()
{
	// Edit SetWaypoint data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("DVL"), Qt::MatchWildcard);

	QTreeWidgetItem *dvl = found.first();

	dvl->child(0)->setText(0,tr("Timestamp"));
	dvl->child(0)->setText(1,QString::number(dvlmsg.timestamp));

	dvl->child(1)->setText(0,tr("Velocity"));
	dvl->child(1)->setText(1,QString::number(dvlmsg.velocity[0]));
	dvl->child(2)->setText(1,QString::number(dvlmsg.velocity[1]));
	dvl->child(3)->setText(1,QString::number(dvlmsg.velocity[2]));

	dvl->child(4)->setText(0,tr("VelocityError"));
	dvl->child(4)->setText(1,QString::number(dvlmsg.velocityerror));

	dvl->child(5)->setText(0,tr("Height"));
	dvl->child(5)->setText(1,QString::number(dvlmsg.height));

	dvl->child(6)->setText(0,tr("BeamCorrelation"));
	dvl->child(6)->setText(1,QString::number(dvlmsg.beamcorrelation[0]));
	dvl->child(7)->setText(1,QString::number(dvlmsg.beamcorrelation[1]));
	dvl->child(8)->setText(1,QString::number(dvlmsg.beamcorrelation[2]));
	dvl->child(9)->setText(1,QString::number(dvlmsg.beamcorrelation[3]));

	dvl->child(10)->setText(0,tr("Good"));
	dvl->child(10)->setText(1,QString::number(dvlmsg.good));
}

void MainWindow::onPDStatusInfoReceived()
{
	// Edit SetWaypoint data
	QList<QTreeWidgetItem *> found = ui->treeDDS->findItems(
		QString("PDStatus"), Qt::MatchWildcard);

	QTreeWidgetItem *pdstatus = found.first();

	pdstatus->child(0)->setText(0,tr("Timestamp"));
	pdstatus->child(0)->setText(1,QString::number(pdstatusmsg.timestamp));

	pdstatus->child(1)->setText(0,tr("State"));
	pdstatus->child(1)->setText(1,QString::number(pdstatusmsg.state));

	pdstatus->child(2)->setText(0,tr("EStop"));
	pdstatus->child(2)->setText(1,QString::number(pdstatusmsg.estop));

	pdstatus->child(3)->setText(0,tr("Current"));
	pdstatus->child(3)->setText(1,QString::number(pdstatusmsg.current[0]));
	pdstatus->child(4)->setText(1,QString::number(pdstatusmsg.current[1]));
	pdstatus->child(5)->setText(1,QString::number(pdstatusmsg.current[2]));
	pdstatus->child(6)->setText(1,QString::number(pdstatusmsg.current[3]));
	pdstatus->child(7)->setText(1,QString::number(pdstatusmsg.current[4]));
	pdstatus->child(8)->setText(1,QString::number(pdstatusmsg.current[5]));
	pdstatus->child(9)->setText(1,QString::number(pdstatusmsg.current[6]));
	pdstatus->child(10)->setText(1,QString::number(pdstatusmsg.current[7]));

	pdstatus->child(11)->setText(0,tr("TickCount"));
	pdstatus->child(11)->setText(1,QString::number(pdstatusmsg.tickcount));

	pdstatus->child(12)->setText(0,tr("Flags"));
	pdstatus->child(12)->setText(1,QString::number(pdstatusmsg.flags));

	pdstatus->child(13)->setText(0,tr("Current16"));
	pdstatus->child(13)->setText(1,QString::number(pdstatusmsg.current16));

	pdstatus->child(14)->setText(0,tr("Voltage16"));
	pdstatus->child(14)->setText(1,QString::number(pdstatusmsg.voltage16));

	pdstatus->child(15)->setText(0,tr("Current32"));
	pdstatus->child(15)->setText(1,QString::number(pdstatusmsg.current32));

	pdstatus->child(16)->setText(0,tr("Voltage32"));
	pdstatus->child(16)->setText(1,QString::number(pdstatusmsg.voltage32));
}

//***************************************************************************
// CALLBACKS
//***************************************************************************
void MainWindow::LPOSVSSDDSReadCallback(const LPOSVSSMessage &msg)
{
	if (!lposvssData)
	{
		// Create Top Category
		QTreeWidgetItem *lposvss = new QTreeWidgetItem(ui->treeDDS);
		// Handle LPOSVSS
		lposvss->setText(0, tr("LPOSVSS"));

		int numOfMessages = 18;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(lposvss);

		lposvssData = true;
	}

	lposvssmsg = msg;
	emit lposvssInfoReceived();
}

void MainWindow::SetWaypointDDSReadCallback(const SetWaypointMessage &msg)
{
	if (!setwaypointData)
	{
		// Create Top Category
		QTreeWidgetItem *setwaypoint = new QTreeWidgetItem(ui->treeDDS);
		// Handle LPOSVSS
		setwaypoint->setText(0, tr("SetWaypoint"));

		int numOfMessages = 7;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(setwaypoint);

		setwaypointData = true;
	}

	setwaypointmsg = msg;
	emit setWaypointInfoReceived();
}

void MainWindow::DepthDDSReadCallback(const DepthMessage &msg)
{
	if (!depthData)
	{
		// Create Top Category
		QTreeWidgetItem *setwaypoint = new QTreeWidgetItem(ui->treeDDS);
		// Handle Depth
		setwaypoint->setText(0, tr("Depth"));

		int numOfMessages = 5;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(setwaypoint);

		depthData = true;
	}

	depthmsg = msg;
	emit depthInfoReceived();
}

void MainWindow::IMUDDSReadCallback(const IMUMessage &msg)
{
	if (!imuData)
	{
		// Create Top Category
		QTreeWidgetItem *imu = new QTreeWidgetItem(ui->treeDDS);
		// Handle IMU
		imu->setText(0, tr("IMU"));

		int numOfMessages = 13;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(imu);

		imuData = true;
	}

	imumsg = msg;
	emit imuInfoReceived();
}

void MainWindow::DVLDDSReadCallback(const DVLMessage &msg)
{
	if (!dvlData)
	{
		// Create Top Category
		QTreeWidgetItem *dvl = new QTreeWidgetItem(ui->treeDDS);
		// Handle DVL
		dvl->setText(0, tr("DVL"));

		int numOfMessages = 11;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(dvl);

		dvlData = true;
	}

	dvlmsg = msg;
	emit dvlInfoReceived();
}

void MainWindow::PDStatusDDSReadCallback(const PDStatusMessage &msg)
{
	if (!pdstatusData)
	{
		// Create Top Category
		QTreeWidgetItem *pdstatus = new QTreeWidgetItem(ui->treeDDS);
		// Handle PDStatus
		pdstatus->setText(0, tr("PDStatus"));

		int numOfMessages = 17;

		for (int i = 0; i< numOfMessages; i++)
			new QTreeWidgetItem(pdstatus);

		pdstatusData = true;
	}

	pdstatusmsg = msg;
	emit pdstatusInfoReceived();
}
