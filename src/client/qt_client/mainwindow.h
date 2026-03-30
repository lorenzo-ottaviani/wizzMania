#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QString>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectClicked();
    void onSendClicked();
    void onWizzClicked();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError();

private:
    void setupUI();
    void sendMessage(const QString &type, const QString &pseudo = "", const QString &content = "");

    QTcpSocket *socket;
    QLineEdit *pseudoEdit;
    QLineEdit *messageEdit;
    QTextEdit *chatDisplay;
    QPushButton *connectBtn;
    QPushButton *sendBtn;
    QPushButton *wizzBtn;
    QLabel *statusLabel;
    QString currentPseudo;
    bool isConnected = false;
    QByteArray buffer;
};

#endif
