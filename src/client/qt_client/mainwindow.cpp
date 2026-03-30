#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), socket(nullptr) {
    setupUI();
}

MainWindow::~MainWindow() {
    if (socket) {
        socket->close();
        delete socket;
    }
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Titre
    QLabel *title = new QLabel("🌊 WIZZ MANIA 🌊");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #00c8ff; margin: 10px;");

    // Connexion
    QHBoxLayout *connectLayout = new QHBoxLayout();
    pseudoEdit = new QLineEdit();
    pseudoEdit->setPlaceholderText("Entrez votre pseudo...");
    pseudoEdit->setMaxLength(20);
    connectBtn = new QPushButton("Se connecter");
    connectLayout->addWidget(pseudoEdit);
    connectLayout->addWidget(connectBtn);

    // Status
    statusLabel = new QLabel("❌ Déconnecté");
    statusLabel->setStyleSheet("padding: 5px; background-color: #8f0f0f; color: #fff; border-radius: 3px;");
    statusLabel->setAlignment(Qt::AlignCenter);

    // Chat display
    chatDisplay = new QTextEdit();
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet("background-color: #0f3460; color: #eee; border: 1px solid #00c8ff;");

    // Message input
    QHBoxLayout *messageLayout = new QHBoxLayout();
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText("Votre message...");
    messageEdit->setEnabled(false);
    sendBtn = new QPushButton("Envoyer");
    sendBtn->setEnabled(false);
    wizzBtn = new QPushButton("WIZZ!");
    wizzBtn->setStyleSheet("background-color: #ff6b9d; color: white; font-weight: bold;");
    wizzBtn->setEnabled(false);
    messageLayout->addWidget(messageEdit);
    messageLayout->addWidget(sendBtn);
    messageLayout->addWidget(wizzBtn);

    // Layout assembly
    mainLayout->addWidget(title);
    mainLayout->addLayout(connectLayout);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(chatDisplay);
    mainLayout->addLayout(messageLayout);

    setWindowTitle("WIZZ Mania - Chat Client");
    resize(700, 600);

    // Connections
    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(wizzBtn, &QPushButton::clicked, this, &MainWindow::onWizzClicked);
    connect(messageEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
}

void MainWindow::onConnectClicked() {
    if (!isConnected) {
        currentPseudo = pseudoEdit->text().trimmed();
        if (currentPseudo.isEmpty()) {
            chatDisplay->append("❌ Veuillez entrer un pseudo!");
            return;
        }
        
        socket = new QTcpSocket(this);
        connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
        connect(socket, 
                static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::errorOccurred),
                this, &MainWindow::onError);

        socket->connectToHost("127.0.0.1", 8080);
        statusLabel->setText("⏳ Connexion...");
        statusLabel->setStyleSheet("padding: 5px; background-color: #8f8f0f; color: #fff; border-radius: 3px;");
    } else {
        socket->close();
    }
}

void MainWindow::onConnected() {
    statusLabel->setText("✅ Connecté");
    statusLabel->setStyleSheet("padding: 5px; background-color: #0f8f0f; color: #fff; border-radius: 3px;");
    
    pseudoEdit->setEnabled(false);
    connectBtn->setText("Se déconnecter");
    messageEdit->setEnabled(true);
    sendBtn->setEnabled(true);
    wizzBtn->setEnabled(true);
    isConnected = true;
    
    chatDisplay->append("✅ Connecté au serveur!");
    
    QJsonObject json;
    json["type"] = "JOIN";
    json["pseudo"] = currentPseudo;
    QJsonDocument doc(json);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void MainWindow::onDisconnected() {
    statusLabel->setText("❌ Déconnecté");
    statusLabel->setStyleSheet("padding: 5px; background-color: #8f0f0f; color: #fff; border-radius: 3px;");
    
    pseudoEdit->setEnabled(true);
    connectBtn->setText("Se connecter");
    messageEdit->setEnabled(false);
    sendBtn->setEnabled(false);
    wizzBtn->setEnabled(false);
    isConnected = false;
    
    chatDisplay->append("\n--- Déconnecté du serveur ---\n");
}

void MainWindow::onSendClicked() {
    QString msg = messageEdit->text().trimmed();
    if (!msg.isEmpty() && socket && socket->state() == QTcpSocket::ConnectedState) {
        QJsonObject json;
        json["type"] = "MESSAGE";
        json["pseudo"] = currentPseudo;
        json["content"] = msg;
        QJsonDocument doc(json);
        socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
        messageEdit->clear();
    }
}

void MainWindow::onWizzClicked() {
    if (socket && socket->state() == QTcpSocket::ConnectedState) {
        QJsonObject json;
        json["type"] = "WIZZ";
        json["pseudo"] = currentPseudo;
        QJsonDocument doc(json);
        socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
        chatDisplay->append("🌊 Vous avez envoyé un WIZZ!");
    }
}

void MainWindow::onReadyRead() {
    if (!socket) return;
    
    buffer.append(socket->readAll());
    
    int newlinePos;
    while ((newlinePos = buffer.indexOf('\n')) != -1) {
        QByteArray line = buffer.left(newlinePos);
        buffer.remove(0, newlinePos + 1);
        
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) continue;
        
        QJsonObject json = doc.object();
        QString type = json["type"].toString();
        
        if (type == "JOIN_OK") {
            chatDisplay->append("✅ Connexion confirmée!");
        } else if (type == "MESSAGE") {
            chatDisplay->append(QString("%1: %2")
                .arg(json["pseudo"].toString(), json["content"].toString()));
        } else if (type == "JOIN") {
            chatDisplay->append(QString("➕ %1 a rejoint le chat")
                .arg(json["pseudo"].toString()));
        } else if (type == "LEAVE") {
            chatDisplay->append(QString("➖ %1 a quitté le chat")
                .arg(json["pseudo"].toString()));
        } else if (type == "WIZZ") {
            chatDisplay->append(QString("🌊🌊🌊 %1 VOUS FAIT UN WIZZ! 🌊🌊🌊")
                .arg(json["pseudo"].toString()));
        } else if (type == "USER_LIST") {
            QString list = "👥 Utilisateurs: ";
            QJsonArray users = json["users"].toArray();
            for (const auto &u : users) {
                list += u.toString() + ", ";
            }
            chatDisplay->append(list);
        }
    }
}

void MainWindow::onError() {
    if (socket) {
        chatDisplay->append(QString("❌ Erreur: %1").arg(socket->errorString()));
    }
}
