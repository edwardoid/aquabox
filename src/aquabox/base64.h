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

#ifndef BASE64_H
#define BASE64_H

namespace aquabox
{
    namespace utils
    {
        static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        static inline unsigned char indexOf(unsigned char c) {
            for(int i = 0; i < 64; ++i) {
                if (base64_chars[i] == c) {
                    return i;
                }
            }

            // Should never happen
            return 0;
        }


        static inline bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

        template<typename StringImpl>
        StringImpl base64_encode(unsigned char const* bytes_to_encode, uint16_t in_len) {
            StringImpl ret;
            int i = 0;
            int j = 0;
            unsigned char char_array_3[3];
            unsigned char char_array_4[4];
        
            while (in_len--) {
                char_array_3[i++] = *(bytes_to_encode++);
                if (i == 3) {
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;
        
                    for(i = 0; (i <4) ; i++)
                        ret += base64_chars[char_array_4[i]];
                    i = 0;
                }
            }
        
            if (i)
            {
                for(j = i; j < 3; j++)
                    char_array_3[j] = '\0';
        
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
        
                for (j = 0; (j < i + 1); j++)
                    ret += base64_chars[char_array_4[j]];
        
                while((i++ < 3))
                    ret += '=';
        
            }
        
            return ret;
        
        }
        
        template<class StringImpl, class ContainerT>
        ContainerT base64_decode(StringImpl const& encoded_string, int in_len) {
            int i = 0;
            int j = 0;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            ContainerT ret;
        
            while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
                char_array_4[i++] = encoded_string[in_]; in_++;
                if (i ==4) {
                    for (i = 0; i <4; i++)
                        char_array_4[i] = indexOf(char_array_4[i]);
        
                    ret.push_back(static_cast<typename ContainerT::value_type>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4)));
                    ret.push_back(static_cast<typename ContainerT::value_type>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2)));
                    ret.push_back(static_cast<typename ContainerT::value_type>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]));
        
                    for (i = 0; (i < 3); i++)
                        ret.push_back(char_array_3[i]);
                    i = 0;
                }
            }
        
            if (i) {
                for (j = i; j <4; j++)
                    char_array_4[j] = 0;
        
                for (j = 0; j <4; j++) {
                    char_array_4[j] = indexOf(char_array_4[j]);
                }
                ret.push_back(static_cast<typename ContainerT::value_type>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4)));
                ret.push_back(static_cast<typename ContainerT::value_type>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2)));
                ret.push_back(static_cast<typename ContainerT::value_type>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]));
            }
        
            return ret;
        }
    }
}

#endif // BASE64_H