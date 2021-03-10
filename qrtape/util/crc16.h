/*
 * Copyright 2021 Andrew Rossignol andrew.rossignol@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QRTAPE_UTIL_CRC16_H_
#define QRTAPE_UTIL_CRC16_H_

#include <cstddef>
#include <cstdint>

namespace qrt {

//! The seed value for CRC generation.
constexpr uint16_t kInitialCrc = 0xffff;

/**
 * Returns a CRC16 of the provided buffer.
 *
 * @param buffer the buffer to generate a CRC16 for.
 * @param length the length of the buffer.
 * @return the generated crc16 value.
 */
uint16_t GenerateCrc16(const uint8_t *buffer, size_t length);

/**
 * Performs one step of a CRC16. This is used for incremental CRC16 generation.
 *
 * @param byte the byte to add to the current CRC.
 * @return the next CRC value.
 */
uint16_t GenerateIncrementalCrc16(uint16_t current_crc, uint8_t byte);

}  // namespace qrt

#endif  // QRTAPE_UTIL_CRC16_H_
