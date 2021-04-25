#ifndef _HAL_VOLTE_H_
#define _HAL_VOLTE_H_

// =============================================================================
//  MACROS
// =============================================================================
/// FIFO depth.
#define HAL_VOLTE_FIFO_DEPTH (2)

// ============================================================================
// HAL_VOLTE_MODE_T
// -----------------------------------------------------------------------------
/// HAL VoLTE modes
// =============================================================================
typedef enum
{
    HAL_AMR66_DEC = 0x000000F0,
    HAL_AMR66_ENC = 0x0000000F,
    HAL_AMR885_DEC = 0x000000F1,
    HAL_AMR885_ENC = 0x0000001F,
    HAL_AMR1265_DEC = 0x000000F2,
    HAL_AMR1265_ENC = 0x0000002F,
    HAL_AMR1425_DEC = 0x000000F3,
    HAL_AMR1425_ENC = 0x0000003F,
    HAL_AMR1585_DEC = 0x000000F4,
    HAL_AMR1585_ENC = 0x0000004F,
    HAL_AMR1825_DEC = 0x000000F5,
    HAL_AMR1825_ENC = 0x0000005F,
    HAL_AMR1985_DEC = 0x000000F6,
    HAL_AMR1985_ENC = 0x0000006F,
    HAL_AMR2305_DEC = 0x000000F7,
    HAL_AMR2305_ENC = 0x0000007F,
    HAL_AMR2385_DEC = 0x000000F8,
    HAL_AMR2385_ENC = 0x0000008F
} HAL_VOLTE_MODE_T;

// ============================================================================
// HAL_VOLTE_FRAME_SIZE_T
// -----------------------------------------------------------------------------
/// Size of the frame in bytes, for the various supported codecs.
// =============================================================================
typedef enum
{
    HAL_VOLTE_FRAME_SIZE_AMR66 = 0x00000014,   // 132
    HAL_VOLTE_FRAME_SIZE_AMR885 = 0x00000017,  // 177
    HAL_VOLTE_FRAME_SIZE_AMR1265 = 0x00000010, // 253
    HAL_VOLTE_FRAME_SIZE_AMR1425 = 0x00000014, // 285
    HAL_VOLTE_FRAME_SIZE_AMR1585 = 0x00000014, // 317
    HAL_VOLTE_FRAME_SIZE_AMR1825 = 0x00000014, // 365
    HAL_VOLTE_FRAME_SIZE_AMR1985 = 0x0000001C, // 397
    HAL_VOLTE_FRAME_SIZE_AMR2305 = 0x00000020, // 461
    HAL_VOLTE_FRAME_SIZE_AMR2385 = 0x0000003C, // 477
    HAL_VOLTE_FRAME_SIZE_COD_BUF = 0x00000048,
    HAL_VOLTE_FRAME_SIZE_PCM_BUF = 0x00000280
} HAL_VOLTE_FRAME_SIZE_T;

#define HAL_VOLTE_FRAME_SIZE_MAX (HAL_VOLTE_FRAME_SIZE_COD_BUF / 2)

// ============================================================================
// HAL_VOLTE_ENC_OUT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// Mode of the voice coded output frame
    UINT16 encRateMode;   //out
    UINT16 txFrameType;   //out
    UINT16 packFormat;    //in
    UINT16 dataLen;       //out
    UINT16 encOutBuf[36]; //0x00000008
} HAL_VOLTE_ENC_OUT_T;    //Size : 0x2C

// ============================================================================
// HAL_VOLTE_DEC_IN_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT16 dtxOn;        //in
    UINT16 rxFrameType;  //in
    UINT16 decRateMode;  //in
    UINT16 packFormat;   //in
    UINT16 dataLen;      //in
    UINT16 reserved;     //
    UINT16 decInBuf[36]; //0x00000010
} HAL_VOLTE_DEC_IN_T;    //Size : 0x34

// ============================================================================
// HAL_VOLTE_PCM_HALF_BUF_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT16 HAL_VOLTE_PCM_HALF_BUF_T[HAL_VOLTE_FRAME_SIZE_PCM_BUF / 2];

// ============================================================================
// HAL_VOLTE_PCM_BUF_T
// -----------------------------------------------------------------------------
/// Structure defining the VoLTE PCM swap buffers used in VoLTE encoding and
/// decoding
// =============================================================================
typedef struct
{
    HAL_VOLTE_PCM_HALF_BUF_T pcmBuf[2]; //0x00000000
} HAL_VOLTE_PCM_BUF_T;                  //Size : 0x500

// ============================================================================
// HAL_VOLTE_FIFO_ELM_T
// -----------------------------------------------------------------------------
/// This types describes a Fifo section. Rx and Tx are joined together because the
/// we always read and write one Rx and a Tx (or inversly) at the same time.
// =============================================================================
typedef struct
{
    /// Received encoded frame
    HAL_VOLTE_DEC_IN_T rx;
    /// Encoded Frame to transmit
    HAL_VOLTE_ENC_OUT_T tx;
} HAL_VOLTE_FIFO_ELM_T;

// ============================================================================
// HAL_SPEECH_FIFO_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// This types describes a Fifo section. Rx and Tx are joined together because
    /// the we always read and write one Rx and a Tx (or inversly) at the same time.
    HAL_VOLTE_FIFO_ELM_T fifo[HAL_VOLTE_FIFO_DEPTH]; //0x00000000
    /// That boolean is set to TRUE when the Speech Fifo Rx has overflowed, which
    /// also corresponds to a Tx underflow. The reset is manual through coolwatcher.
    BOOL rxOverflowTxUnderflow; //0x00000180
    /// That boolean is set to TRUE when the Speech Fifo Tx has overflowed, which
    /// also corresponds to a Rx underflow. This is normally expected in case of
    /// handover, for example. The reset is manual through coolwatcher.
    BOOL txOverflowRxUnderflow; //0x00000181
} HAL_VOLTE_FIFO_T;             //Size : 0x182

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_VolteFifoReset
// -----------------------------------------------------------------------------
/// Initializes the speech related mechanisms in HAL (Speech Fifo, ...)
// =============================================================================
PUBLIC VOID hal_VolteFifoReset(VOID);

// =============================================================================
// hal_VoltePushRxPopTx
// -----------------------------------------------------------------------------
/// VoLTE side of the FIFO accessor.
///
/// @param pushedRx Pointer to the received trame to push in the FIFO
/// @param popedTx Pointer to the buffer where the trame to send will
/// be copied.
// =============================================================================
PUBLIC VOID hal_VoltePushRxPopTx(CONST HAL_VOLTE_DEC_IN_T *pushedRx,
                                 HAL_VOLTE_ENC_OUT_T *popedTx);

// =============================================================================
// hal_VoltePopRxPushTx
// -----------------------------------------------------------------------------
/// Vois side of the FIFO accessor.
///
/// @param popedRx Pointer where to copy the received trame.
/// @param pushedTx Pointer to the trame to send to push in the FIFO.
// =============================================================================
PUBLIC VOID hal_VoltePopRxPushTx(HAL_VOLTE_DEC_IN_T *popedRx,
                                 CONST HAL_VOLTE_ENC_OUT_T *pushedTx);

#endif
