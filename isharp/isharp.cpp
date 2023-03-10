#include <Windows.h>
#include <fstream>
#include <atlstr.h>
#include <string>

using namespace std;

#include "stdafx.h"
#include "helperclasses.h"
#include "ScriptHandler.h"

/**

Entry point for the program.
This function is responsible for setting up the main window and
running the program loop.
@param hInstance Handle to the current instance of the application.
@param hPrevInstance Handle to the previous instance of the application.
@param lpCmdLine Pointer to the command line string.
@param nCmdShow Specifies how the window is to be shown.
@return Returns an integer exit code.
*/
int _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    // Convert the command line to a string
    std::string argv(lpCmdLine);

    // Remove any quotes from the command line string
    argv.erase(remove(argv.begin(), argv.end(), '"'), argv.end());

    // Open the script file
    std::ifstream file(argv);
    std::string str;
    // Vector to store the contents of the script file
    vector<string> file_contents;
    int counter = 0;
    // Read each line of the file and store it in the vector
    while (std::getline(file, str)) {
        file_contents.resize(counter + 2);
        file_contents[counter] = str;
        counter++;
    }

    // Create a ScriptHandler object
    ScriptHandler scriptHandler;
    // Run the script with the contents of the file and the command line argument
    scriptHandler.runScript(file_contents, argv);
    // Return success
    return 0;
}