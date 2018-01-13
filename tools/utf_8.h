// Copyright (c) 2006 Nemanja Trifunovic

/*
Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/


#ifndef UTF8_FOR_CPP_2675DCD0_9480_4c0c_B92A_CC14C027B731
#define UTF8_FOR_CPP_2675DCD0_9480_4c0c_B92A_CC14C027B731

#include <iterator>
#include <exception>

namespace utf8
{
    // The typedefs for 8-bit, 16-bit and 32-bit unsigned integers
    // You may need to change them to match your system. 
    // These typedefs have the same names as ones from cstdint, or boost/cstdint
    typedef unsigned char   uint8_t;
    typedef unsigned short  uint16_t;
    typedef unsigned int    uint32_t;
    
    // Exceptions that may be thrown from the library functions.
    class invalid_code_point : public std::exception {
        uint32_t cp;
    public:
        invalid_code_point(uint32_t cp) : cp(cp) {}
        const char* what() { return "Invalid code point"; }
        uint32_t code_point() const {return cp;}
    };

    class invalid_utf8 : public std::exception {
        uint8_t u8;
    public:
        invalid_utf8 (uint8_t u) : u8(u) {}
        const char* what() { return "Invalid UTF-8"; }
        uint8_t utf8_octet() const {return u8;}
    };

    class invalid_utf16 : public std::exception {
        uint16_t u16;
    public:
        invalid_utf16 (uint16_t u) : u16(u) {}
        const char* what() { return "Invalid UTF-16"; }
        uint16_t utf16_word() const {return u16;}
    };

    class not_enough_room : public std::exception {
    public:
        const char* what() { return "Not enough space"; }
    };



    
    // Unicode constants
    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    const uint32_t LEAD_SURROGATE_MIN  = 0xd800;
    const uint32_t LEAD_SURROGATE_MAX  = 0xdbff;
    const uint32_t TRAIL_SURROGATE_MIN = 0xdc00;
    const uint32_t TRAIL_SURROGATE_MAX = 0xdfff;
    const uint32_t LEAD_OFFSET         = LEAD_SURROGATE_MIN - (0x10000 >> 10);
    const uint32_t SURROGATE_OFFSET    = 0x10000 - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN;

    // Maximum valid value for a Unicode code point
    const uint32_t CODE_POINT_MAX      = 0x0010ffff;

    // Byte order mark
    const uint8_t bom[] = {0xef, 0xbb, 0xbf}; 

    /// Helper functions - not intended to be directly called by the library users
    template<typename octet_type>
    inline uint8_t mask8(octet_type oc)
    {
        return static_cast<uint8_t>(0xff & oc);
    }
    template<typename u16_type>
    inline uint16_t mask16(u16_type oc)
    {
        return static_cast<uint16_t>(0xffff & oc);
    }
    template<typename octet_type>
    inline bool is_trail(octet_type oc)
    {
        return ((mask8(oc) >> 6) == 0x2);
    }

    template <typename u16>
    inline bool is_surrogate(u16 cp)
    {
        return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    
    
    /// The library API - functions intended to be called by the users
    template <typename octet_iterator>
    octet_iterator find_invalid(octet_iterator start, octet_iterator end)
    {
        octet_iterator result = start;
        while (result != end) {
            if (mask8(*result) > 0xf4)
                break;
            if (mask8(*result) < 0x80) 
                ;
            else if ((mask8(*result) >> 5) == 0x6) {
                uint8_t lead = mask8(*result);
                if (++result == end)
                    return (--result);
                if (!is_trail(*result))
                    return result;
                switch (lead) {
                    case 0xe0:
                        if ((mask8(*result)) < 0xa0)
                            return result; 
                        break;
                    case 0xed:
                        if ((mask8(*result)) > 0x9F)
                            return result;
                        break;
                    case 0xf0: 
                        if ((mask8(*result)) < 0x90)
                            return result;
                        break;
                }
            }
            else if ((mask8(*result) >> 4) == 0xe) { 
                if (++result == end) 
                    break;                
                if (!is_trail(*result))
                    break;
                if (++result == end)
                    break;
                if (!is_trail(*result))
                    break;
            } 

            else if ((mask8(*result) >> 3) == 0x1e) {
                if (++result == end)
                    break;
                if (!is_trail(*result))
                    break;
                if (++result == end) 
                    break;
                if (!is_trail(*result))
                    break;
                if (++result == end)
                    break;
                if (!is_trail(*result))
                    break;
            }
            else
                break;
            ++result;
        }
        return result;
    }

    template <typename octet_iterator>
    bool is_valid(octet_iterator start, octet_iterator end)
    {
        return (find_invalid(start, end) == end);
    }

    template <typename octet_iterator>
    bool is_bom (octet_iterator it)
    {
        return (
            (mask8(*it++)) == bom[0] &&
            (mask8(*it++)) == bom[1] &&
            (mask8(*it))   == bom[2]
           );
    }
    template <typename octet_iterator>
    octet_iterator append(uint32_t cp, octet_iterator result)
    {
        if (cp < 0x80)                        // one octet
            *(result++) = static_cast<uint8_t>(cp);  
        else if (cp < 0x800) {                // two octets
            if (is_surrogate(cp)) 
                throw invalid_code_point(cp);

            *(result++) = static_cast<uint8_t>((cp >> 6)   | 0xc0);
            *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
        }
        else if (cp < 0x10000) {              // three octets
            *(result++) = static_cast<uint8_t>((cp >> 12)  | 0xe0);
            *(result++) = static_cast<uint8_t>((cp >> 6)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
        }
        else if (cp <= CODE_POINT_MAX) {      // four octets
            *(result++) = static_cast<uint8_t>((cp >> 18)  | 0xf0);
            *(result++) = static_cast<uint8_t>((cp >> 12)  | 0x80);
            *(result++) = static_cast<uint8_t>((cp >> 6)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
        }
        else
            throw invalid_code_point(cp);

        return result;
    }

    template <typename octet_iterator>
    uint32_t next(octet_iterator& it, octet_iterator end)
    {
        uint32_t cp = mask8(*it);
        if (cp < 0x80) 
            ;
        else if ((mask8(*it) >> 5) == 0x6) {
            if (++it != end) { 
                if (is_trail(*it)) { 
                    cp = ((cp << 6) & 0x7ff) + ((*it) & 0x3f);
                }
                else
                    throw invalid_utf8 (*it);
            }
            else
                throw not_enough_room();
        }
        else if ((mask8(*it) >> 4) == 0xe) { 
            if (++it != end) { 
                if (is_trail(*it)) {
                    cp = ((cp << 12) & 0xffff) + ((mask8(*it) << 6) & 0xfff);
                    if (++it != end) {
                        if (is_trail(*it)) {
                            cp += (*it) & 0x3f;
                        }
                        else
                            throw invalid_utf8 (*it);
                    }
                    else
                        throw not_enough_room();
                }
                else
                    throw invalid_utf8 (*it);
            }
            else
                throw not_enough_room();
        }    
        else if ((mask8(*it) >> 3) == 0x1e) {
            if (++it != end) {
                if (is_trail(*it)) {
                    cp = ((cp << 18) & 0x1fffff) + (mask8(*it) << 12) & 0x3ffff;                
                    if (++it != end) {
                        if (is_trail(*it)) {
                            cp += (mask8(*it) << 6) & 0xfff;
                            if (++it != end) {
                                if (is_trail(*it)) {
                                    cp += (*it) & 0x3f; 
                                }
                                else
                                    throw invalid_utf8 (*it);
                            }
                            else
                                throw not_enough_room();
                        }
                        else
                            throw invalid_utf8 (*it);
                    }
                    else
                        throw not_enough_room();
                }
                else
                    throw invalid_utf8 (*it);
            }
            else
                throw not_enough_room();
        }
        ++it;
        if (cp > CODE_POINT_MAX || is_surrogate(cp))
            throw invalid_code_point(cp);

        return cp;        
    }


    template <typename octet_iterator>
    uint32_t previous(octet_iterator& it, octet_iterator pass_start)
    {
        octet_iterator end = it;
        while (is_trail(*(--it))) 
            if (it == pass_start)
                throw invalid_utf8(*it); // error - no lead byte in the sequence
        octet_iterator temp = it;
        return next(temp, end);
    }

    template <typename octet_iterator, typename distance_type>
    void advance (octet_iterator& it, distance_type n, octet_iterator end)
    {
        for (distance_type i = 0; i < n; ++i)
            next(it, end);
    }

    template <typename octet_iterator>
    typename std::iterator_traits<octet_iterator>::difference_type
    distance (octet_iterator first, octet_iterator last)
    {
        typename std::iterator_traits<octet_iterator>::difference_type dist;
        for (dist = 0; first < last; ++dist) 
            next(first, last);
        return dist;
    }

    template <typename u16bit_iterator, typename octet_iterator>
    void utf16to8 (u16bit_iterator start, u16bit_iterator end, octet_iterator result)
    {       
        while (start != end) {
            uint32_t cp = mask16(*start++);
            // Take care of surrogate pairs first
            if (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX) {
                if (start != end) {
                    uint32_t trail_surrogate = mask16(*start++);
                    if (trail_surrogate >= TRAIL_SURROGATE_MIN && trail_surrogate <= TRAIL_SURROGATE_MAX)
                        cp = (cp << 10) + trail_surrogate + SURROGATE_OFFSET;                    
                    else 
                        throw invalid_utf16(static_cast<uint16_t>(trail_surrogate));
                }
                else 
                    throw invalid_utf16(static_cast<uint16_t>(*start));
            
            }
            *result = append(cp, result);
        }         
    }

    template <typename u16bit_iterator, typename octet_iterator>
    void utf8to16 (octet_iterator start, octet_iterator end, u16bit_iterator result)
    {
        while (start != end) {
            uint32_t cp = next(start, end);
            if (cp > 0xffff) { //make a surrogate pair
                *result++ = static_cast<uint16_t>((cp >> 10)   + LEAD_OFFSET);
                *result++ = static_cast<uint16_t>((cp & 0x3ff) + TRAIL_SURROGATE_MIN);
            }
            else
                *result++ = static_cast<uint16_t>(cp);
        }
    }

    template <typename octet_iterator, typename u32bit_iterator>
    void utf32to8 (u32bit_iterator start, u32bit_iterator end, octet_iterator result)
    {
        while (start != end)
            *result = append(*(start++), result);
    }

    template <typename octet_iterator, typename u32bit_iterator>
    void utf8to32 (octet_iterator start, octet_iterator end, u32bit_iterator result)
    {
        while (start < end)
            (*result++) = next(start, end);
    }

    namespace unchecked 
    {
        template <typename octet_iterator>
        octet_iterator append(uint32_t cp, octet_iterator result)
        {
            if (cp < 0x80)                        // one octet
                *(result++) = static_cast<uint8_t>(cp);  
            else if (cp < 0x800) {                // two octets
                *(result++) = static_cast<uint8_t>((cp >> 6)   | 0xc0);
                *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
            }
            else if (cp < 0x10000) {              // three octets
                *(result++) = static_cast<uint8_t>((cp >> 12)  | 0xe0);
                *(result++) = static_cast<uint8_t>((cp >> 6)   | 0x80);
                *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
            }
            else {                                // four octets
                *(result++) = static_cast<uint8_t>((cp >> 18)  | 0xf0);
                *(result++) = static_cast<uint8_t>((cp >> 12)  | 0x80);
                *(result++) = static_cast<uint8_t>((cp >> 6)   | 0x80);
                *(result++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
            }
            return result;
        }
        template <typename octet_iterator>
        uint32_t next(octet_iterator& it)
        {
            uint32_t cp = mask8(*it);
            if (cp < 0x80) 
                ;
            else if ((mask8(*it) >> 5) == 0x6) {
                it++;
                cp = ((cp << 6) & 0x7ff) + ((*it) & 0x3f);
            }
            else if ((mask8(*it) >> 4) == 0xe) { 
                ++it; 
                cp = ((cp << 12) & 0xffff) + ((mask8(*it) << 6) & 0xfff);
                ++it;
                cp += (*it) & 0x3f;
            }
            else if (((*it) >> 3) == 0x1e) {
                ++it;
                cp = ((cp << 18) & 0x1fffff) + (mask8(*it) << 12) & 0x3ffff;                
                ++it;
                cp += (mask8(*it) << 6) & 0xfff;
                ++it;
                cp += (*it) & 0x3f; 
            }
            ++it;
            return cp;        
        }

        template <typename octet_iterator>
        uint32_t previous(octet_iterator& it)
        {
            while (is_trail(*(--it))) ;
            octet_iterator temp = it;
            return next(temp);
        }

        template <typename octet_iterator, typename distance_type>
        void advance (octet_iterator& it, distance_type n)
        {
            for (distance_type i = 0; i < n; ++i)
                next(it);
        }

        template <typename octet_iterator>
        uint32_t get(octet_iterator it)
        {
            return next(it);
        }

        template <typename octet_iterator>
        typename std::iterator_traits<octet_iterator>::difference_type
        distance (octet_iterator first, octet_iterator last)
        {
            typename std::iterator_traits<octet_iterator>::difference_type dist;
            for (dist = 0; first < last; ++dist) 
                next(first);
            return dist;
        }

        template <typename u16bit_iterator, typename octet_iterator>
        void utf16to8 (u16bit_iterator start, u16bit_iterator end, octet_iterator result)
        {       
            while (start != end) {
                uint32_t cp = mask16(*start++);
            // Take care of surrogate pairs first
                if (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX) {
                    uint32_t trail_surrogate = mask16(*start++);
                    cp = (cp << 10) + trail_surrogate + SURROGATE_OFFSET;
                }
                *result = append(cp, result);
            }         
        }

        template <typename u16bit_iterator, typename octet_iterator>
        void utf8to16 (octet_iterator start, octet_iterator end, u16bit_iterator result)
        {
            while (start != end) {
                uint32_t cp = next(start);
                if (cp > 0xffff) { //make a surrogate pair
                    *result++ = static_cast<uint16_t>((cp >> 10)   + LEAD_OFFSET);
                    *result++ = static_cast<uint16_t>((cp & 0x3ff) + TRAIL_SURROGATE_MIN);
                }
                else
                    *result++ = static_cast<uint16_t>(cp);
            }
        }

        template <typename octet_iterator, typename u32bit_iterator>
        void utf32to8 (u32bit_iterator start, u32bit_iterator end, octet_iterator result)
        {
            while (start != end)
                *result = append(*(start++), result);
        }

        template <typename octet_iterator, typename u32bit_iterator>
        void utf8to32 (octet_iterator start, octet_iterator end, u32bit_iterator result)
        {
            while (start < end)
                (*result++) = next(start);
        }

    } // namespace utf8::unchecked
} // namespace utf8 

#endif // header guard
