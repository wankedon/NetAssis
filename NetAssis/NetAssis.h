#pragma once
#include <QtWidgets/QWidget>
#include "ui_NetAssis.h"
#include "NetImpl.h"
#include <memory>

class QLabel;
class QTextEdit;
class QComboBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCheckBox;
class NetImpl;
class NetAssis : public QWidget
{
    Q_OBJECT

public:
    NetAssis(QWidget *parent = nullptr);
    ~NetAssis();

private:
    void initWidget();
    void initTransModelWidget();
    void initReceiveSetWidget();
    void initSendSetWidget();
    void initDisplayWidget();
    void setFrameStyle();
    void createLayout();
    QVBoxLayout* createtransSetLayout();
    QVBoxLayout* createRecSetLayout();
    QVBoxLayout* createSendSetLayout();
    template<class T1, class T2>
    QHBoxLayout* creatLableEdit(T1* widget1, T2* widget2)
    {
        widget2->setFixedHeight(40);
        QHBoxLayout* btLayout = new QHBoxLayout();
        btLayout->addWidget(widget1);
        btLayout->addWidget(widget2);
        return btLayout;
    }
    void connectSignalSlot();
    void onStart();
    void onReceiveData(const DataBlock& db, void* user);
    void onClose();

signals:
    void updateReceiveData(QString datastr);

public slots:
    void updateReceiveDataSlot(QString datastr);
    void workModelComboBoxSlot(QString workModel);
    void printRecSlot(int state);
    void loopSendSlot(int state);
    void startSlot();
    void sendSlot();
    void clearRecSlot();
    void clearSendSlot();
    void clearStatusSlot();

private:
    //工作模式
    QLabel* m_workModelLable = nullptr;
    QComboBox* m_workModelComboBox = nullptr;
    QLabel* m_localIpLable = nullptr;
    QTextEdit* m_localIpEdit = nullptr;
    QLabel* m_localPortLable = nullptr;
    QTextEdit* m_localPortEdit = nullptr;
    QLabel* m_destIpLable = nullptr;
    QTextEdit* m_destIpEdit = nullptr;
    QLabel* m_destPortLable = nullptr;
    QTextEdit* m_destPortEdit = nullptr;
    QPushButton* m_startButton = nullptr;
    //接收区设置
    QCheckBox* m_recHexCheck = nullptr;
    QCheckBox* m_printRecCheck = nullptr;
    QLabel* m_recBufferLable = nullptr;
    QTextEdit* m_recBufferEdit = nullptr;
    QLabel* m_bytesLable = nullptr;
    //发送区设置
    QCheckBox* m_sendHexCheck = nullptr;
    QCheckBox* m_loopSendCheck = nullptr;
    QLabel* m_sendIntervalLable = nullptr;
    QTextEdit* m_sendIntervalEdit = nullptr;
    QLabel* m_msLable = nullptr;
    //接收区
    QTextEdit* m_recRegionEdit = nullptr;
    QPushButton* m_clearRecButton = nullptr;
    //发送区
    QTextEdit* m_sendRegionEdit = nullptr;
    QPushButton* m_sendButton = nullptr;
    QPushButton* m_clearSendButton = nullptr;
    //状态区
    QTextEdit* m_statusEdit = nullptr;
    QPushButton* m_clearStatusButton = nullptr;


private:
    Ui::NetAssisClass ui;
    bool m_isStart;
    bool m_isPrintRec;
    uint64_t m_timeInterval;
    std::map<QString, WorkMode> m_workModelList;
    WorkMode m_workModel;
    std::vector<char> m_sendBuffer;
    std::unique_ptr<NetImpl> m_netImpl;

private:
    const int LABLE_WIDTH = 100;
    const int BUTTON_WIDTH = 125;
};