
/**
 * MIT License
 * 
 * Copyright (c) 2024 Harley Coughlin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GETINPUTCPP_HPP_
#define GETINPUTCPP_HPP_

#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <cassert>
#include <type_traits>

constexpr auto MAX_STREAM_SIZE = std::numeric_limits<std::streamsize>::max();

/**
 * Clears bad input state flags then removes everything from the buffer until
 * '\n' is reached. Recommended for use after calls to std::cin if you're 
 * planning to use get_input or getline.
 */
inline void clear_input(std::istream &in, char delim = '\n') {
    in.clear();
    in.ignore(MAX_STREAM_SIZE, delim);
}

/**
 * Reads input of a specified type from the given input stream.
 * Performs simple validation that ensures user input is of the given type.
 * Clears the input buffer after a successful read.
 * 
 * @tparam T The type of input to read (not string or bool)
 * @param in The input stream to read from
 * @param prompt The prompt to display before reading input
 * @return The value read from the input stream
 */
template<typename T>
T get_input(std::istream &in, const std::string &prompt) {
    static_assert(!std::is_same<T, bool>::value,
        "get_input can't be used with booleans.");
    static_assert(!std::is_same<T, std::string>::value,
        "It is recommended to use getline() to read strings from input");
    bool valid = false;
    std::string input;
    T value;
    while (!valid) {
        std::cout << prompt;
        std::getline(in, input);
        std::istringstream iss(input);
        if (iss >> value) {
            std::string remainder;
            std::getline(iss >> std::ws, remainder);
            if (!remainder.empty()) {
                std::cout << "Warning: Additional input not accepted.";
                continue;
            }
            valid = true;
        } else {
            clear_input(in);
            std::cout << "Invalid input. Please try again.\n";
        }
    }
    return value;
}

#endif  // GETINPUTCPP_HPP_
