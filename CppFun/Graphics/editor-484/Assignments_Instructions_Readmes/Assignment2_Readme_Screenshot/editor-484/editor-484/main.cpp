
#include "Application.h"


// Main function
int main(int argc, char** argv) {

    Application& app = Application::getInstance();
    
    app.initialize(argc, argv);  // Initialize the application
    app.run();                   // Run the application loop
	
    return 0;

}

