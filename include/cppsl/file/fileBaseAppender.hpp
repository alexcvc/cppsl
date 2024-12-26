//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   The given code is a C++ class called  `FileBaseAppender` .
 * It is a file appender used for logging purposes. Here is a brief explanation
 * of its members and functionalities:
 *   - Constructor    : It initializes the  `FileBaseAppender`  object with a shared
 *                      pointer to a log, a file path, an append flag, and a file mode.
 *   - Destructor     : It is responsible for cleaning up any resources used by
 *                      the  `FileBaseAppender` .
 *   - getFilePath()  : This function returns the file path of the log file.
 *   - reopenFile()   : This method reopens the log file. It can be useful when
 *                      log files are rotated externally.
 *   - closeFile()    : This function closes the log file.
 *   - setAppend()    : This method sets the append vs truncate flag, indicating
 *                      whether to append to an existing file or truncate it.
 *   - getAppend()    : It returns the value of the append flag.
 *   - setMode()      : This function sets the file open mode.
 *   - getMode()      : It returns the file open mode.
 *   - writeMessage() : This protected method is responsible for writing a log
 *                      message to the file.
 *
 *   Overall, the  `FileBaseAppender`  class provides functionality for logging messages
 *   to a file, managing the file open mode, and reopening or closing the log file.
 *
 * \author  Alexander Sacharov
 * \date    2022-01-06
 * \ingroup
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <filesystem>
#include <fstream>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::file {

/// @brief File Appender
class FileBaseAppender {
 public:
  /** The  `FileBaseAppender`  constructor takes the following parameters:
   * @param  `filePath` : The path to the file to which the  `FileBaseAppender` will log.
   * @param  `append` : A boolean flag indicating whether to append to an existing
   * file ( `true` ) or truncate it ( `false` ). It is optional and defaults to `true` .
   * @param  `mode` : The file open mode, specified using  `std::ios_base::openmode`.
   * It is optional and defaults to  `std::ios_base::in | std::ios_base::out` , which
   * means the file will be opened for both input and output operations.
   *
   * Note: The code you provided is incomplete, so it's possible that there might
   * be more details or functionality related to the  `FileBaseAppender` class
   * that are not included here.
   */
  FileBaseAppender(const std::filesystem::path& filePath, bool append = true,
                   std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

  virtual ~FileBaseAppender() = default;

  /// get file path
  [[nodiscard]] const std::filesystem::path& getFilePath() const { return m_filePath; }

  /// \brief returns a boolean value indicating whether a file stream ( m_fs ) is open or not.
  /// \return true if opened, otherwise - false
  [[nodiscard]] bool isOpen() const { return m_fs.is_open(); }

  /// Reopens the logfile.
  /// This can be useful for logfiles that are rotated externally,
  /// e.g. by logrotate. This method is a NOOP for FileAppenders that
  /// have been constructed with a file descriptor.
  /// \return true if the reopen succeeded, otherwise - false
  [[nodiscard]] virtual bool reopenFile();

  /// Closes the logfile.
  void closeFile();

  /// Sets the append vs truncate flag.
  /// NB. currently the base_file_appender opens the logfile in the constructor.
  /// Therefore this method is too late to influence the first file opening.
  /// \param append false to truncate, true to append
  virtual void setAppend(bool append);

  /// Gets the value of the 'append' option.
  /// \return true if successfully, otherwise - false
  [[nodiscard]] virtual bool getAppend() const;

  /// Sets the file open mode.
  /// \param mode file mode.
  virtual void setMode(std::ios_base::openmode mode);

  /// Gets the file open mode.
  [[nodiscard]] virtual std::ios_base::openmode getMode() const;

  /// virtual member function takes a string_view ( message ) as
  /// a parameter and returns a boolean value.
  /// The purpose of this function is to write a message to a file.
  /// \param message message
  /// \return true if successfully, otherwise - false
  [[nodiscard]] virtual bool writeMessage(std::string_view message);

 protected:
  std::filesystem::path m_filePath;                                         ///< log file path
  std::fstream m_fs;                                                        ///< file stream
  std::ios_base::openmode m_mode{std::ios_base::in | std::ios_base::out};   ///< mode
};

}   // end namespace cppsl::file
