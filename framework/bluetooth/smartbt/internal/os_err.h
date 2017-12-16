/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OS_ERR_H_
#define _OS_ERR_H_

#define oNoErr                      0       //! No error occurred.
#define oGeneralErr                -1       //! General error.
#define oInProgressErr              1       //! Operation in progress.

// Generic error codes are in the range -6700 to -6779.

#define oGenericErrorBase           -6700   //! Starting error code for all generic errors.

#define oUnknownErr                 -6700   //! Unknown error occurred.
#define oOptionErr                  -6701   //! Option was not acceptable.
#define oSelectorErr                -6702   //! Selector passed in is invalid or unknown.
#define oExecutionStateErr          -6703   //! Call made in the wrong execution state (e.g. called at interrupt time).
#define oPathErr                    -6704   //! Path is invalid, too long, or otherwise not usable.
#define oParamErr                   -6705   //! Parameter is incorrect, missing, or not appropriate.
#define oUserRequiredErr            -6706   //! User interaction is required.
#define oCommandErr                 -6707   //! Command invalid or not supported.
#define oIDErr                      -6708   //! Unknown, invalid, or inappropriate identifier.
#define oStateErr                   -6709   //! Not in appropriate state to perform operation.
#define oRangeErr                   -6710   //! Index is out of range or not valid.
#define oRequestErr                 -6711   //! Request was improperly formed or not appropriate.
#define oResponseErr                -6712   //! Response was incorrect or out of sequence.
#define oChecksumErr                -6713   //! Checksum does not match the actual data.
#define oNotHandledErr              -6714   //! Operation was not handled (or not handled completely).
#define oVersionErr                 -6715   //! Version is not correct or not compatible.
#define oSignatureErr               -6716   //! Signature did not match what was expected.
#define oFormatErr                  -6717   //! Unknown, invalid, or inappropriate file/data format.
#define oNotInitializedErr          -6718   //! Action request before needed services were initialized.
#define oAlreadyInitializedErr      -6719   //! Attempt made to initialize when already initialized.
#define oNotInUseErr                -6720   //! Object not in use (e.g. cannot abort if not already in use).
#define oAlreadyInUseErr            -6721   //! Object is in use (e.g. cannot reuse active param blocks).
#define oTimeoutErr                 -6722   //! Timeout occurred.
#define oCanceledErr                -6723   //! Operation canceled (successful cancel).
#define oAlreadyCanceledErr         -6724   //! Operation has already been canceled.
#define oCannotCancelErr            -6725   //! Operation could not be canceled (maybe already done or invalid).
#define oDeletedErr                 -6726   //! Object has already been deleted.
#define oNotFoundErr                -6727   //! Something was not found.
#define oNoMemoryErr                -6728   //! Not enough memory was available to perform the operation.
#define oNoResourcesErr             -6729   //! Resources unavailable to perform the operation.
#define oDuplicateErr               -6730   //! Duplicate found or something is a duplicate.
#define oImmutableErr               -6731   //! Entity is not changeable.
#define oUnsupportedDataErr         -6732   //! Data is unknown or not supported.
#define oIntegrityErr               -6733   //! Data is corrupt.
#define oIncompatibleErr            -6734   //! Data is not compatible or it is in an incompatible format.
#define oUnsupportedErr             -6735   //! Feature or option is not supported.
#define oUnexpectedErr              -6736   //! Error occurred that was not expected.
#define oValueErr                   -6737   //! Value is not appropriate.
#define oNotReadableErr             -6738   //! Could not read or reading is not allowed.
#define oNotWritableErr             -6739   //! Could not write or writing is not allowed.
#define oBadReferenceErr            -6740   //! An invalid or inappropriate reference was specified.
#define oFlagErr                    -6741   //! An invalid, inappropriate, or unsupported flag was specified.
#define oMalformedErr               -6742   //! Something was not formed correctly.
#define oSizeErr                    -6743   //! Size was too big, too small, or not appropriate.
#define oNameErr                    -6744   //! Name was not correct, allowed, or appropriate.
#define oNotPreparedErr             -6745   //! Device or service is not ready.
#define oReadErr                    -6746   //! Could not read.
#define oWriteErr                   -6747   //! Could not write.
#define oMismatchErr                -6748   //! Something does not match.
#define oDateErr                    -6749   //! Date is invalid or out-of-range.
#define oUnderrunErr                -6750   //! Less data than expected.
#define oOverrunErr                 -6751   //! More data than expected.
#define oEndingErr                  -6752   //! Connection, session, or something is ending.
#define oConnectionErr              -6753   //! Connection failed or could not be established.
#define oAuthenticationErr          -6754   //! Authentication failed or is not supported.
#define oOpenErr                    -6755   //! Could not open file, pipe, device, etc.
#define oTypeErr                    -6756   //! Incorrect or incompatible type (e.g. file, data, etc.).
#define oSkipErr                    -6757   //! Items should be or was skipped.
#define oNoAckErr                   -6758   //! No acknowledge.
#define oCollisionErr               -6759   //! Collision occurred (e.g. two on bus at same time).
#define oBackoffErr                 -6760   //! Backoff in progress and operation intentionally failed.
#define oNoAddressAckErr            -6761   //! No acknowledge of address.
#define oInternalErr                -6762   //! An error internal to the implementation occurred.
#define oNoSpaceErr                 -6763   //! Not enough space to perform operation.
#define oCountErr                   -6764   //! Count is incorrect.
#define oEndOfDataErr               -6765   //! Reached the end of the data (e.g. recv returned 0).
#define oWouldBlockErr              -6766   //! Would need to block to continue (e.g. non-blocking read/write).
#define oLookErr                    -6767   //! Special case that needs to be looked at (e.g. interleaved data).
#define oSecurityRequiredErr        -6768   //! Security is required for the operation (e.g. must use encryption).
#define oOrderErr                   -6769   //! Order is incorrect.
#define oUpgradeErr                 -6770   //! Must upgrade.
#define oAsyncNoErr                 -6771   //! Async operation successfully started and is now in progress.
#define oDeprecatedErr              -6772   //! Operation or data is deprecated.
#define oPermissionErr              -6773   //! Permission denied.

#define oGenericErrorEnd            -6779   //! Last generic error code (inclusive)

#endif
