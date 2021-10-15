/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CRYPTO_H
#define CRYPTO_H
#include <string>

namespace aquabox {
class SHA512 {
protected:
    typedef unsigned char uint8;
    typedef unsigned int uint32;
    typedef unsigned long long uint64;

    const static uint64 sha512_k[];
    static const unsigned int SHA384_512_BLOCK_SIZE = (1024 / 8);

public:
    void init();
    void update(const unsigned char* message, unsigned int len);
    void final(unsigned char* digest);
    static const unsigned int DIGEST_SIZE = (512 / 8);

    static std::string hash(std::string input);

protected:
    void transform(const unsigned char* message, unsigned int block_nb);
    unsigned int m_tot_len;
    unsigned int m_len;
    unsigned char m_block[2 * SHA384_512_BLOCK_SIZE];
    uint64 m_h[8];
};
}

#endif