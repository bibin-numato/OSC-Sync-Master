/**
 * @file OscBundle.c
 * @author Seb Madgwick
 * @brief Functions and structures for constructing and deconstructing OSC
 * bundles.
 * @see http://opensoundcontrol.org/spec-1_0
 */

//------------------------------------------------------------------------------
// Includes

#include "OscBundle.h"

//------------------------------------------------------------------------------
// Functions - Bundle construction

/**
 * @brief Initialises an OSC bundle structure with a specified OSC time tag.
 *
 * An OSC bundle structure must be initialised before use.  The oscTimeTag
 * argument may be specified as OSC_TIME_TAG_ZERO for an OSC time tag value of
 * zero.  This may be of use if the OSC time tag value is irrelevant to the user
 * application, if the contained OSC messages should be invoke immediately, or
 * if the OSC time tag value is intended to be overwritten after initialisation
 * of the OSC bundle structure.
 *
 * Example use:
 * @code
 * OscBundle oscBundle;
 * OscBundleInitialise(&oscBundle, OSC_TIME_TAG_ZERO);
 * oscBundle.oscTimeTag = 0x100000000; // overwrite OSC time tag with value of 1 second
 * @endcode
 *
 * @param oscPacket Address of the OSC bundle structure to be initialised.
 * @param oscTimeTag OSC time tag.
 */
void OscBundleInitialise(OscBundle * const oscBundle, const OscTimeTag oscTimeTag) {
    oscBundle->header[0] = OSC_BUNDLE_HEADER[0];
    oscBundle->header[1] = OSC_BUNDLE_HEADER[1];
    oscBundle->header[2] = OSC_BUNDLE_HEADER[2];
    oscBundle->header[3] = OSC_BUNDLE_HEADER[3];
    oscBundle->header[4] = OSC_BUNDLE_HEADER[4];
    oscBundle->header[5] = OSC_BUNDLE_HEADER[5];
    oscBundle->header[6] = OSC_BUNDLE_HEADER[6];
    oscBundle->header[7] = OSC_BUNDLE_HEADER[7];
    oscBundle->oscTimeTag = oscTimeTag;
    oscBundle->oscBundleElementsSize = 0;
}

/**
 * @brief Adds an OSC message or OSC bundle to an OSC bundle.
 *
 * The oscContents argument must point to an initialised OSC message or OSC
 * bundle structure.  This function may be called multiple times to add multiple
 * OSC messages or OSC bundles to a containing OSC bundle.  If the remaining
 * capacity of the containing OSC bundle is insufficient to hold the additional
 * contents then the additional contents will be discarded and the function will
 * return an error.
 *
 * Example use:
 * @code
 * OscBundle oscBundle;
 * OscBundleInitialise(&oscBundle, OSC_TIME_TAG_ZERO);
 * OscBundleAddContents(&oscBundle, &oscMessageA);
 * OscBundleAddContents(&oscBundle, &oscMessageB);
 * OscBundleAddContents(&oscBundle, &oscBundleNested);
 * @endcode
 *
 * @param oscBundle Address of OSC bundle structure that will contain the OSC
 * message or OSC bundle to be added.
 * @param oscContents Address of the OSC message structure or OSC bundle
 * structure to be added to the OSC bundle.
 * @return 0 if successful.
 */
int OscBundleAddContents(OscBundle * const oscBundle, const OscContents * const oscContents) {
    if (oscBundle->oscBundleElementsSize + sizeof (OscArgument32) > MAX_OSC_BUNDLE_ELEMENTS_SIZE) {
        return 1; // error: bundle full
    }
    OscBundleElement oscBundleElement;
    oscBundleElement.contents = &oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize + sizeof (OscArgument32)];
    int oscError = 1; // error: invalid or uninitialised OSC contents
    if (OSC_CONTENTS_IS_MESSAGE(oscContents)) {
        oscError = OscMessageToCharArray((OscMessage*) oscContents, (size_t*) & oscBundleElement.size.int32, oscBundleElement.contents, OscBundleGetRemainingCapacity(oscBundle));
    }
    if (OSC_CONTENTS_IS_BUNDLE(oscContents)) {
        oscError = OscBundleToCharArray((OscBundle*) oscContents, (size_t*) & oscBundleElement.size.int32, oscBundleElement.contents, OscBundleGetRemainingCapacity(oscBundle));
    }
    if (oscError != 0) {
        return oscError; // error: ???
    }
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte3;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte2;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte1;
    oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = oscBundleElement.size.byteStruct.byte0;
    oscBundle->oscBundleElementsSize += oscBundleElement.size.int32;
    return 0;
}

/**
 * @brief Empties an OSC bundle.
 *
 * All OSC bundle elements contained within the OSC bundle are discarded.  The
 * OSC bundle's OSC time tag is not modified.
 *
 * Example use:
 * @code
 * OscBundleEmpty(&oscBundle);
 * @endcode
 *
 * @param oscBundle Address of OSC bundle structure to be emptied.
 */
void OscBundleEmpty(OscBundle * const oscBundle) {
    oscBundle->oscBundleElementsSize = 0;
}

/**
 * @brief Returns true is the OSC bundle is empty.
 *
 * An empty OSC bundle contains no OSC bundle elements (OSC messages or OSC
 * bundles) but does retain an OSC time tag.
 *
 * Example use:
 * @code
 * if(OscBundleIsEmpty(&oscBundle) {
 *     printf("oscBundle is empty.");
 * }
 * @endcode
 *
 * @param oscBundle Address of the OSC bundle structure.
 * @return true is the OSC bundle is empty.
 */
bool OscBundleIsEmpty(OscBundle * const oscBundle) {
    return oscBundle->oscBundleElementsSize == 0;
}

/**
 * @brief Returns the remaining capacity (number of bytes) of an OSC bundle.
 *
 * The remaining capacity of an OSC bundle is the number of bytes available to
 * contain an OSC message or OSC bundle.
 *
 * Example use:
 * @code
 * const int remainingCapacity = OscBundleGetRemainingCapacity(&oscBundle);
 * @endcode
 *
 * @param oscBundle Address of OSC bundle structure.
 * @return Remaining capacity (number of bytes) of an OSC bundle.
 */
size_t OscBundleGetRemainingCapacity(const OscBundle * const oscBundle) {
    return MAX_OSC_BUNDLE_ELEMENTS_SIZE - oscBundle->oscBundleElementsSize - sizeof (OscArgument32); // account for int32 size required by OSC bundle element
}

/**
 * @brief Returns the size (number of bytes) of an OSC bundle.
 *
 * An example use of this function would be to check whether the OSC bundle size
 * exceeds the remaining capacity of a containing OSC bundle.
 *
 * Example use:
 * @code
 * if(OscBundleGetSize(&oscBundleChild) > OscBundleGetRemainingCapacity(&oscBundleParent)) {
 *     printf("oscBundleChild is too large to be contained within oscBundleParent");
 * }
 * @endcode
 *
 * @param oscBundle Address of OSC bundle structure.
 * @return Size (number of bytes) of the OSC bundle.
 */
size_t OscBundleGetSize(const OscBundle * const oscBundle) {
    return sizeof (OSC_BUNDLE_HEADER) + sizeof (OscTimeTag) + oscBundle->oscBundleElementsSize;
}

/**
 * @brief Converts an OSC bundle into a char array to be contained within an OSC
 * packet or containing OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle Address of OSC bundle structure.
 * @param oscBundleSize Address of the OSC bundle size.
 * @param destination Destination address of char array.
 * @param destinationSize Destination size that cannot exceed.
 * @return 0 if successful.
 */
int OscBundleToCharArray(const OscBundle * const oscBundle, size_t * const oscBundleSize, char* const destination, const size_t destinationSize) {
    *oscBundleSize = 0; // size will be 0 if function unsuccessful
    if (sizeof (OSC_BUNDLE_HEADER) + sizeof (OscTimeTag) + oscBundle->oscBundleElementsSize > destinationSize) {
        return 1; // error: destination too small
    }
    size_t destinationIndex = 0;
    int i;
    for (i = 0; i < sizeof (OSC_BUNDLE_HEADER); i++) {
        destination[destinationIndex++] = oscBundle->header[i];
    }
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte7;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte6;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte5;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte4;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte3;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte2;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte1;
    destination[destinationIndex++] = oscBundle->oscTimeTag.byteStruct.byte0;
    for (i = 0; i < oscBundle->oscBundleElementsSize; i++) {
        destination[destinationIndex++] = oscBundle->oscBundleElements[i];
    }
    *oscBundleSize = destinationIndex;
    return 0;
}

//------------------------------------------------------------------------------
// Functions - Bundle deconstruction

/**
 * @brief Initialises an OSC bundle from a char array contained within an OSC
 * packet or containing OSC bundle.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle Address of the OSC bundle structure.
 * @param source Address of the char array.
 * @param sourceSize Number of bytes within the char array.
 * @return 0 if successful.
 */
int OscBundleInitialiseFromCharArray(OscBundle * const oscBundle, const char* const source, const size_t sourceSize) {
    int sourceIndex = 0;

    // Return error if not valid bundle
    if (sourceSize % 4 != 0) {
        return 1; // error: size not multiple of 4
    }
    if (sourceSize < MIN_OSC_BUNDLE_SIZE) {
        return 1; // error: too few bytes to contain bundle
    }
    if (sourceSize > MAX_OSC_BUNDLE_SIZE) {
        return 1; // error: size exceeds maximum bundle size
    }
    if (source[sourceIndex] != (char) OscContentsTypeBundle) {
        return 1; // error: first byte is not '#'
    }

    // Header
    oscBundle->header[0] = source[sourceIndex++];
    oscBundle->header[1] = source[sourceIndex++];
    oscBundle->header[2] = source[sourceIndex++];
    oscBundle->header[3] = source[sourceIndex++];
    oscBundle->header[4] = source[sourceIndex++];
    oscBundle->header[5] = source[sourceIndex++];
    oscBundle->header[6] = source[sourceIndex++];
    oscBundle->header[7] = source[sourceIndex++];

    // OSC time tag
    oscBundle->oscTimeTag.byteStruct.byte7 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte6 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte5 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte4 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte3 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte2 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte1 = source[sourceIndex++];
    oscBundle->oscTimeTag.byteStruct.byte0 = source[sourceIndex++];

    // Osc bundle elements
    oscBundle->oscBundleElementsSize = 0;
    oscBundle->oscBundleElementsIndex = 0;
    do {
        oscBundle->oscBundleElements[oscBundle->oscBundleElementsSize++] = source[sourceIndex++];
    } while (sourceIndex < sourceSize);

    return 0;
}

/**
 * @brief Returns true if an OSC bundle element is available based on the
 * current oscBundleElementsIndex value.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle Address of OSC bundle structure.
 * @return true if a bundle element is available.
 */
bool OscBundleIsBundleElementAvailable(const OscBundle * const oscBundle) {
    return oscBundle->oscBundleElementsIndex + sizeof (OscArgument32) < oscBundle->oscBundleElementsSize;
}

/**
 * @brief Gets the next OSC bundle element available within the OSC bundle based
 * on the current oscBundleElementsIndex.
 *
 * oscBundleElementsIndex will be incremented to the next OSC bundle element if
 * this function is successful.  Otherwise, the oscBundleElementsIndex will
 * remain unmodified.
 *
 * This function is used internally and should not be used by the user
 * application.
 *
 * @param oscBundle Address of OSC bundle structure.
 * @param oscBundleElement Address of OSC bundle element.
 * @return 0 if successful.
 */
int OscBundleGetBundleElement(OscBundle * const oscBundle, OscBundleElement * const oscBundleElement) {
    if (oscBundle->oscBundleElementsIndex + sizeof (OscArgument32) >= oscBundle->oscBundleElementsSize) {
        return 1; // error: too few bytes to contain bundle element
    }
    oscBundleElement->size.byteStruct.byte3 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte2 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte1 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    oscBundleElement->size.byteStruct.byte0 = oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex++];
    if (oscBundleElement->size.int32 < 0) {
        return 1; // error: size cannot be negative
    }
    if (oscBundleElement->size.int32 % 4 != 0) {
        return 1; // error: size not multiple of 4
    }
    if (oscBundle->oscBundleElementsIndex + oscBundleElement->size.int32 > oscBundle->oscBundleElementsSize) {
        return 1; // error: too few bytes for indicated size
    }
    oscBundleElement->contents = &oscBundle->oscBundleElements[oscBundle->oscBundleElementsIndex];
    oscBundle->oscBundleElementsIndex += oscBundleElement->size.int32;
    return 0;
}

//------------------------------------------------------------------------------
// End of file
