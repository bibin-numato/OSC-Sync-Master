/**
 * @file OscMessage.c
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * messages.
 * @see http://opensoundcontrol.org/spec-1_0
 */

//------------------------------------------------------------------------------
// Includes

#include "OscMessage.h"

//------------------------------------------------------------------------------
// Function prototypes

static int TerminateOscString(char* const oscString, size_t * const oscStringSize, const size_t maxOscStringSize);

//------------------------------------------------------------------------------
// Functions - Message construction

/**
 * @brief Initialises an OSC message structure.
 *
 * An OSC message structure must be initialised before use.  The
 * oscAddressPattern argument must be a null terminated string of zero of more
 * characters.  A message may be initialised without an address pattern by
 * parsing an oscAddressPattern value of "".  This may be of use if the address
 * pattern is undetermined at the time of initialisation.  In which case, the
 * address pattern may be set later using OscMessageSetAddressPattern.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "/example/address/pattern");
 * @endcode
 *
 * @param oscMessage Address of the OSC message to be initialised.
 * @param oscAddressPattern OSC address pattern as null terminated string.
 * @return 0 if successful.
 */
int OscMessageInitialise(OscMessage * const oscMessage, const char* oscAddressPattern) {
    oscMessage->oscAddressPattern[0] = '\0'; // null terminate string
    oscMessage->oscTypeTagString[0] = ',';
    oscMessage->oscTypeTagString[1] = '\0'; // null terminate string
    oscMessage->oscAddressPatternLength = 0;
    oscMessage->oscTypeTagStringLength = 1; // includes comma
    oscMessage->argumentsSize = 0;
    oscMessage->oscTypeTagStringIndex = 1; // skip comma
    oscMessage->argumentsIndex = 0;
    if (*oscAddressPattern != '\0') {
        return OscMessageSetAddressPattern(oscMessage, oscAddressPattern);
    }
    return 0;
}

/**
 * @brief Sets the OSC address pattern of an OSC message structure.
 *
 * The oscAddressPattern argument must be a null terminated string of zero of
 * more characters.  The existing OSC address pattern will be overwritten.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "");
 * OscMessageSetAddressPattern(&oscMessage, "/example/address/pattern");
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param oscAddressPattern OSC address pattern as null terminated string.
 * @return 0 if successful.
 */
int OscMessageSetAddressPattern(OscMessage * const oscMessage, const char* oscAddressPattern) {
    oscMessage->oscAddressPatternLength = 0;
    return OscMessageAppendAddressPattern(oscMessage, oscAddressPattern);
}

/**
 * @brief Appends OSC address pattern parts to the OSC address pattern of an
 * OSC message.
 *
 * The appendedParts argument must be a null terminated string.
 *
 * Example use:
 * @code
 * OscMessage oscMessage;
 * OscMessageInitialise(&oscMessage, "");
 * OscMessageAppendAddressPattern(&oscMessage, "/example");
 * OscMessageAppendAddressPattern(&oscMessage, "/address");
 * OscMessageAppendAddressPattern(&oscMessage, "/pattern");
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param appendedParts Address of the OSC pattern parts to be appended.
 * @return 0 if successful.
 */
int OscMessageAppendAddressPattern(OscMessage * const oscMessage, const char* appendedParts) {
    if (*appendedParts != (char) OscContentsTypeMessage) {
        return 1; // error: address must start with '/'
    }
    while (*appendedParts != '\0') {
        if (oscMessage->oscAddressPatternLength >= MAX_OSC_ADDRESS_PATTERN_LENGTH) {
            return 1; // error: address pattern too long
        }
        oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength++] = *appendedParts++;
    }
    oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a 32-bit integer argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInt32(&oscMessage, 123);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param int32 32-bit integer to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddInt32(OscMessage * const oscMessage, const int32_t int32) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInt32;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.int32 = int32;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds a 32-bit float argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddFloat32(&oscMessage, 3.14f);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param float32 32-bit float to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddFloat32(OscMessage * const oscMessage, const float float32) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagFloat32;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.float32 = float32;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds a string argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddString(&oscMessage, "Hello World!");
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param string String to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddString(OscMessage * const oscMessage, const char * string) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    size_t argumentsSize = oscMessage->argumentsSize; // local copy in case function returns error
    while (*string != '\0') {
        if (argumentsSize >= MAX_ARGUMENTS_SIZE) {
            return 1; // error: message full
        }
        oscMessage->arguments[argumentsSize++] = *string++;
    }
    if (TerminateOscString(oscMessage->arguments, &argumentsSize, MAX_ARGUMENTS_SIZE)) {
        return 1; // error: message full
    }
    oscMessage->argumentsSize = argumentsSize;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagString;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a blob (byte array) argument to an OSC message.
 *
 * Example use:
 * @code
 * const char source[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };
 * OscMessageAddBlob(&oscMessage, source, sizeof(source));
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param source Address of the char array to be added as argument.
 * @param sourceSize Size (number of bytes) of the char array to be added as
 * argument.
 * @return 0 if successful.
 */
int OscMessageAddBlob(OscMessage * const oscMessage, const char* const source, const size_t sourceSize) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) + sourceSize > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    size_t argumentsSize = oscMessage->argumentsSize; // local copy in case function returns error
    OscArgument32 blobSize;
    blobSize.int32 = (int32_t) sourceSize;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte3;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte2;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte1;
    oscMessage->arguments[argumentsSize++] = blobSize.byteStruct.byte0;
    int sourceIndex;
    for (sourceIndex = 0; sourceIndex < sourceSize; sourceIndex++) {
        oscMessage->arguments[argumentsSize++] = source[sourceIndex];
    }
    while (argumentsSize % 4 != 0) {
        if (argumentsSize >= MAX_ARGUMENTS_SIZE) {
            return 1; // error: message full
        }
        oscMessage->arguments[argumentsSize++] = 0;
    }
    oscMessage->argumentsSize = argumentsSize;
    oscMessage->oscTypeTagString[(oscMessage->oscTypeTagStringLength)++] = OscTypeTagBlob;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a 64-bit integer argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInt64(&oscMessage, 123);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param int64 64-bit integer to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddInt64(OscMessage * const oscMessage, const uint64_t int64) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument64) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInt64;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument64 oscArgument64;
    oscArgument64.int64 = int64;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds an OSC time tag argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddTimeTag(&oscMessage, OSC_TIME_TAG_ZERO);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param oscTimeTag OSC time tag to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddTimeTag(OscMessage * const oscMessage, const OscTimeTag oscTimeTag) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscTimeTag) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[(oscMessage->oscTypeTagStringLength)++] = OscTypeTagTimeTag;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscTimeTag.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds a 64-bit double argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddDouble(&oscMessage, 3.14);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param double64 64-bit double to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddDouble(OscMessage * const oscMessage, const Double64 double64) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument64) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagDouble;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument64 oscArgument64;
    oscArgument64.double64 = double64;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte7;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte6;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte5;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte4;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument64.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds an alternate string argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddAlternateString(&oscMessage, "Hello World!");
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param string String to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddAlternateString(OscMessage * const oscMessage, const char* string) {
    if (OscMessageAddString(oscMessage, string)) {
        return 1;
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength - 1] = OscTypeTagAlternateString;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a char argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddCharacter(&oscMessage, 'a');
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param string String to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddCharacter(OscMessage * const oscMessage, const char asciiChar) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagCharacter;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = 0;
    oscMessage->arguments[oscMessage->argumentsSize++] = asciiChar;
    return 0;
}

/**
 * @brief Adds a 32-bit RGBA colour argument to an OSC message.
 *
 * Example use:
 * @code
 * const RgbaColour rgbaColour = { 0x00, 0x00, 0x00, 0x00 };
 * OscMessageAddRgbaColour(&oscMessage, rgbaColour);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param rgbaColour 32-bit RGBA colour to be added as argument to the OSC
 * message.
 * @return 0 if successful.
 */
int OscMessageAddRgbaColour(OscMessage * const oscMessage, const RgbaColour rgbaColour) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagRgbaColour;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.rgbaColour = rgbaColour;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds a 4 byte MIDI message argument to an OSC message.
 *
 * Example use:
 * @code
 * const MidiMessage midiMessage = { 0x00, 0x00, 0x00, 0x00 };
 * OscMessageAddMidiMessage(&oscMessage, midiMessage);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param midiMessage 4 byte MIDI message to be added as argument to the OSC
 * message.
 * @return 0 if successful.
 */
int OscMessageAddMidiMessage(OscMessage * const oscMessage, const MidiMessage midiMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    if (oscMessage->argumentsSize + sizeof (OscArgument32) > MAX_ARGUMENTS_SIZE) {
        return 1; // error: message full
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagMidiMessage;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    OscArgument32 oscArgument32;
    oscArgument32.midiMessage = midiMessage;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte3;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte2;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte1;
    oscMessage->arguments[oscMessage->argumentsSize++] = oscArgument32.byteStruct.byte0;
    return 0;
}

/**
 * @brief Adds a boolean argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddBool(&oscMessage, true);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param midiMessage Boolean to be added as argument to the OSC message.
 * @return 0 if successful.
 */
int OscMessageAddBool(OscMessage * const oscMessage, const bool boolean) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = boolean ? OscTypeTagTrue : OscTypeTagFalse;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a nil argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddNil(&oscMessage);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return 0 if successful.
 */
int OscMessageAddNil(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagNil;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds an infinitum argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddInfinitum(&oscMessage);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return 0 if successful.
 */
int OscMessageAddInfinitum(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagInfinitum;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a 'begin array' argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddBeginArray(&oscMessage);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return 0 if successful.
 */
int OscMessageAddBeginArray(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagBeginArray;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Adds a 'end array' argument to an OSC message.
 *
 * Example use:
 * @code
 * OscMessageAddEndArray(&oscMessage);
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return 0 if successful.
 */
int OscMessageAddEndArray(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringLength > MAX_NUMBER_OF_ARGUMENTS) {
        return 1; // error: too many arguments
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength++] = OscTypeTagEndArray;
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string
    return 0;
}

/**
 * @brief Returns the size (number of bytes) of an OSC message.
 *
 * An example use of this function would be to check whether the OSC message size
 * exceeds the remaining capacity of a containing OSC bundle.
 *
 * Example use:
 * @code
 * if(OscMessageGetSize(&oscMessage) > OscBundleGetRemainingCapacity(&oscBundle)) {
 *     printf("oscMessage is too large to be contained within oscBundle");
 * }
 * @endcode
 *
 * @param oscBundle Address of OSC bundle structure.
 * @return Size (number of bytes) of the OSC bundle.
 */
size_t OscMessageGetSize(const OscMessage * const oscMessage) {
    size_t messageSize = 0;
    messageSize += oscMessage->oscAddressPatternLength + 1; // include null character
    if (messageSize % 4 != 0) {
        messageSize += 4 - messageSize % 4; // increase to multiple of 4
    }
    messageSize += oscMessage->oscTypeTagStringLength + 1; // include null character
    if (messageSize % 4 != 0) {
        messageSize += 4 - messageSize % 4; // increase to multiple of 4
    }
    messageSize += oscMessage->argumentsSize;
    return messageSize;
}

/**
 * @brief Converts an OSC message into a char array to be contained within an
 * OSC packet or OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscMessage Address of OSC bundle structure.
 * @param oscMessageSize Address of the OSC bundle size.
 * @param destination Destination address of char array.
 * @param destinationSize Destination size that cannot exceed.
 * @return 0 if successful.
 */
int OscMessageToCharArray(const OscMessage * const oscMessage, size_t * const oscMessageSize, char* const destination, const size_t destinationSize) {
    *oscMessageSize = 0; // size will be 0 if function unsuccessful
    size_t messageSize = 0;
    int i;

    // Address pattern
    if (oscMessage->oscAddressPatternLength == 0) {
        return 1; // error: address pattern not set
    }
    if (oscMessage->oscAddressPattern[0] != (char) OscContentsTypeMessage) {
        return 1; // error: address pattern does not start with '/'
    }
    if (oscMessage->oscAddressPatternLength > destinationSize) {
        return 1; // error: destination too small
    }
    for (i = 0; i < oscMessage->oscAddressPatternLength; i++) {
        destination[messageSize++] = oscMessage->oscAddressPattern[i];
    }
    if (TerminateOscString(destination, &messageSize, destinationSize)) {
        return 1; // error: destination too small
    }

    // Type tag string
    if (messageSize + oscMessage->oscTypeTagStringLength > destinationSize) {
        return 1; // error: destination too small
    }
    for (i = 0; i < oscMessage->oscTypeTagStringLength; i++) {
        destination[messageSize++] = oscMessage->oscTypeTagString[i];
    }
    if (TerminateOscString(destination, &messageSize, destinationSize)) {
        return 1; // error: destination too small
    }

    // Arguments
    if (messageSize + oscMessage->argumentsSize > destinationSize) {
        return 1; // error: destination too small
    }
    for (i = 0; i < oscMessage->argumentsSize; i++) {
        destination[messageSize++] = oscMessage->arguments[i];
    }

    *oscMessageSize = messageSize;
    return 0;
}

/**
 * @brief Terminates an OSC string with one or more null characters so that the
 * OSC string size is a multiple of 4.
 *
 * This is an internal function and cannot be called by the user application.
 *
 * @param oscString OSC string to the terminated.
 * @param oscStringSize Size of the OSC string.
 * @param maxOscStringSize Maximum size of the OSC string that cannot be
 * exceeded.
 * @return 0 if successful.
 */
static int TerminateOscString(char* const oscString, size_t * const oscStringSize, const size_t maxOscStringSize) {
    do {
        if (*oscStringSize >= maxOscStringSize) {
            return 1; // error: string exceeds maximum size
        }
        oscString[(*oscStringSize)++] = '\0';
    } while (*oscStringSize % 4 != 0);
    return 0;
}

//------------------------------------------------------------------------------
// Functions - Message deconstruction

/**
 * @brief Initialises an OSC message from a char array contained within an OSC
 * packet or OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscMessage Address of the OSC message structure.
 * @param source Address of the char array.
 * @param sourceSize Number of bytes within the char array.
 * @return 0 if successful.
 */
int OscMessageInitialiseFromCharArray(OscMessage * const oscMessage, const char* const source, const size_t sourceSize) {
    OscMessageInitialise(oscMessage, "");

    // Return error if not valid OSC message
    if (sourceSize % 4 != 0) {
        return 1; // error: size not multiple of 4
    }
    if (sourceSize < MIN_OSC_MESSAGE_SIZE) {
        return 1; // error: too few bytes to contain an OSC message
    }
    if (sourceSize > MAX_OSC_MESSAGE_SIZE) {
        return 1; // error: size exceeds maximum OSC message size
    }
    if (source[0] != (char) OscContentsTypeMessage) {
        return 1; // error: first byte is not '/'
    }

    // OSC address pattern
    int sourceIndex = 0;
    while (source[sourceIndex] != '\0') {
        oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = source[sourceIndex];
        if (++oscMessage->oscAddressPatternLength > MAX_OSC_ADDRESS_PATTERN_LENGTH) {
            return 1; // error: OSC address pattern too long
        }
        if (++sourceIndex >= sourceSize) {
            return 1; // error: unexpected end of source
        }
    }
    oscMessage->oscAddressPattern[oscMessage->oscAddressPatternLength] = '\0'; // null terminate string

    // Advance index to OSC type tag string
    while (source[sourceIndex - 1] != ',') { // skip index past comma
        if (++sourceIndex >= sourceSize) {
            return 1; // error: unexpected end of source
        }
    }

    // OSC type tag string
    while (source[sourceIndex] != '\0') {
        oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = source[sourceIndex];
        if (++oscMessage->oscTypeTagStringLength > MAX_OSC_TYPE_TAG_STRING_LENGTH) {
            return 1; // error: type tag string too long
        }
        if (++sourceIndex >= sourceSize) {
            return 1; // error: unexpected end of source
        }
    }
    oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringLength] = '\0'; // null terminate string

    // Advance index to arguments
    do {
        if (++sourceIndex > sourceSize) {
            return 1; // error: unexpected end of source
        }
    } while (sourceIndex % 4 != 0);

    // Arguments
    while (sourceIndex < sourceSize) {
        oscMessage->arguments[oscMessage->argumentsSize++] = source[sourceIndex++];
    }

    return 0;
}

/**
 * @brief Returns true if an argument is available indicated by the current
 * oscTypeTagStringIndex value.
 *
 * Example use:
 * @code
 * if(OscMessageIsArgumentAvailable(&oscMessage)) {
 *     printf("Argument is available");
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return true if an argument is available.
 */
bool OscMessageIsArgumentAvailable(OscMessage * const oscMessage) {
    return oscMessage->oscTypeTagStringIndex <= oscMessage->oscTypeTagStringLength - 1;
}

/**
 * @brief Returns OSC type tag of the next argument available within an OSC
 * message indicated by the current oscTypeTagStringIndex value.
 *
 * A null character (value zero) will be returned if no arguments are available.
 *
 * Example use:
 * @code
 * const OscTypeTag oscTypeTag = OscMessageGetArgumentType(&oscMessage));
 * printf("The next argument is: %c", (char)oscTypeTag);
 * @endcode
 *
 * @param oscMessage Address of OSC message structure.
 * @return Next type tag in type tag string.
 */
OscTypeTag OscMessageGetArgumentType(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringIndex > oscMessage->oscTypeTagStringLength) {
        return '\0'; // error: end of type tag string
    }
    return (OscTypeTag) oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex];
}

/**
 * @brief Skips the next argument available within an OSC message indicated by the
 * current oscTypeTagStringIndex value.
 *
 * A null character (value zero) will be returned if no arguments are available.
 * oscTypeTagStringIndex will be incremented to the next OSC bundle element if
 * this function is successful.
 *
 * Example use:
 * @code
 * while(true) { // loop to skip to first int32 argument
 *     if(OscMessageGetArgumentType(&oscMessage) == OscTypeTagInt32) {
 *         break; // found int32 argument
 *     }
 *     if(OscMessageSkipArgument(&oscMessage)) {
 *         break; // error: no more arguments available
 *     }
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @return 0 if successful.
 */
int OscMessageSkipArgument(OscMessage * const oscMessage) {
    if (oscMessage->oscTypeTagStringIndex > oscMessage->oscTypeTagStringLength) {
        return 1; // error: end of type tag string
    }
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 32-bit integer argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 32-bit integer else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagInt32:
 *     {
 *         int32_t int32;
 *         OscMessageGetInt32(&oscMessage, &int32);
 *         printf("Value = %d", int32);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param int32 Address where write the 32-bit integer value will be written.
 * @return 0 if successful.
 */
int OscMessageGetInt32(OscMessage * const oscMessage, int32_t * const int32) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagInt32) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *int32 = oscArgument32.int32;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 32-bit float argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 32-bit float else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagFloat32:
 *     {
 *         float float32;
 *         OscMessageGetFloat32(&oscMessage, &float32);
 *         printf("Value = %f", float32);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param float32 Address where the 32-bit float value will be written.
 * @return 0 if successful.
 */
int OscMessageGetFloat32(OscMessage * const oscMessage, float * const float32) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagFloat32) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *float32 = oscArgument32.float32;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a string or alternate string argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a string else this function will return
 * an error.  The internal index, oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagString:
 *     {
 *         char string[128];
 *         OscMessageGetString(&oscMessage, string, sizeof(string));
 *         printf("Value = %s", string);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param destination Address where the string will be written.
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return 0 if successful.
 */
int OscMessageGetString(OscMessage * const oscMessage, char* const destination, const size_t destinationSize) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagString
            && oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagAlternateString) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof ("\0\0\0") > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    if (sizeof ("\0\0\0") > destinationSize) {
        return 1; // error: destination too small
    }
    int argumentsIndex = oscMessage->argumentsIndex; // local copy in case function returns error
    int destinationIndex = 0;
    do {
        destination[destinationIndex] = oscMessage->arguments[argumentsIndex];
        if (++destinationIndex > destinationSize) {
            return 1; // error: destination too small
        }
    } while (oscMessage->arguments[argumentsIndex++] != '\0');
    while (argumentsIndex % 4 != 0) {
        if (++argumentsIndex > oscMessage->argumentsSize) {
            return 1; // error: message too short to contain argument
        }
    }
    oscMessage->argumentsIndex = argumentsIndex;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a blob (byte array) argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a blob else this function will return
 * an error.  The internal index, oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagBlob:
 *     {
 *         char byteArray[128];
 *         size_t numberOfBytes;
 *         OscMessageGetBlob(&oscMessage, &numberOfBytes, byteArray, sizeof(byteArray));
 *         int i = 0;
 *         while(i <= numberOfBytes) {
 *             printf("%u,", (unsigned int)byteArray[i]);
 *         }
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param blobSize Address where the blob size (number of bytes) will be
 * written.
 * @param destination Address where the byte array will be written.
 * @param destinationSize Size of the destination that cannot be exceeded.
 * @return 0 if successful.
 */
int OscMessageGetBlob(OscMessage * const oscMessage, size_t * const blobSize, char* const destination, const size_t destinationSize) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagBlob) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    int argumentsIndex = oscMessage->argumentsIndex; // local copy in case function returns error
    OscArgument32 blobSizeArgument;
    blobSizeArgument.byteStruct.byte3 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte2 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte1 = oscMessage->arguments[argumentsIndex++];
    blobSizeArgument.byteStruct.byte0 = oscMessage->arguments[argumentsIndex++];
    if (argumentsIndex + blobSizeArgument.int32 > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    if (blobSizeArgument.int32 > destinationSize) {
        return 1; // error: destination too small
    }
    int destinationIndex;
    for (destinationIndex = 0; destinationIndex < blobSizeArgument.int32; destinationIndex++) {
        destination[destinationIndex] = oscMessage->arguments[argumentsIndex++];
    }
    while (argumentsIndex % 4 != 0) {
        if (++argumentsIndex > oscMessage->argumentsSize) {
            return 1; // error: message too short to contain argument
        }
    }
    oscMessage->argumentsIndex = argumentsIndex;
    *blobSize = blobSizeArgument.int32;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 64-bit integer argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 64-bit integer else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagInt64:
 *     {
 *         int64_t int64;
 *         OscMessageGetInt64(&oscMessage, &int64);
 *         printf("Value = %d", int64);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param int64 Address where the 64-bit integer value will be written.
 * @return 0 if successful.
 */
int OscMessageGetInt64(OscMessage * const oscMessage, int64_t * const int64) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagInt64) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument64) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument64 oscArgument64;
    oscArgument64.byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *int64 = oscArgument64.int64;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets an OSC time tag argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be an OSC time tag else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagTimeTag:
 *     {
 *         OscTimeTag oscTimeTag;
 *         OscMessageGetTimeTag(&oscMessage, &oscTimeTag);
 *         printf("Value = %u", (unsigned int)oscTimeTag.seconds);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param oscTimeTag Address where the OSC time tag value will be written.
 * @return 0 if successful.
 */
int OscMessageGetTimeTag(OscMessage * const oscMessage, OscTimeTag * const oscTimeTag) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagTimeTag) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscTimeTag) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    oscTimeTag->byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscTimeTag->byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 64-bit double argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 64-bit double else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagDouble:
 *     {
 *         double double64;
 *         OscMessageGetDouble(&oscMessage, &double64);
 *         printf("Value = %f", double64);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param double64 Address where the 64-bit double value will be written.
 * @return 0 if successful.
 */
int OscMessageGetDouble(OscMessage * const oscMessage, Double64 * const double64) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagDouble) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument64) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument64 oscArgument64;
    oscArgument64.byteStruct.byte7 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte6 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte5 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte4 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument64.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *double64 = oscArgument64.double64;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a character argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a character else this function will
 * return an error.  The internal index, oscTypeTagStringIndex, will only be
 * incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagCharacter:
 *     {
 *         char character;
 *         OscMessageGetCharacter(&oscMessage, &character);
 *         printf("Value = %c", character);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param character Address where the character value will be written.
 * @return 0 if successful.
 */
int OscMessageGetCharacter(OscMessage * const oscMessage, char* const character) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagCharacter) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    oscMessage->argumentsIndex += 3;
    *character = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 32 bit RGBA colour argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 32 bit RGBA colour else this function
 * will return an error.  The internal index, oscTypeTagStringIndex, will only
 * be incremented to the next argument if this function is successful.  The user
 * application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagRgbaColour:
 *     {
 *         RgbaColour rgbaColour;
 *         OscMessageGetRgbaColour(&oscMessage, &rgbaColour);
 *         printf("Value = %u,%u,%u,%u", rgbaColour.red, rgbaColour.green, rgbaColour.blue, rgbaColour.alpha);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param rgbaColour Address where the 32 bit RGBA colour will be written.
 * @return 0 if successful.
 */
int OscMessageGetRgbaColour(OscMessage * const oscMessage, RgbaColour * const rgbaColour) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagRgbaColour) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *rgbaColour = oscArgument32.rgbaColour;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

/**
 * @brief Gets a 4 byte MIDI message argument from an OSC message.
 *
 * The next argument available within the OSC message (indicated by the internal
 * index, oscTypeTagStringIndex) must be a 4 byte MIDI message else this
 * function will return an error.  The internal index, oscTypeTagStringIndex,
 * will only be incremented to the next argument if this function is successful.
 * The user application may determine the next argument type by first calling
 * OscMessageGetArgumentType.
 *
 * Example use:
 * @code
 * switch (OscMessageGetArgumentType(&oscMessage)) {
 *     case OscTypeTagMidiMessage:
 *     {
 *         MidiMessage midiMessage;
 *         OscMessageGetMidiMessage(&oscMessage, &midiMessage);
 *         printf("Value = %u,%u,%u,%u", midiMessage.portID, midiMessage.status, midiMessage.data1, midiMessage.data2);
 *         break;
 *     }
 *     default:
 *         printf("Unexpected argument type");
 *         break;
 * }
 * @endcode
 *
 * @param oscMessage Address of the OSC message structure.
 * @param midiMessage Address where the 4 byte MIDI message will be written.
 * @return 0 if successful.
 */
int OscMessageGetMidiMessage(OscMessage * const oscMessage, MidiMessage * const midiMessage) {
    if (oscMessage->oscTypeTagString[oscMessage->oscTypeTagStringIndex] != OscTypeTagMidiMessage) {
        return 1; // error: unexpected argument type
    }
    if (oscMessage->argumentsIndex + sizeof (OscArgument32) > oscMessage->argumentsSize) {
        return 1; // error: message too short to contain argument
    }
    OscArgument32 oscArgument32;
    oscArgument32.byteStruct.byte3 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte2 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte1 = oscMessage->arguments[oscMessage->argumentsIndex++];
    oscArgument32.byteStruct.byte0 = oscMessage->arguments[oscMessage->argumentsIndex++];
    *midiMessage = oscArgument32.midiMessage;
    oscMessage->oscTypeTagStringIndex++;
    return 0;
}

//------------------------------------------------------------------------------
// End of file
