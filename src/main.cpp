#include <iostream>
#include <thread>
#include <string>

// --- Libraries ---
#include <crow_all.h>
#include <QApplication>
#include <QPushButton>

/**
 * FUNCTION: test_crow
 * Starts the Web Server on port 18080.
 * NOTE: This function blocks (infinite loop), so we will run it in a thread.
 */
void test_crow()
{
    crow::SimpleApp app;

    // Route 1: Root
    CROW_ROUTE(app, "/")([](){
        return "Hello world from Crow Server!";
    });

    // Route 2: JSON Test
    CROW_ROUTE(app, "/json")
    ([]{
        crow::json::wvalue x({{"message", "Hello, World!"}});
        x["message2"] = "Hello, World.. Again!";
        return x;
    });

    // Route 3: Dynamic URL Test
    CROW_ROUTE(app,"/hello/<int>")
    ([](int count){
        return crow::response(std::to_string(count));
    });

    // Log to console
    std::cout << "[CROW] Server starting on port 18080..." << std::endl;

    // Run the server (Multi-threaded to handle requests properly)
    app.port(18080).multithreaded().run();
}

/**
 * FUNCTION: test_qt
 * Starts the GUI Application.
 * NOTE: GUI must always run on the main thread.
 */
int test_qt(int argc, char *argv[])
{
    // 1. Create the application (Mandatory)
    QApplication app(argc, argv);

    // 2. Create a widget (here a simple button)
    QPushButton button("Qt works! The Server is running too.\nCheck http://localhost:18080");

    // 3. Set the size
    button.resize(400, 200);

    // 4. Show the window
    button.show();

    std::cout << "[QT] Window displayed." << std::endl;

    // 5. Start the event loop
    return app.exec();
}

int main(int argc, char *argv[])
{
    std::cout << "=== STARTING WIZZ AIR TEST ===" << std::endl;

    // --- STEP 1: Start Crow in a separate thread ---
    // We use std::thread so it doesn't block Qt from starting.
    std::thread crowThread(test_crow);

    // --- STEP 2: Start Qt in the main thread ---
    // This will block main() until the window is closed.
    int qtResult = test_qt(argc, argv);

    // --- STEP 3: Cleanup ---
    // When the Qt window is closed, we arrive here.
    // Ideally, we should stop Crow gracefully, but for a test,
    // we simply detach the thread (it will die when the program ends).
    crowThread.detach();

    std::cout << "=== END OF PROGRAM ===" << std::endl;

    return qtResult;
}
