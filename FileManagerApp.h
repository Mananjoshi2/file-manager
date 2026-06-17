/*
 * Author: Manan Joshi
 * Description: Header file for FileManagerApp class. Declares the main application
 *             class that handles application initialization and main frame creation.
 * Date: February 1 2026
 */

#ifndef FILE_MANAGER_APP_H
#define FILE_MANAGER_APP_H

#include <wx/wx.h>

/**
 * Main application class for the file manager.
 * Handles application initialization and main frame creation.
 */
class FileManagerApp : public wxApp {
public:
    /**
     * Initializes the application and creates the main frame.
     * @return true if initialization succeeds, false otherwise
     */
    virtual bool OnInit() override;

    /**
     * Destructor for FileManagerApp.
     * Cleans up any resources if needed.
     */
    virtual ~FileManagerApp();
};

#endif // FILE_MANAGER_APP_H

