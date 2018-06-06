/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef _HAL_AIF_H_
#define _HAL_AIF_H_

#include "cs_types.h"
#include "hal_error.h"


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_SERIAL_MODE_T
// -----------------------------------------------------------------------------
// =============================================================================
typedef enum
{
    HAL_SERIAL_MODE_I2S,
    HAL_SERIAL_MODE_VOICE,
    HAL_SERIAL_MODE_DAI,

    HAL_SERIAL_MODE_QTY
} HAL_SERIAL_MODE_T;


// =============================================================================
// HAL_AIF_RX_DELAY_T
// -----------------------------------------------------------------------------
/// Indicates the delay between serial data in MSB and LRCK edge.
// =============================================================================
typedef enum
{
    /// "00" = Digital audio in MSB is aligned with LRCLK edge.
    HAL_AIF_RX_DELAY_ALIGN,
    /// "01" = Digital audio in MSB is 1 cycle delayed to LRCLK edge.
    HAL_AIF_RX_DELAY_1,
    /// "10" = Digital audio in MSB is 2 cycle delayed to LRCLK edge.
    HAL_AIF_RX_DELAY_2,
    /// "11" = Digital audio in MSB is 3 cycle delayed to LRCLK edge.
    HAL_AIF_RX_DELAY_3,

    HAL_AIF_RX_DELAY_QTY
} HAL_AIF_RX_DELAY_T;



// =============================================================================
// HAL_AIF_TX_DELAY_T
// -----------------------------------------------------------------------------
/// Indicates the delay between serial data out MSB and LRCK edge.
// =============================================================================
typedef enum
{
    /// "00" = Digital audio out MSB is aligned with LRCLK edge.
    HAL_AIF_TX_DELAY_ALIGN,
    /// "01" = Digital audio out MSB is 1 cycle delayed to LRCLK edge.
    HAL_AIF_TX_DELAY_1,

    HAL_AIF_TX_DELAY_QTY
} HAL_AIF_TX_DELAY_T;



// =============================================================================
// HAL_AIF_TX_MODE_T
// -----------------------------------------------------------------------------
/// Configure mono or stereo format for Audio data out.
/// if data from IFC is stereo and AIF works in DAI or Voice mode,  the left
/// channel is always selected.
// =============================================================================
typedef enum
{
    /// Stereo input from IFC, stereo output to pin.
    HAL_AIF_TX_MODE_STEREO_STEREO,
    /// Mono input from IFC, stereo output in left channel to pin
    HAL_AIF_TX_MODE_MONO_STEREO_CHAN_L,
    /// Mono input from IFC, stereo output duplicate in both channels to pin.
    HAL_AIF_TX_MODE_MONO_STEREO_DUPLI,

    HAL_AIF_TX_MODE_QTY
} HAL_AIF_TX_MODE_T ;



// =============================================================================
// HAL_AIF_RX_MODE_T
// -----------------------------------------------------------------------------
/// Configure mono or stereo format for Audio data in.
// =============================================================================
typedef enum
{
    /// Stereo input from pin, stereo output to IFC.
    HAL_AIF_RX_MODE_STEREO_STEREO,
    /// Stereo input from pin, mono input to IFC selected from left channel.
    HAL_AIF_RX_MODE_STEREO_MONO_FROM_L,

    HAL_AIF_RX_MODE_QTY
} HAL_AIF_RX_MODE_T;





// =============================================================================
// HAL_AIF_SERIAL_CFG_T
// -----------------------------------------------------------------------------
// =============================================================================
typedef struct
{
    HAL_SERIAL_MODE_T   mode;

    /// If TRUE, AIF is used as the master
    BOOL                aifIsMaster;

    /// If TRUE, lsb comes first
    BOOL                lsbFirst;

    /// Configure LRCK polarity.
    ///0 = high level on LRCK means left channel, low level means right.
    ///1 = high level on LRCK means right channel, low level means left.
    BOOL                polarity;

    HAL_AIF_RX_DELAY_T  rxDelay;

    HAL_AIF_TX_DELAY_T  txDelay;

    HAL_AIF_RX_MODE_T   rxMode;

    HAL_AIF_TX_MODE_T   txMode;

    /// LRCK frequency (sampling)
    UINT32              fs;

    /// BCK/LRCK ratio
    UINT32              bckLrckRatio;

    /// if Master Mode, invert BCLK out. if slave Mode, invert BCLK in.
    BOOL                invertBck;

    /// Delay Audio output data or LRCK by half cycle.
    BOOL                outputHalfCycleDelay;

    /// Delay Audio input data or LRCK by half cycle.
    BOOL                inputHalfCycleDelay;

    /// Sets the Bck gating. This bit decide if AIF continue to output BCK
    /// clock after 16-bit data has been sent.
    BOOL                enableBckOutGating;
} HAL_AIF_SERIAL_CFG_T;


// =============================================================================
// HAL_AIF_IF_T
// -----------------------------------------------------------------------------
/// Describes the interface used by the AIF. The input and output are
/// decorrellated and different interfaces. Those interfaces can also be either
/// serial or parallel. (NOTA: some combination of input interface with an
/// output interface are forbidden, depending on the capabilities of the chip.
/// Some Output/Inputs do not exist on some chips.)
// =============================================================================
typedef enum
{
    // I2S_2 is reserved for internal FM on 8805/8806;
    // and it is unused on 8808.
    HAL_AIF_IF_SERIAL,
    HAL_AIF_IF_SERIAL_1,
    HAL_AIF_IF_SERIAL_2,
    HAL_AIF_IF_PARALLEL,
    HAL_AIF_IF_PARALLEL_STEREO,
    HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT,
    HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT,
    HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT,
} HAL_AIF_IF_T;


typedef enum
{
    HAL_AIF_TX_STB_SEL_CODEC,
    HAL_AIF_TX_STB_SEL_XTAL,
    HAL_AIF_TX_STB_SEL_FM,

} HAL_AIF_TX_STB_SEL_T;
// =============================================================================
// HAL_AIF_HANDLER_T
// -----------------------------------------------------------------------------
/// Type use to define the user handling function called when an interrupt
/// related to the IFC occurs. Interrupt can be generated when playing or
/// recording a buffer, when we reach the middle or the end of the buffer.
// =============================================================================
typedef VOID (*HAL_AIF_HANDLER_T)(VOID);



// =============================================================================
// HAL_AIF_SR_T
// -----------------------------------------------------------------------------
///  Audio stream sample rate.
// =============================================================================
typedef enum
{
    HAL_AIF_FREQ_8000HZ  =  8000,
    HAL_AIF_FREQ_11025HZ = 11025,
    HAL_AIF_FREQ_12000HZ = 12000,
    HAL_AIF_FREQ_16000HZ = 16000,
    HAL_AIF_FREQ_22050HZ = 22050,
    HAL_AIF_FREQ_24000HZ = 24000,
    HAL_AIF_FREQ_32000HZ = 32000,
    HAL_AIF_FREQ_44100HZ = 44100,
    HAL_AIF_FREQ_48000HZ = 48000
} HAL_AIF_FREQ_T;



// =============================================================================
// HAL_AIF_CH_NB_T
// -----------------------------------------------------------------------------
/// This type defines the number of channels used by a stream
// ============================================================================
typedef enum
{
    HAL_AIF_MONO = 1,
    HAL_AIF_STEREO = 2,
} HAL_AIF_CH_NB_T;



// =============================================================================
// HAL_AIF_CONFIG_T
// -----------------------------------------------------------------------------
/// AIF module configuration structure.
// =============================================================================
typedef struct
{

    HAL_AIF_IF_T            interface;
    HAL_AIF_FREQ_T          sampleRate;
    HAL_AIF_CH_NB_T         channelNb;

    /// This field is only relevant if the serial
    /// interface is used. Otherwise, set it to 0.
    HAL_AIF_SERIAL_CFG_T*   serialCfg;
    HAL_AIF_TX_STB_SEL_T clksel;

} HAL_AIF_CONFIG_T;

// =============================================================================
// HAL_AIF_STREAM_T
// -----------------------------------------------------------------------------
/// This type describes a stream, played or record.
// =============================================================================
typedef struct
{
    UINT32* startAddress;
    /// Stream length in bytes.
    /// The length of a stream must be a multiple of 16 bytes.
    /// The maximum size is 32 KB.
    UINT16 length;
    HAL_AIF_FREQ_T sampleRate;
    HAL_AIF_CH_NB_T channelNb;
    /// True if this is a voice stream, coded on 13 bits, mono, at 8kHz.
    /// Voice quality streams are the only that can be output through
    /// the receiver interface.
    BOOL voiceQuality;
    /// True if the play stream is started along with the record stream.
    /// In this case, the play stream will be configured at first,
    /// but it is not started until the record stream is ready to start.
    BOOL playSyncWithRecord;
    /// Handler called when the middle of the buffer is reached.
    /// If this field is left blank (NULL), no interrupt will be
    /// generated.
    HAL_AIF_HANDLER_T halfHandler;
    /// Handler called when the end of the buffer is reached.
    /// If this field is left blank (NULL), no interrupt will be
    /// generated.
    HAL_AIF_HANDLER_T endHandler;
} HAL_AIF_STREAM_T;


// =============================================================================
// HAL_AIF_TONE_TYPE_T
// -----------------------------------------------------------------------------
/// Tone types.
/// The DTMF Tones are used to inform the user that the number is being
/// composed. All the standard DTMF are available: 0 to 9, A to D,
/// pound and star. \n
/// The Comfort Tones are used to inform the user on the current state of
/// the call: Ringing, Busy, Unavailable... All frequencies needed to do
/// the standard Comfort Tones are available: 425 Hz, 950 Hz, 1400 Hz and
/// 1800 Hz. \n
// =============================================================================
typedef enum
{
    /// Tone when the '0' key
    HAL_AIF_DTMF_0,
    /// Tone when the '1' key
    HAL_AIF_DTMF_1,
    /// Tone when the '2' key
    HAL_AIF_DTMF_2,
    /// Tone when the '3' key
    HAL_AIF_DTMF_3,
    /// Tone when the '4' key
    HAL_AIF_DTMF_4,
    /// Tone when the '5' key
    HAL_AIF_DTMF_5,
    /// Tone when the '6' key
    HAL_AIF_DTMF_6,
    /// Tone when the '7' key
    HAL_AIF_DTMF_7,
    /// Tone when the '8' key
    HAL_AIF_DTMF_8,
    /// Tone when the '9' key
    HAL_AIF_DTMF_9,
    HAL_AIF_DTMF_A,
    HAL_AIF_DTMF_B,
    HAL_AIF_DTMF_C,
    HAL_AIF_DTMF_D,
    /// Tone when the * key
    HAL_AIF_DTMF_S,
    /// Tone when the # key
    HAL_AIF_DTMF_P,
    /// Comfort tone at 425 Hz
    HAL_AIF_COMFORT_425,
    /// Comfort tone at 950 Hz
    HAL_AIF_COMFORT_950,
    /// Comfort tone at 1400 Hz
    HAL_AIF_COMFORT_1400,
    /// Confort tone at 1800 Hz
    HAL_AIF_COMFORT_1800,
    ///
    HAL_AIF_SILENCE
} HAL_AIF_TONE_TYPE_T;


// =============================================================================
// HAL_AIF_TONE_ATTENUATION_T
// -----------------------------------------------------------------------------
/// Attenuation of the tone.
///
/// The attenuation can be set to 0 dB, -3 dB, -9 dB and -15dB.
// =============================================================================
typedef enum
{
    /// No attenuation
    HAL_AIF_TONE_0DB,
    /// -3dB
    HAL_AIF_TONE_M3DB,
    /// -9db
    HAL_AIF_TONE_M9DB,
    /// -15dB
    HAL_AIF_TONE_M15DB
} HAL_AIF_TONE_ATTENUATION_T;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_AifOpen
// -----------------------------------------------------------------------------
/// Open the AIF and set the appropriate config.
/// When the analog interface is used, leave the \c serialCfg field of the
/// \c config parameter at 0.
///
/// This function requests a resource of #HAL_SYS_FREQ_26M only for the pll to be on.
///
/// @param config Pointer to the configuration of the Audio InterFace.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// the AIF has already been opened.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifOpen(CONST HAL_AIF_CONFIG_T* config);

// =============================================================================
// hal_AifClose
// -----------------------------------------------------------------------------
/// Close the AIF.
///
/// This function release the resource to #HAL_SYS_FREQ_32K.
// =============================================================================
PUBLIC VOID hal_AifClose(VOID);


// =============================================================================
// hal_AifSetSideTone
// -----------------------------------------------------------------------------
/// Set the side-tone volume.
/// @param vol Ume.
// =============================================================================
PUBLIC VOID hal_AifSetSideTone(UINT32 vol);

// =============================================================================
// hal_AifPlayStream
// -----------------------------------------------------------------------------
/// Play a stream, copied from a buffer in memory to the AIF fifos, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes. The maximum size is 32 KB.
///
/// @param playedStream Pointer to the played stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable the audio, without
/// playing data from anywhere.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// a play is already in process.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPlayStream(CONST HAL_AIF_STREAM_T* playedStream);



// =============================================================================
// hal_AifRecordStream
// -----------------------------------------------------------------------------
/// Record a stream, copied from the AIF fifos to a buffer in memory, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes. The maximum size is 32 KB.
///
/// @param recordedStream Pointer to the recorded stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable
/// the audio, without recording data from anywhere.
///
// =============================================================================
PUBLIC HAL_ERR_T hal_AifRecordStream(CONST HAL_AIF_STREAM_T* recordedStream);





// =============================================================================
// hal_AifStopPlay
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC VOID hal_AifStopPlay(VOID);


// =============================================================================
// hal_AifStopRecord
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC VOID hal_AifStopRecord(VOID);



// =============================================================================
// hal_AifPause
// -----------------------------------------------------------------------------
/// Pauses the streaming through the AIF
/// @param pause If \c TRUE, pauses a running stream. If \c FALSE, resume a
/// pause stream.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPause(BOOL pause);


// =============================================================================
// hal_AifLoopBack
// -----------------------------------------------------------------------------
/// Loopback the AIF interface
/// @param loop If \c TRUE, the interface is in loopback mod . If \c FALSE,
/// the interface is in normal mode
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC VOID hal_AifLoopBack(BOOL pause);


// =============================================================================
// hal_AifPlayReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedHalf(VOID);


// =============================================================================
// hal_AifPlayReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedEnd(VOID);


// =============================================================================
// hal_AifRecordReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedHalf(VOID);


// =============================================================================
// hal_AifRecordReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedEnd(VOID);




// =============================================================================
// hal_AifTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone.
///
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output
/// as well. This function starts or stops the output of a tone generated in the audio
/// module. \n
/// You can call this function several, if you just need to change the attenuation
/// or the tone type, without stopping the tone generation before (with the \c start
/// parameter set to \c FALSE. \n
/// If the function returns #HAL_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param attenuation The attenuation level of the tone generator
/// @param start If \c TRUE, start to play a tone. If \c FALSE, stop it.
///
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifTone(
    CONST HAL_AIF_TONE_TYPE_T         tone,
    CONST HAL_AIF_TONE_ATTENUATION_T  attenuation,
    CONST BOOL start);



// =============================================================================
// hal_AifTonePause
// -----------------------------------------------------------------------------
/// Pauses a tone generated by the AIF
/// @param pause If \c TRUE, pauses a bipping tone. If \c FALSE, resume a
/// paused tone.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifTonePause(BOOL pause);

// =============================================================================
// hal_AifGetIfcStatusRegPtr
// -----------------------------------------------------------------------------
/// Get a pointer to the IFC Curr_AHB_Addr register.
/// Used to Read this register by VoC.
/// @param
/// @return INT32* pointer to the IFC Curr_AHB_Addr register.
// =============================================================================
PUBLIC INT32* hal_AifGetIfcStatusRegPtr(VOID);

// =============================================================================
// hal_AifGetOverflowStatus
// -----------------------------------------------------------------------------
/// Returns Rx and Tx Overflow status bits
// =============================================================================
PUBLIC UINT32 hal_AifGetOverflowStatus(VOID);

// =============================================================================
// hal_AifPlayDisableIfcAtNextIrq
// -----------------------------------------------------------------------------
/// During Play the IFC channel is in Fifo loop mode
/// In order to stop it cleanly, you can ask to automatically disable
/// the IFC channel at the next Half Fifo/End Fifo interrupt.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPlayDisableIfcAtNextIrq(VOID);

// =============================================================================
// hal_AifRecordDisableIfcAtNextIrq
// -----------------------------------------------------------------------------
/// During Record the IFC channel is in Fifo loop mode
/// In order to stop it cleanly, you can ask to automatically disable
/// the IFC channel at the next Half Fifo/End Fifo interrupt.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifRecordDisableIfcAtNextIrq(VOID);

// =============================================================================
// hal_AifSideToneGainDb2Val
// -----------------------------------------------------------------------------
/// Convert a nominal AIF sidetone gain in dB unit to the corresponding register value.
/// @param db nominal gain in dB unit
/// @return register value
// =============================================================================
PUBLIC UINT32 hal_AifSideToneGainDb2Val(INT32 db);

// =============================================================================
// hal_AifEnFMRecNewPath
// -----------------------------------------------------------------------------
/// it provides a interface for fm record, with which the FM pcm stream cam entrace the aif directly.
/// @param en, enable or disable this feature.
/// @return none
// =============================================================================
#ifdef CHIP_DIE_8955
PUBLIC VOID hal_AifEnFMRecNewPath(BOOL en);
#endif
/// @} //<-- End of aif group


#endif // _HAL_AIF_H_



