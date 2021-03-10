/*
 * Copyright 2021 Andrew Rossignol andrew.rossignol@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "qrtape/util/file.h"

#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>

#include "qrtape/util/log.h"

namespace qrt {

bool ReadFile(const std::string& filename, std::string* contents) {
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (!in) {
    return false;
  }

  in.seekg(0, std::ios::end);
  contents->reserve(in.tellg());
  in.seekg(0, std::ios::beg);
  contents->assign((std::istreambuf_iterator<char>(in)),
                   std::istreambuf_iterator<char>());
  return true;
}

bool WriteFile(const std::string& filename,
    const std::string& contents, int mode, bool fail_if_exists) {
  return WriteFile(
      filename, reinterpret_cast<const uint8_t*>(contents.data()),
      contents.size(), mode, fail_if_exists);
}

bool WriteFile(const std::string& filename,
    const uint8_t* buffer, size_t size, int mode, bool fail_if_exists) {
  int flags = O_WRONLY | O_CREAT;
  if (fail_if_exists) {
    flags |= O_EXCL;
  } else {
    flags |= O_TRUNC;
  }

  int fd = open(filename.c_str(), flags, mode);
  if (fd < 0) {
    return false;
  }

  bool success = true;
  size_t total_bytes_written = 0;
  while (success && total_bytes_written < size) {
    ssize_t bytes_written = write(fd, &buffer[total_bytes_written],
        size - total_bytes_written);
    if (bytes_written < 0) {
      success = false;
      LOGE("failed to write file: %d (%s)", errno, strerror(errno));
    } else {
      total_bytes_written += bytes_written;
    }
  }

  close(fd);
  return success;
}

}  // namespace qrt
