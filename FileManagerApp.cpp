/*
 * Author: Manan Joshi
 * Description: Implementation file for FileManagerApp class. Contains the application
 *             entry point that creates and displays the main frame.
 * Date: February 1 2026
 */

#include "FileManagerApp.h"
#include "FileManagerFrame.h"

/**
 * Function: OnInit
 * Description: Application entry point. Creates and shows the main frame.
 * Parameters: None (override of wxApp::OnInit)
 * Returns: true if initialization succeeds, false otherwise
 */
bool FileManagerApp::OnInit() {
    FileManagerFrame* frame = new FileManagerFrame();
    frame->Show(true);
    return true;
}

/**
 * Function: ~FileManagerApp
 * Description: Destructor for FileManagerApp. Currently no special cleanup needed
 *              as wxWidgets handles resource management.
 * Parameters: None
 * Returns: None
 */
FileManagerApp::~FileManagerApp() {
    // No special cleanup needed
}

wxIMPLEMENT_APP(FileManagerApp);

