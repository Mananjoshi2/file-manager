/*
 * Author: Manan Joshi
 * Description: Header file for FileManagerFrame class. Declares the main frame
 *             class that contains the file listing, menu, status bar, and handles
 *             all file operations for the file manager application.
 * Date: February 1 2026
 */

#ifndef FILE_MANAGER_FRAME_H
#define FILE_MANAGER_FRAME_H

#include <filesystem>
#include <string>
#include <wx/listctrl.h>
#include <wx/wx.h>

/**
 * Main frame class for the file manager application.
 * Contains the file listing, menu, status bar, and handles all file operations.
 */
class FileManagerFrame : public wxFrame {
public:
    /**
     * Constructor for the main frame.
     * Initializes the UI components and sets up event handlers.
     */
    FileManagerFrame();

    /**
     * Destructor for FileManagerFrame.
     * Cleans up any resources if needed.
     */
    virtual ~FileManagerFrame();

private:
    // Control IDs
    enum {
        ID_PATH_TEXT = 1000,
        ID_FILE_LIST
    };

    // UI Components
    wxTextCtrl* m_pathTextCtrl;
    wxListCtrl* m_fileListCtrl;
    wxStatusBar* m_statusBar;
    wxMenuBar* m_menuBar;

    // File operations state
    std::filesystem::path m_currentPath;
    std::filesystem::path m_clipboardPath;
    bool m_isCutOperation;

    /**
     * Initializes the menu bar with all file operations.
     */
    void CreateMenuBar();

    /**
     * Initializes the file list control.
     * @param parent The parent window for the list control
     */
    void CreateFileList(wxWindow* parent);

    /**
     * Refreshes the file listing to show current directory contents.
     */
    void RefreshFileList();

    /**
     * Updates the path text control with the current directory.
     */
    void UpdatePathDisplay();

    /**
     * Gets the currently selected file path.
     * @return The path of the selected file, or empty path if none selected
     */
    std::filesystem::path GetSelectedPath() const;

    /**
     * Formats file size for display.
     * @param size The file size in bytes
     * @return Formatted size string (ex 1.5 KB, 2.3 MB)
     */
    std::string FormatFileSize(std::uintmax_t size) const;

    /**
     * Formats file modification time for display.
     * @param time The file time to format
     * @return Formatted date/time string
     */
    std::string FormatFileTime(const std::filesystem::file_time_type& time) const;

    // Event handlers
    void OnPathChanged(wxCommandEvent& event);
    void OnFileActivated(wxListEvent& event);
    void OnFileSelected(wxListEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnCreateDirectory(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif // FILE_MANAGER_FRAME_H

