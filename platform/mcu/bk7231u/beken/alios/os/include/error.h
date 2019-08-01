#ifndef _ERROR_H_
#define _ERROR_H_

#define kNoErr                      0       //! No error occurred.
#define kGeneralErr                -1       //! General error.
#define kInProgressErr              1       //! Operation in progress.

// Generic error codes are in the range -6700 to -6779.
#define kGenericErrorBase           -6700   //! Starting error code for all generic errors.

#define kUnknownErr                 -6700   //! Unknown error occurred.
#define kOptionErr                  -6701   //! Option was not acceptable.
#define kSelectorErr                -6702   //! Selector passed in is invalid or unknown.
#define kExecutionStateErr          -6703   //! Call made in the wrong execution state (e.g. called at interrupt time).
#define kPathErr                    -6704   //! Path is invalid, too long, or otherwise not usable.
#define kParamErr                   -6705   //! Parameter is incorrect, missing, or not appropriate.
#define kUserRequiredErr            -6706   //! User interaction is required.
#define kCommandErr                 -6707   //! Command invalid or not supported.
#define kIDErr                      -6708   //! Unknown, invalid, or inappropriate identifier.
#define kStateErr                   -6709   //! Not in appropriate state to perform operation.
#define kRangeErr                   -6710   //! Index is out of range or not valid.
#define kRequestErr                 -6711   //! Request was improperly formed or not appropriate.
#define kResponseErr                -6712   //! Response was incorrect or out of sequence.
#define kChecksumErr                -6713   //! Checksum does not match the actual data.
#define kNotHandledErr              -6714   //! Operation was not handled (or not handled completely).
#define kVersionErr                 -6715   //! Version is not correct or not compatible.
#define kSignatureErr               -6716   //! Signature did not match what was expected.
#define kFormatErr                  -6717   //! Unknown, invalid, or inappropriate file/data format.
#define kNotInitializedErr          -6718   //! Action request before needed services were initialized.
#define kAlreadyInitializedErr      -6719   //! Attempt made to initialize when already initialized.
#define kNotInUseErr                -6720   //! Object not in use (e.g. cannot abort if not already in use).
#define kAlreadyInUseErr            -6721   //! Object is in use (e.g. cannot reuse active param blocks).
#define kTimeoutErr                 -6722   //! Timeout occurred.
#define kCanceledErr                -6723   //! Operation canceled (successful cancel).
#define kAlreadyCanceledErr         -6724   //! Operation has already been canceled.
#define kCannotCancelErr            -6725   //! Operation could not be canceled (maybe already done or invalid).
#define kDeletedErr                 -6726   //! Object has already been deleted.
#define kNotFoundErr                -6727   //! Something was not found.
#define kNoMemoryErr                -6728   //! Not enough memory was available to perform the operation.
#define kNoResourcesErr             -6729   //! Resources unavailable to perform the operation.
#define kDuplicateErr               -6730   //! Duplicate found or something is a duplicate.
#define kImmutableErr               -6731   //! Entity is not changeable.
#define kUnsupportedDataErr         -6732   //! Data is unknown or not supported.
#define kIntegrityErr               -6733   //! Data is corrupt.
#define kIncompatibleErr            -6734   //! Data is not compatible or it is in an incompatible format.
#define kUnsupportedErr             -6735   //! Feature or option is not supported.
#define kUnexpectedErr              -6736   //! Error occurred that was not expected.
#define kValueErr                   -6737   //! Value is not appropriate.
#define kNotReadableErr             -6738   //! Could not read or reading is not allowed.
#define kNotWritableErr             -6739   //! Could not write or writing is not allowed.
#define kBadReferenceErr            -6740   //! An invalid or inappropriate reference was specified.
#define kFlagErr                    -6741   //! An invalid, inappropriate, or unsupported flag was specified.
#define kMalformedErr               -6742   //! Something was not formed correctly.
#define kSizeErr                    -6743   //! Size was too big, too small, or not appropriate.
#define kNameErr                    -6744   //! Name was not correct, allowed, or appropriate.
#define kNotPreparedErr             -6745   //! Device or service is not ready.
#define kReadErr                    -6746   //! Could not read.
#define kWriteErr                   -6747   //! Could not write.
#define kMismatchErr                -6748   //! Something does not match.
#define kDateErr                    -6749   //! Date is invalid or out-of-range.
#define kUnderrunErr                -6750   //! Less data than expected.
#define kOverrunErr                 -6751   //! More data than expected.
#define kEndingErr                  -6752   //! Connection, session, or something is ending.
#define kConnectionErr              -6753   //! Connection failed or could not be established.
#define kAuthenticationErr          -6754   //! Authentication failed or is not supported.
#define kOpenErr                    -6755   //! Could not open file, pipe, device, etc.
#define kTypeErr                    -6756   //! Incorrect or incompatible type (e.g. file, data, etc.).
#define kSkipErr                    -6757   //! Items should be or was skipped.
#define kNoAckErr                   -6758   //! No acknowledge.
#define kCollisionErr               -6759   //! Collision occurred (e.g. two on bus at same time).
#define kBackoffErr                 -6760   //! Backoff in progress and operation intentionally failed.
#define kNoAddressAckErr            -6761   //! No acknowledge of address.
#define kInternalErr                -6762   //! An error internal to the implementation occurred.
#define kNoSpaceErr                 -6763   //! Not enough space to perform operation.
#define kCountErr                   -6764   //! Count is incorrect.
#define kEndOfDataErr               -6765   //! Reached the end of the data (e.g. recv returned 0).
#define kWouldBlockErr              -6766   //! Would need to block to continue (e.g. non-blocking read/write).
#define kLookErr                    -6767   //! Special case that needs to be looked at (e.g. interleaved data).
#define kSecurityRequiredErr        -6768   //! Security is required for the operation (e.g. must use encryption).
#define kOrderErr                   -6769   //! Order is incorrect.
#define kUpgradeErr                 -6770   //! Must upgrade.
#define kAsyncNoErr                 -6771   //! Async operation successfully started and is now in progress.
#define kDeprecatedErr              -6772   //! Operation or data is deprecated.
#define kPermissionErr              -6773   //! Permission denied.

#define kGenericErrorEnd            -6779   //! Last generic error code (inclusive)
#endif
// eof

