/*
 * Author: Manan Joshi
 * Description: Implementation file for FileManagerFrame class. Contains all
 *             implementations for the file manager UI components and file operations
 *             including open, create, rename, delete, copy, cut, and paste.
 * Date: February 1 2026
 */

#include "FileManagerFrame.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

// Event table for menu and control events
wxBEGIN_EVENT_TABLE(FileManagerFrame, wxFrame)
    EVT_TEXT(ID_PATH_TEXT, FileManagerFrame::OnPathChanged)
    EVT_LIST_ITEM_ACTIVATED(ID_FILE_LIST, FileManagerFrame::OnFileActivated)
    EVT_LIST_ITEM_SELECTED(ID_FILE_LIST, FileManagerFrame::OnFileSelected)
    EVT_MENU(wxID_OPEN, FileManagerFrame::OnOpen)
    EVT_MENU(wxID_NEW, FileManagerFrame::OnCreateDirectory)
    EVT_MENU(wxID_SAVEAS, FileManagerFrame::OnRename)
    EVT_MENU(wxID_DELETE, FileManagerFrame::OnDelete)
    EVT_MENU(wxID_COPY, FileManagerFrame::OnCopy)
    EVT_MENU(wxID_CUT, FileManagerFrame::OnCut)
    EVT_MENU(wxID_PASTE, FileManagerFrame::OnPaste)
    EVT_MENU(wxID_REFRESH, FileManagerFrame::OnRefresh)
    EVT_MENU(wxID_EXIT, FileManagerFrame::OnExit)
wxEND_EVENT_TABLE()

/**
 * Function: FileManagerFrame (Constructor)
 * Description: Initializes the frame and all UI components including the menu bar,
 *              path text control, file list control, and status bar. Sets up the
 *              initial directory to the current working directory or home directory
 *              if that fails. Initializes all member variables to appropriate defaults.
 * Parameters: None
 * Returns: None
 */
FileManagerFrame::FileManagerFrame()
    : wxFrame(nullptr, wxID_ANY, "File Manager", wxDefaultPosition, wxSize(800, 600)),
      m_clipboardPath(),
      m_isCutOperation(false) {
    
    // Set current path to current working directory, fallback to home or root
    try {
        m_currentPath = std::filesystem::current_path();
    } catch (const std::filesystem::filesystem_error& e) {
        m_currentPath = std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "/");
    }

    // Create menu bar
    CreateMenuBar();
    SetMenuBar(m_menuBar);

    // Create main panel
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Create path text control
    wxBoxSizer* pathSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pathLabel = new wxStaticText(panel, wxID_ANY, "Path:");
    m_pathTextCtrl = new wxTextCtrl(panel, ID_PATH_TEXT, "", wxDefaultPosition, wxDefaultSize);
    pathSizer->Add(pathLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    pathSizer->Add(m_pathTextCtrl, 1, wxALL | wxEXPAND, 5);

    // Create file list
    CreateFileList(panel);
    wxBoxSizer* listSizer = new wxBoxSizer(wxHORIZONTAL);
    listSizer->Add(m_fileListCtrl, 1, wxALL | wxEXPAND, 5);

    // Main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(pathSizer, 0, wxEXPAND);
    mainSizer->Add(listSizer, 1, wxEXPAND);
    panel->SetSizer(mainSizer);

    // Create status bar
    m_statusBar = CreateStatusBar();
    m_statusBar->SetStatusText("Ready");

    // Initial display
    UpdatePathDisplay();
    RefreshFileList();
}

/**
 * Function: CreateMenuBar
 * Description: Creates the menu bar with all file operations and keyboard shortcuts.
 *              Sets up the File menu with options for open, create directory, rename,
 *              delete, copy, cut, paste, refresh, and exit operations.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::CreateMenuBar() {
    m_menuBar = new wxMenuBar();

    // File menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open selected file or directory");
    fileMenu->Append(wxID_NEW, "&New Directory\tCtrl+N", "Create a new directory");
    fileMenu->Append(wxID_SAVEAS, "&Rename\tCtrl+R", "Rename selected file or directory");
    fileMenu->Append(wxID_DELETE, "&Delete\tDel", "Delete selected file or directory");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_COPY, "&Copy\tCtrl+C", "Copy selected file or directory");
    fileMenu->Append(wxID_CUT, "Cu&t\tCtrl+X", "Cut selected file or directory");
    fileMenu->Append(wxID_PASTE, "&Paste\tCtrl+V", "Paste file or directory");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_REFRESH, "&Refresh\tF5", "Refresh file listing");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tCtrl+Q", "Exit the application");

    m_menuBar->Append(fileMenu, "&File");
}

/**
 * Function: CreateFileList
 * Description: Creates and configures the file list control with columns for name,
 *              type, size, and modification date. Sets up the list in report view
 *              mode with single selection enabled.
 * Parameters: parent - The parent window for the list control
 * Returns: None
 */
void FileManagerFrame::CreateFileList(wxWindow* parent) {
    m_fileListCtrl = new wxListCtrl(parent, ID_FILE_LIST, wxDefaultPosition, wxDefaultSize,
                                     wxLC_REPORT | wxLC_SINGLE_SEL);

    // Add columns
    m_fileListCtrl->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 200);
    m_fileListCtrl->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 100);
    m_fileListCtrl->InsertColumn(2, "Size", wxLIST_FORMAT_RIGHT, 100);
    m_fileListCtrl->InsertColumn(3, "Modified", wxLIST_FORMAT_LEFT, 200);
}

/**
 * Function: RefreshFileList
 * Description: Refreshes the file listing to show current directory contents.
 *              Clears the existing list and repopulates it with files and directories
 *              from the current path, including a parent directory entry if applicable.
 *              Displays file properties including name, type, size, and modification date.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::RefreshFileList() {
    m_fileListCtrl->DeleteAllItems();

    try {
        // Add parent directory entry
        if (m_currentPath.has_parent_path() && m_currentPath != m_currentPath.root_path()) {
            long index = m_fileListCtrl->InsertItem(0, "..");
            m_fileListCtrl->SetItem(index, 1, "Directory");
            m_fileListCtrl->SetItem(index, 2, "-");
            m_fileListCtrl->SetItem(index, 3, "-");
        }

        // Iterate through directory contents
        int itemIndex = m_fileListCtrl->GetItemCount();
        for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
            try {
                std::string name = entry.path().filename().string();
                std::string type = entry.is_directory() ? "Directory" : "File";
                std::string size = "-";
                std::string modified = "-";

                if (entry.is_regular_file()) {
                    size = FormatFileSize(entry.file_size());
                }

                if (entry.exists()) {
                    auto time = entry.last_write_time();
                    modified = FormatFileTime(time);
                }

                long index = m_fileListCtrl->InsertItem(itemIndex, name);
                m_fileListCtrl->SetItem(index, 1, type);
                m_fileListCtrl->SetItem(index, 2, size);
                m_fileListCtrl->SetItem(index, 3, modified);
                itemIndex++;
            } catch (const std::filesystem::filesystem_error& e) {
                // Skip entries that can't be accessed
                continue;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        wxMessageBox("Error reading directory: " + std::string(e.what()),
                     "Error", wxOK | wxICON_ERROR);
    }
}

/**
 * Function: UpdatePathDisplay
 * Description: Updates the path text control with the current directory path.
 *              Converts the filesystem path to a string and displays it in the
 *              editable path text control.
 * Parameters: None
 * Returns: None
 */
void FileManagerFrame::UpdatePathDisplay() {
    m_pathTextCtrl->SetValue(m_currentPath.string());
}

/**
 * Function: GetSelectedPath
 * Description: Gets the currently selected file or directory path from the list control.
 *              Returns an empty path if no item is selected. Handles the special case
 *              of the ".." parent directory entry.
 * Parameters: None
 * Returns: The filesystem path of the selected item, or empty path if none selected
 */
std::filesystem::path FileManagerFrame::GetSelectedPath() const {
    long selected = m_fileListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected == -1) {
        return std::filesystem::path();
    }

    wxString name = m_fileListCtrl->GetItemText(selected);
    if (name == "..") {
        return m_currentPath.parent_path();
    }

    return m_currentPath / name.ToStdString();
}

/**
 * Function: FormatFileSize
 * Description: Formats file size in bytes to a human-readable string with appropriate
 *              units (B, KB, MB, GB, TB). Converts the size to the largest unit that
 *              results in a value >= 1.0.
 * Parameters: size - The file size in bytes
 * Returns: Formatted size string (e.g., "1.5 KB", "2.3 MB")
 */
std::string FileManagerFrame::FormatFileSize(std::uintmax_t size) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double fileSize = static_cast<double>(size);
    int unitIndex = 0;

    while (fileSize >= 1024.0 && unitIndex < 4) {
        fileSize /= 1024.0;
        unitIndex++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << fileSize << " " << units[unitIndex];
    return oss.str();
}

/**
 * Function: FormatFileTime
 * Description: Formats file modification time for display. Uses a cross-platform
 *              approach to convert filesystem time to a readable date/time string.
 *              Converts the time to system_clock, then to time_t, and formats using
 *              strftime with format "%Y-%m-%d %H:%M:%S".
 * Parameters: time - The file modification time to format
 * Returns: Formatted date/time string (e.g., "2026-02-01 19:00:00")
 */
std::string FileManagerFrame::FormatFileTime(const std::filesystem::file_time_type& time) const {
    // Convert to system_clock time_point
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        time - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

    // Convert to time_t
    std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

    // Format using strftime
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&tt));
    return std::string(buffer);
}

/**
 * Function: OnPathChanged
 * Description: Event handler for path text control changes. Called when the user
 *              edits the path text control. Validates the new path and navigates
 *              to it if it's a valid directory, otherwise displays an error message.
 * Parameters: event - The command event from the text control
 * Returns: None
 */
void FileManagerFrame::OnPathChanged(wxCommandEvent& event) {
    wxString pathStr = m_pathTextCtrl->GetValue();
    std::filesystem::path newPath(pathStr.ToStdString());

    try {
        if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath)) {
            m_currentPath = std::filesystem::canonical(newPath);
            RefreshFileList();
            m_statusBar->SetStatusText("Directory changed");
        } else {
            m_statusBar->SetStatusText("Invalid directory path");
        }
    } catch (const std::filesystem::filesystem_error& e) {
        m_statusBar->SetStatusText("Error: " + std::string(e.what()));
    }
}

/**
 * Function: OnFileActivated
 * Description: Event handler for double-clicking a file or directory in the list.
 *              If a directory is double-clicked, navigates into it. If a file is
 *              double-clicked, opens it with the system default application.
 * Parameters: event - The list event from the list control
 * Returns: None
 */
void FileManagerFrame::OnFileActivated(wxListEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        return;
    }

    try {
        if (std::filesystem::is_directory(selectedPath)) {
            // Navigate to directory
            m_currentPath = std::filesystem::canonical(selectedPath);
            UpdatePathDisplay();
            RefreshFileList();
            m_statusBar->SetStatusText("Directory changed");
        } else if (std::filesystem::is_regular_file(selectedPath)) {
            // Open file with system default application
            wxString path = selectedPath.string();
            wxLaunchDefaultApplication(path);
            m_statusBar->SetStatusText("Opened: " + selectedPath.filename().string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        wxMessageBox("Error: " + std::string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/**
 * Function: OnFileSelected
 * Description: Event handler for file selection in the list control. Currently
 *              no additional handling is needed beyond the default selection behavior.
 * Parameters: event - The list event from the list control
 * Returns: None
 */
void FileManagerFrame::OnFileSelected(wxListEvent& event) {
    // Could add additional selection handling here if needed
}

/**
 * Function: OnOpen
 * Description: Event handler for Open menu item. Opens the selected file with the
 *              system default application, or navigates into the selected directory.
 *              Displays an error message if no item is selected.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnOpen(wxCommandEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        wxMessageBox("Please select a file or directory to open.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    try {
        if (std::filesystem::is_directory(selectedPath)) {
            m_currentPath = std::filesystem::canonical(selectedPath);
            UpdatePathDisplay();
            RefreshFileList();
            m_statusBar->SetStatusText("Directory changed");
        } else if (std::filesystem::is_regular_file(selectedPath)) {
            wxString path = selectedPath.string();
            wxLaunchDefaultApplication(path);
            m_statusBar->SetStatusText("Opened: " + selectedPath.filename().string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        wxMessageBox("Error opening: " + std::string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/**
 * Function: OnCreateDirectory
 * Description: Event handler for Create Directory menu item. Prompts the user for
 *              a directory name and creates a new directory in the current location.
 *              Validates the name and checks for existing directories with the same name.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnCreateDirectory(wxCommandEvent& event) {
    wxTextEntryDialog dialog(this, "Enter directory name:", "Create Directory", "");
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString dirName = dialog.GetValue();
        if (dirName.IsEmpty()) {
            wxMessageBox("Directory name cannot be empty.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        try {
            std::filesystem::path newDir = m_currentPath / dirName.ToStdString();
            if (std::filesystem::exists(newDir)) {
                wxMessageBox("Directory already exists.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            std::filesystem::create_directory(newDir);
            RefreshFileList();
            m_statusBar->SetStatusText("Directory created: " + dirName);
        } catch (const std::filesystem::filesystem_error& e) {
            wxMessageBox("Error creating directory: " + std::string(e.what()),
                         "Error", wxOK | wxICON_ERROR);
        }
    }
}

/**
 * Function: OnRename
 * Description: Event handler for Rename menu item. Prompts the user for a new name
 *              and renames the selected file or directory. Validates the new name
 *              and checks for conflicts with existing files or directories.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnRename(wxCommandEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        wxMessageBox("Please select a file or directory to rename.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    wxString currentName = selectedPath.filename().string();
    wxTextEntryDialog dialog(this, "Enter new name:", "Rename", currentName);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString newName = dialog.GetValue();
        if (newName.IsEmpty()) {
            wxMessageBox("Name cannot be empty.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        try {
            std::filesystem::path newPath = selectedPath.parent_path() / newName.ToStdString();
            if (std::filesystem::exists(newPath)) {
                wxMessageBox("A file or directory with that name already exists.", "Error",
                             wxOK | wxICON_ERROR);
                return;
            }

            std::filesystem::rename(selectedPath, newPath);
            RefreshFileList();
            m_statusBar->SetStatusText("Renamed: " + currentName + " -> " + newName);
        } catch (const std::filesystem::filesystem_error& e) {
            wxMessageBox("Error renaming: " + std::string(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }
}

/**
 * Function: OnDelete
 * Description: Event handler for Delete menu item. Prompts the user for confirmation
 *              before deleting the selected file or directory. Uses remove_all for
 *              directories and remove for files.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnDelete(wxCommandEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        wxMessageBox("Please select a file or directory to delete.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    wxString name = selectedPath.filename().string();
    int result = wxMessageBox("Are you sure you want to delete \"" + name + "\"?",
                              "Confirm Delete", wxYES_NO | wxICON_QUESTION);

    if (result == wxYES) {
        try {
            if (std::filesystem::is_directory(selectedPath)) {
                std::filesystem::remove_all(selectedPath);
            } else {
                std::filesystem::remove(selectedPath);
            }
            RefreshFileList();
            m_statusBar->SetStatusText("Deleted: " + name);
        } catch (const std::filesystem::filesystem_error& e) {
            wxMessageBox("Error deleting: " + std::string(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }
}

/**
 * Function: OnCopy
 * Description: Event handler for Copy menu item. Marks the selected file or directory
 *              for copying by storing its path in the clipboard. Sets the cut flag to false.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnCopy(wxCommandEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        wxMessageBox("Please select a file or directory to copy.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    m_clipboardPath = selectedPath;
    m_isCutOperation = false;
    m_statusBar->SetStatusText("Copied: " + selectedPath.filename().string());
}

/**
 * Function: OnCut
 * Description: Event handler for Cut menu item. Marks the selected file or directory
 *              for moving by storing its path in the clipboard. Sets the cut flag to true.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnCut(wxCommandEvent& event) {
    std::filesystem::path selectedPath = GetSelectedPath();
    if (selectedPath.empty()) {
        wxMessageBox("Please select a file or directory to cut.", "No Selection",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    m_clipboardPath = selectedPath;
    m_isCutOperation = true;
    m_statusBar->SetStatusText("Cut: " + selectedPath.filename().string());
}

/**
 * Function: OnPaste
 * Description: Event handler for Paste menu item. Completes a copy or cut operation
 *              by copying or moving the marked file/directory into the current directory.
 *              Prompts the user for confirmation if the destination already exists.
 *              Clears the clipboard after the operation completes.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnPaste(wxCommandEvent& event) {
    if (m_clipboardPath.empty()) {
        wxMessageBox("No file or directory in clipboard.", "No Clipboard",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    try {
        if (!std::filesystem::exists(m_clipboardPath)) {
            wxMessageBox("Source file or directory no longer exists.", "Error",
                         wxOK | wxICON_ERROR);
            m_clipboardPath.clear();
            m_statusBar->SetStatusText("Clipboard cleared");
            return;
        }

        std::filesystem::path destPath = m_currentPath / m_clipboardPath.filename();

        // Check if destination already exists
        if (std::filesystem::exists(destPath)) {
            int result = wxMessageBox("File or directory already exists. Overwrite?",
                                      "Overwrite?", wxYES_NO | wxICON_QUESTION);
            if (result != wxYES) {
                return;
            }
            // Remove existing file/directory
            if (std::filesystem::is_directory(destPath)) {
                std::filesystem::remove_all(destPath);
            } else {
                std::filesystem::remove(destPath);
            }
        }

        if (m_isCutOperation) {
            // Move operation
            std::filesystem::rename(m_clipboardPath, destPath);
            m_statusBar->SetStatusText("Moved: " + m_clipboardPath.filename().string());
        } else {
            // Copy operation
            if (std::filesystem::is_directory(m_clipboardPath)) {
                std::filesystem::copy(m_clipboardPath, destPath,
                                      std::filesystem::copy_options::recursive);
            } else {
                std::filesystem::copy_file(m_clipboardPath, destPath);
            }
            m_statusBar->SetStatusText("Copied: " + m_clipboardPath.filename().string());
        }

        m_clipboardPath.clear();
        m_statusBar->SetStatusText(m_statusBar->GetStatusText() + " (Clipboard cleared)");
        RefreshFileList();
    } catch (const std::filesystem::filesystem_error& e) {
        wxMessageBox("Error pasting: " + std::string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/**
 * Function: OnRefresh
 * Description: Event handler for Refresh menu item. Refreshes the file listing to
 *              show the current directory contents, updating the display with any
 *              changes that may have occurred.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnRefresh(wxCommandEvent& event) {
    RefreshFileList();
    m_statusBar->SetStatusText("Display refreshed");
}

/**
 * Function: OnExit
 * Description: Event handler for Exit menu item. Closes the application window,
 *              which triggers the application shutdown process.
 * Parameters: event - The command event from the menu
 * Returns: None
 */
void FileManagerFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

/**
 * Function: ~FileManagerFrame
 * Description: Destructor for FileManagerFrame. Currently no special cleanup needed
 *              as wxWidgets handles resource management for UI components.
 * Parameters: None
 * Returns: None
 */
FileManagerFrame::~FileManagerFrame() {
    // No special cleanup needed - wxWidgets manages UI component lifecycle
}

