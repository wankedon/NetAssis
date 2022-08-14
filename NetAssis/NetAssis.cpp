#include "Header.h"
#include "Tools.h"
#include "NetAssis.h"
#include <QMetaType>

NetAssis::NetAssis(QWidget *parent)
    : QWidget(parent),
	m_isStart(false),
	m_timeInterval(0),
	m_isPrintRec(true),
	m_workModel(WorkMode::TCP_CLIENT)
{
	qRegisterMetaType<QString>("QString");
	auto handler = [this](const DataBlock& db, void* user)
	{
		onReceiveData(db, user);
	};
	m_netImpl = std::make_unique<NetImpl>(handler);
	m_workModelList["TCP客户端"] = TCP_CLIENT;
	m_workModelList["TCP服务端"] = TCP_SERVER;
	m_workModelList["UDP"] = UDP;
	initWidget();
	connectSignalSlot();
}

NetAssis::~NetAssis()
{
}

void NetAssis::onReceiveData(const DataBlock& db, void* user)
{
	std::vector<char> data(db.length);
	memcpy(data.data(), db.addr, db.length);
	string datastr(data.begin(), data.end());
	if (m_isPrintRec)
		emit this->updateReceiveData(QString::fromStdString(datastr));
}

void NetAssis::initWidget()
{
	initTransModelWidget();
	initReceiveSetWidget();
	initSendSetWidget();
	initDisplayWidget();
	setFrameStyle();
	createLayout();
}

void NetAssis::initTransModelWidget()
{
	m_workModelLable = new QLabel("工作模式:", this);
	m_workModelComboBox = new QComboBox(this);
	m_workModelComboBox->addItems({"TCP客户端","TCP服务端","UDP"});
	m_localIpLable = new QLabel("本地IP:", this);
	m_localIpEdit = new QTextEdit(QString::fromStdString(getLocalIp()),this);
	m_localIpEdit->setEnabled(false);
	m_localPortLable = new QLabel("本地端口:", this);
	m_localPortEdit = new QTextEdit("5555",this);
	m_localPortEdit->setEnabled(false);
	m_destIpLable = new QLabel("目的IP:", this);
	m_destIpEdit = new QTextEdit("127.0.0.1",this);
	m_destPortLable = new QLabel("目的端口:", this);
	m_destPortEdit = new QTextEdit("5151", this);
	m_startButton = new QPushButton("打开", this);
	m_startButton->setFixedWidth(BUTTON_WIDTH);
}

void NetAssis::initReceiveSetWidget()
{
	m_recHexCheck = new QCheckBox("十六进制显示",this);
	m_printRecCheck = new QCheckBox("暂停接收显示", this);
	m_recBufferLable = new QLabel("接收缓冲区", this);
	m_recBufferEdit = new QTextEdit("1024", this);
	m_bytesLable = new QLabel("字节", this);
}

void NetAssis::initSendSetWidget()
{
	m_sendHexCheck = new QCheckBox("十六进制显示", this);
	m_loopSendCheck = new QCheckBox("指令循环发送", this);
	m_loopSendCheck->setEnabled(false);
	m_sendIntervalLable = new QLabel("指令间隔", this);
	m_sendIntervalEdit = new QTextEdit("1000", this);
	m_msLable = new QLabel("毫秒", this);
}

void NetAssis::initDisplayWidget()
{
	m_recRegionEdit = new QTextEdit(this);
	m_recRegionEdit->setEnabled(false);
	m_clearRecButton = new QPushButton("清空显示", this);
	m_clearRecButton->setFixedWidth(BUTTON_WIDTH);
	m_sendRegionEdit = new QTextEdit(this);
	m_sendButton = new QPushButton("发送", this);
	m_sendButton->setEnabled(false);
	m_sendButton->setFixedWidth(BUTTON_WIDTH);
	m_clearSendButton = new QPushButton("清空显示", this);
	m_clearSendButton->setFixedWidth(BUTTON_WIDTH);
	m_statusEdit = new QTextEdit(this);
	m_statusEdit->setEnabled(false);
	m_clearStatusButton= new QPushButton("清空显示", this);
	m_clearStatusButton->setFixedWidth(BUTTON_WIDTH);
}

void NetAssis::setFrameStyle()
{
	m_workModelLable->setFixedWidth(LABLE_WIDTH);
	m_localIpLable->setFixedWidth(LABLE_WIDTH);
	m_localPortLable->setFixedWidth(LABLE_WIDTH);
	m_destIpLable->setFixedWidth(LABLE_WIDTH);
	m_destPortLable->setFixedWidth(LABLE_WIDTH);
	m_localIpEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_localPortEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_destIpEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_destPortEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_recBufferEdit->setFixedHeight(40);
	m_recBufferEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_sendIntervalEdit->setFixedHeight(40);
	m_sendIntervalEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_recRegionEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_sendRegionEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
	m_statusEdit->setFrameStyle(QFrame::Box | QFrame::Raised);
}

void NetAssis::createLayout()
{
	//通信设置
	QGroupBox* transSetGroupBox = new QGroupBox("通信设置", this);
	transSetGroupBox->setLayout(createtransSetLayout());
	//transSetGroupBox->setStyleSheet("QGroupBox{border: 1px solid gray;margin-top: 0.5ex;} QGroupBox::title { subcontrol-origin: margin;subcontrol-position: top left;left:10px;margin - left: 0px;padding:0 1px;}");
	//transSetGroupBox->setStyleSheet("QGroupBox::title { subcontrol-origin: margin;subcontrol-position: top left;left:10px;margin - left: 0px;padding:0 1px;}");
	QGroupBox* recSetGroupBox = new QGroupBox("接收区设置", this);
	recSetGroupBox->setLayout(createRecSetLayout());
	QGroupBox* sendSetGroupBox = new QGroupBox("发送区设置", this);
	sendSetGroupBox->setLayout(createSendSetLayout());
	//接收区
	QGroupBox* recRegionGroupBox = new QGroupBox("接收区", this);
	QVBoxLayout* recRegionLayout = new QVBoxLayout();
	recRegionLayout->addWidget(m_recRegionEdit);
	QHBoxLayout* m_clearRecButtonLayout = new QHBoxLayout();
	m_clearRecButtonLayout->addStretch();
	m_clearRecButtonLayout->addWidget(m_clearRecButton);
	recRegionLayout->addLayout(m_clearRecButtonLayout);
	recRegionGroupBox->setLayout(recRegionLayout);
	//发送区
	QGroupBox* sendRegionGroupBox = new QGroupBox("发送区", this);
	QVBoxLayout* sendRegionLayout = new QVBoxLayout();
	sendRegionLayout->addWidget(m_sendRegionEdit);
	QHBoxLayout* clearSendButtonLayout = new QHBoxLayout();
	clearSendButtonLayout->addStretch();
	clearSendButtonLayout->addWidget(m_sendButton);
	clearSendButtonLayout->addWidget(m_clearSendButton);
	sendRegionLayout->addLayout(clearSendButtonLayout);
	sendRegionGroupBox->setLayout(sendRegionLayout);
	//状态区
	QGroupBox* statusGroupBox = new QGroupBox("状态区", this);
	QVBoxLayout* statusLayout = new QVBoxLayout();
	statusLayout->addWidget(m_statusEdit);
	QHBoxLayout* clearStatusButtonLayout = new QHBoxLayout();
	clearStatusButtonLayout->addStretch();
	clearStatusButtonLayout->addWidget(m_clearStatusButton);
	statusLayout->addLayout(clearStatusButtonLayout);
	statusGroupBox->setLayout(statusLayout);
	//设置区
	QVBoxLayout* setlayout = new QVBoxLayout();
	setlayout->addWidget(transSetGroupBox, 1);
	setlayout->addWidget(recSetGroupBox, 1);
	setlayout->addWidget(sendSetGroupBox, 1);
	QHBoxLayout* m_startButtonLayout = new QHBoxLayout();
	m_startButtonLayout->addStretch();
	m_startButtonLayout->addWidget(m_startButton);
	setlayout->addLayout(m_startButtonLayout);
	//显示区
	QVBoxLayout* displaylayout = new QVBoxLayout();
	displaylayout->addWidget(recRegionGroupBox, 2);
	displaylayout->addWidget(sendRegionGroupBox, 2);
	displaylayout->addWidget(statusGroupBox, 1);
	QHBoxLayout* layout = new QHBoxLayout(this);
	//总布局
	layout->addLayout(setlayout, 1);
	layout->addLayout(displaylayout, 4);
	setLayout(layout);
}

QVBoxLayout* NetAssis::createtransSetLayout()
{
	auto transSetlayout = creatLableEdit(m_workModelLable, m_workModelComboBox);
	auto localiplayout = creatLableEdit(m_localIpLable, m_localIpEdit);
	auto localportllayout = creatLableEdit(m_localPortLable, m_localPortEdit);
	auto destiplayout = creatLableEdit(m_destIpLable, m_destIpEdit);
	auto destportllayout = creatLableEdit(m_destPortLable, m_destPortEdit);
	QVBoxLayout* transSetBoxLayout = new QVBoxLayout();
	transSetBoxLayout->addLayout(transSetlayout);
	transSetBoxLayout->addLayout(localiplayout);
	transSetBoxLayout->addLayout(localportllayout);
	transSetBoxLayout->addLayout(destiplayout);
	transSetBoxLayout->addLayout(destportllayout);
	transSetBoxLayout->addStretch();
	return transSetBoxLayout;
}

QVBoxLayout* NetAssis::createRecSetLayout()
{
	QVBoxLayout* recSetBoxLayout = new QVBoxLayout();
	recSetBoxLayout->addWidget(m_recHexCheck);
	recSetBoxLayout->addWidget(m_printRecCheck);
	QHBoxLayout* recBufferLayout = new QHBoxLayout();
	recBufferLayout->addWidget(m_recBufferLable);
	recBufferLayout->addWidget(m_recBufferEdit);
	recBufferLayout->addWidget(m_bytesLable);
	recSetBoxLayout->addLayout(recBufferLayout);
	return recSetBoxLayout;
}

QVBoxLayout* NetAssis::createSendSetLayout()
{
	QVBoxLayout* sendSetBoxLayout = new QVBoxLayout();
	sendSetBoxLayout->addWidget(m_sendHexCheck);
	sendSetBoxLayout->addWidget(m_loopSendCheck);
	QHBoxLayout* sendintervalLayout = new QHBoxLayout();
	sendintervalLayout->addWidget(m_sendIntervalLable);
	sendintervalLayout->addWidget(m_sendIntervalEdit);
	sendintervalLayout->addWidget(m_msLable);
	sendSetBoxLayout->addLayout(sendintervalLayout);
	return sendSetBoxLayout;
}

void NetAssis::connectSignalSlot()
{
	connect(this, SIGNAL(updateReceiveData(QString)), this, SLOT(updateReceiveDataSlot(QString)));
	connect(m_workModelComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(workModelComboBoxSlot(QString)));
	connect(m_printRecCheck, SIGNAL(stateChanged(int)), this, SLOT(printRecSlot(int)));
	connect(m_loopSendCheck, SIGNAL(stateChanged(int)), this, SLOT(loopSendSlot(int)));
	connect(m_startButton, SIGNAL(clicked(bool)), this, SLOT(startSlot()));
	connect(m_sendButton, SIGNAL(clicked(bool)), this, SLOT(sendSlot()));
	connect(m_clearRecButton, SIGNAL(clicked(bool)), this, SLOT(clearRecSlot()));
	connect(m_clearSendButton, SIGNAL(clicked(bool)), this, SLOT(clearSendSlot()));
	connect(m_clearStatusButton, SIGNAL(clicked(bool)), this, SLOT(clearStatusSlot()));
}

void NetAssis::updateReceiveDataSlot(QString datastr)
{
	m_recRegionEdit->insertPlainText(datastr);
}

void NetAssis::workModelComboBoxSlot(QString workModel)
{
	m_workModel = m_workModelList[workModel];
	switch (m_workModel)
	{
	case WorkMode::TCP_CLIENT:
		m_localPortEdit->setEnabled(false);
		m_destIpEdit->setEnabled(true);
		m_destPortEdit->setEnabled(true);
		break;
	case WorkMode::TCP_SERVER:
		m_localPortEdit->setEnabled(true);
		m_destIpEdit->setEnabled(false);
		m_destPortEdit->setEnabled(false);
		break;
	case WorkMode::UDP:
		m_localPortEdit->setEnabled(true);
		m_destIpEdit->setEnabled(true);
		m_destPortEdit->setEnabled(true);
		break;
	default:
		break;
	}
}

void NetAssis::printRecSlot(int state)
{
	m_isPrintRec = (state == 2) ? false : true;
}

void NetAssis::loopSendSlot(int state)
{
	if (state == 2 && m_timeInterval == 0)
	{
		m_timeInterval = m_sendIntervalEdit->toPlainText().toUInt();
		m_netImpl->startTimer(m_timeInterval);
	}
	m_netImpl->setLoopSend(state);
}

void NetAssis::startSlot()
{
	m_isStart = (m_isStart == false) ? true : false;
	if (m_isStart)
		onStart();
	else
		onClose();
}

void NetAssis::onStart()
{
	m_netImpl->creatTransport(m_workModel);
	auto destIp = m_destIpEdit->toPlainText().toStdString();
	auto destPort = m_destPortEdit->toPlainText().toStdString();
	if (m_workModel == WorkMode::TCP_CLIENT || m_workModel == WorkMode::UDP)
		m_netImpl->setDestEndpoint((destIp + ":" + destPort).c_str());
	if (m_workModel == WorkMode::TCP_SERVER || m_workModel == WorkMode::UDP)
		m_netImpl->setLocalPort(m_localPortEdit->toPlainText().toShort());
	m_netImpl->start();
	m_localIpEdit->setPlainText(QString::fromStdString(getLocalIp()));
	
	if (m_workModel == WorkMode::TCP_CLIENT)
		m_statusEdit->append("TCP Client Connected to " + QString::fromStdString(destIp + ":" + destPort));
	if (m_workModel == WorkMode::TCP_SERVER)
		m_statusEdit->append("TCP Server Listen at port " + QString::fromStdString(destPort));
	if (m_workModel == WorkMode::UDP)
		m_statusEdit->append("UDP " + QString::fromStdString("Listen at port "+destPort) + QString::fromStdString(" Connected to " + destIp));
	m_sendButton->setEnabled(true);
	m_startButton->setText("关闭");
}

void NetAssis::sendSlot()
{
	auto inputStr = m_sendRegionEdit->toPlainText().toStdString();
	auto inputChar = inputStr.c_str();
	m_sendBuffer.clear();
	m_sendBuffer.resize(inputStr.size());
	memcpy(m_sendBuffer.data(), inputChar, m_sendBuffer.size());
	DataBlock db{ m_sendBuffer.data(),m_sendBuffer.size() };
	m_netImpl->send(db);
	m_loopSendCheck->setEnabled(true);
}

void NetAssis::onClose()
{
	m_netImpl->close();
	m_statusEdit->append("停止");
	m_printRecCheck->setChecked(false);
	m_isPrintRec = true;
	m_loopSendCheck->setChecked(false);
	m_loopSendCheck->setEnabled(false);
	m_timeInterval = 0;
	m_sendButton->setEnabled(false);
	m_startButton->setText("打开");
}

void NetAssis::clearRecSlot()
{
	m_recRegionEdit->clear();
}

void NetAssis::clearSendSlot()
{
	m_sendRegionEdit->clear();
}

void NetAssis::clearStatusSlot()
{
	m_statusEdit->clear();
}