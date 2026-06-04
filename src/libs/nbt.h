#pragma once

/**
 * Self implementation of NBT specs
 */

enum NBT_Tag {
    TAG_End = 0x00,
    TAG_Byte = 0x01,
    TAG_Short = 0x02,
    TAG_Int = 0x03,
    TAG_Long = 0x04,
    TAG_Float = 0x05,
    TAG_Double = 0x06,
    TAG_Byte_Array = 0x07,
    TAG_String = 0x08,
    TAG_List = 0x09,
    TAG_Compound = 0x0A,
    TAG_Int_Array = 0x0B,
    TAG_Long_Array = 0x0C
};

/**
 * @note nbt tags are formed like the following:
 *                8b - tag as specified before
 *               16b - length of the name
 *  8b * name_length - name of the tag in UTF-8
 *      payload_size - payload
 * !EXCEPT FOR TAG_End, which does not have trailing data after the tag itself
 * 
 * @note sizes of lists and arrays are always Uint32
 *
 * @note lists contain 1byte for payload type, then size and then the values
 *
 */