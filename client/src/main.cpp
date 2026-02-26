#include "ChatClient.hpp"

#include <QCoreApplication>

int main(int argc, char *argv[]) {
    // Start Qt console application
    QCoreApplication app(argc, argv);

    // Call the client class
    ChatClient client;

    // Launch the event loop
    return app.exec();
}
