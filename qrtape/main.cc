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

#include <tclap/CmdLine.h>
#include <cstring>

#include "qrtape/util/crc16.h"
#include "qrtape/util/file.h"
#include "qrtape/util/log.h"
#include "qrtape/util/string.h"

// A description of the program.
constexpr char kDescription[] = "Encode/decode files into QR code fragments.";

// The version of the program.
constexpr char kVersion[] = "0.0.1";

// Encodes the supplied value as a little-endian value into the string.
void EncodeInt(uint16_t value, std::string* string) {
  string->push_back(value & 0xff);
  string->push_back((value >> 8) & 0xff);
}

// Decodes a value from the string as little-endian.
uint16_t DecodeInt(const std::string& string, size_t offset) {
  return static_cast<uint16_t>(string[offset] & 0xff)
      | static_cast<uint16_t>(string[offset + 1]) << 8;
}

// Encodes file chunks from the given file.
void Encode(const std::string& input_filename, size_t max_chunk_size,
    const std::string& output_prefix) {
  std::string file;
  if (!qrt::ReadFile(input_filename, &file)) {
    LOGFATAL("Failed to read file '%s': %s (%d)", input_filename.c_str(),
        strerror(errno), errno);
  }

  max_chunk_size -= 6;  // Leave space for size/checksums.

  LOGI("Generating files from '%s' in %zu byte chunks",
      input_filename.c_str(), max_chunk_size);
  size_t chunk_index = 0;
  for (size_t i = 0; i < file.size(); i += max_chunk_size) {
    uint16_t chunk_size = std::min(max_chunk_size, file.size() - i);
    auto filename = qrt::StringFormat("%s%zu.bin",
        output_prefix.c_str(), chunk_index);
    LOGI("Generating chunk %zu: offset %zu, size %zu, filename '%s'",
        chunk_index, i, chunk_size, filename.c_str());

    std::string chunk;
    std::string file_chunk = file.substr(i, max_chunk_size);
    if (file_chunk.size() < max_chunk_size) {
      file_chunk += std::string(max_chunk_size - file_chunk.size(), '\0');
    }

    EncodeInt(chunk_index, &chunk);
    EncodeInt(chunk_size, &chunk);
    chunk += file_chunk;
    EncodeInt(qrt::GenerateCrc16(
        reinterpret_cast<const uint8_t*>(chunk.c_str()),
        chunk.size()), &chunk);

    if (!qrt::WriteFile(filename, chunk)) {
      LOGFATAL("Failed to write chunk '%s': %s (%d)", filename.c_str(),
          strerror(errno), errno);
    }

    chunk_index++;
  }

  LOGI("Finished generating chunks");
}

// Decodes file chunks received on stdin and writes output to stdout.
void Decode(size_t chunk_size, bool allow_skip) {
  if (freopen(nullptr, "rb", stdin) == nullptr) {
    LOGFATAL("Failed to reopen stdin in binary mode: %s (%d)",
        strerror(errno), errno);
  }

  if (freopen(nullptr, "wb", stdout) == nullptr) {
    LOGFATAL("Failed to reopen stdout in binary mode: %s (%d)",
        strerror(errno), errno);
  }

  LOGI("Reading chunks from stdin");

  uint16_t chunk_index = 0;
  while (1) {
    std::string chunk;
    for (size_t i = 0; i < chunk_size; i++) {
      int c = getchar();
      if (c < 0) {
        return;
      }

      chunk += static_cast<char>(c);
    }

    uint16_t read_crc = DecodeInt(chunk, chunk.size() - 2);
    uint16_t computed_crc = qrt::GenerateCrc16(
        reinterpret_cast<const uint8_t*>(chunk.c_str()), chunk.size() - 2);
    if (read_crc != computed_crc) {
      LOGE("Chunk %zu CRC mismatch read_crc=%04x != computed_crc=%04x",
          chunk_index, read_crc, computed_crc);
      continue;
    }

    uint16_t read_chunk_index = DecodeInt(chunk, 0);
    if (allow_skip && read_chunk_index < chunk_index
        || !allow_skip && read_chunk_index != chunk_index) {
      LOGE("Chunk %zu expected, found %zu", chunk_index, read_chunk_index);
    } else {
      uint16_t read_chunk_index = DecodeInt(chunk, 0);
      uint16_t read_chunk_size = DecodeInt(chunk, 2);
      for (size_t i = 0; i < read_chunk_size; i++) {
        if (putchar(chunk[i + 4]) < 0) {
          return;
        }
      }

      LOGI("Processed chunk %zu with size %zu successfully",
          read_chunk_index, read_chunk_size);
      chunk_index = read_chunk_index;
    }

    // Discard a byte.
    int c = getchar();
    if (c < 0) {
      return;
    }
  }
}

int main(int argc, char** argv) {
  TCLAP::CmdLine cmd(kDescription, ' ', kVersion);
  TCLAP::SwitchArg encode_arg("e", "encode",
      "Set to true to encode a given file.");
  TCLAP::SwitchArg decode_arg("d", "decode",
      "Set to true to decode a given file.");
  TCLAP::ValueArg<size_t> chunk_size_arg("s", "size",
      "The maximum size of a chunk.", false, 1024, "bytes", cmd);
  TCLAP::ValueArg<std::string> input_arg("i", "input",
      "The input file to encode when used in encode mode.",
      false, "", "file", cmd);
  TCLAP::ValueArg<std::string> output_prefix_arg("p", "prefix",
      "The output file prefix.", false, "", "file prefix", cmd);
  TCLAP::SwitchArg allow_skip_arg("", "allow-skip",
      "Allow skipping a QR code in the input.", cmd);
  cmd.xorAdd(encode_arg, decode_arg);
  cmd.parse(argc, argv);

  if (encode_arg.getValue()) {
    Encode(input_arg.getValue(), chunk_size_arg.getValue(),
        output_prefix_arg.getValue());
  } else if (decode_arg.getValue()) {
    Decode(chunk_size_arg.getValue(), allow_skip_arg.getValue());
  }

  return 0;
}
