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

#ifndef QRTAPE_UTIL_STRING_H_
#define QRTAPE_UTIL_STRING_H_

#include <string>
#include <vector>

namespace qrt {

// Formats the supplied arguments into a string and returns it.
std::string StringFormat(const char* format, ...);

}  // namespace qrt

#endif  // QRTAPE_UTIL_STRING_H_
