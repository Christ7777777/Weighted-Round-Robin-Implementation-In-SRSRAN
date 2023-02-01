/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/support/error_handling.h"
#include <fstream>
#include <vector>

namespace srsgnb {

/// \c file_vector open modes.
enum class openmode {
  /// Opens the file in read-only mode.
  read_only,
  /// Opens the file in write-only mode.
  write_only,
  /// \brief Opens the file in read-write mode.
  ///
  /// Any previous content of the file will be lost.
  read_write
};

template <typename T>
/// \brief Simple binary file input-output interface.
///
/// An object of this file allows reading and writing values of type \c T from and to a binary file.
class file_vector
{
private:
  std::string file_name;
  openmode    op;

public:
  /// \brief Constructor.
  ///
  /// Sets the name and the mode (default is \e read-only) of the binary file. No operations are performed on the file.
  file_vector(std::string filename, openmode op_ = openmode::read_only) : file_name(std::move(filename)), op(op_) {}

  /// Returns the file name.
  const std::string& get_file_name() const { return file_name; }

  /// \brief Reads the file.
  ///
  /// The contents of the file are dumped to a \c std::vector<T>. The file is closed when the operation is completed.
  /// \warning An exception arises if the file cannot be opened or if the write-only open mode was specified at
  /// creation.
  std::vector<T> read() const
  {
    report_fatal_error_if_not(op != openmode::write_only, "The file_vector was created with write-only permissions.");
    std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);

    report_fatal_error_if_not(binary_file.is_open(), "Error opening file '{}'. {}.", file_name, strerror(errno));
    std::vector<T> read_data;

    T read_value;
    while (binary_file.read(reinterpret_cast<char*>(&read_value), sizeof(T))) {
      read_data.push_back(read_value);
    }

    return read_data;
  }

  /// \brief Writes data to the file.
  ///
  /// The sequence of \c T values viewed by \c write_data are written to the file.
  /// If the specified file does not exist, it is created. If it exists, previous contents will be erased.
  /// The file is closed when the operation is completed.
  ///
  /// \warning An exception arises if the file cannot be opened or if the read-only open mode was specified at creation.
  void write(span<const T> write_data) const
  {
    report_fatal_error_if_not(op != openmode::read_only, "The file_vector was created with read-only permissions.");
    std::ofstream binary_file(file_name, std::ios::out | std::ios::binary);

    report_fatal_error_if_not(binary_file.is_open(), "Error opening file '{}'. {}.", file_name, strerror(errno));

    binary_file.write(reinterpret_cast<const char*>(write_data.begin()), write_data.size() * sizeof(T));
  }
};

} // namespace srsgnb
