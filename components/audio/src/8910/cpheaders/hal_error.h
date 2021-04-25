#ifndef _HAL_ERROR_H_
#define _HAL_ERROR_H_

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_ERR_T
// -----------------------------------------------------------------------------
/// This types is used to describes all possible error returned by HAL's
/// API functions.
// =============================================================================
typedef enum
{
    /// No error occured
    HAL_ERR_NO,

    /// A resource reset is required
    HAL_ERR_RESOURCE_RESET,

    /// An attempt to access a busy resource failed
    HAL_ERR_RESOURCE_BUSY,

    /// Timeout while trying to access the resource
    HAL_ERR_RESOURCE_TIMEOUT,

    /// An attempt to access a resource that is not enabled
    HAL_ERR_RESOURCE_NOT_ENABLED,

    /// Invalid parameter
    HAL_ERR_BAD_PARAMETER,

    /// Uart RX FIFO overflowed
    HAL_ERR_UART_RX_OVERFLOW,

    /// Uart TX FIFO overflowed
    HAL_ERR_UART_TX_OVERFLOW,
    HAL_ERR_UART_PARITY,
    HAL_ERR_UART_FRAMING,
    HAL_ERR_UART_BREAK_INT,
    HAL_ERR_TIM_RTC_NOT_VALID,
    HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED,
    HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED,

    /// Communication failure
    HAL_ERR_COMMUNICATION_FAILED,

    HAL_ERR_QTY
} HAL_ERR_T;

// =============================================================================
// VOIS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the VOIS module.
// =============================================================================
typedef enum
{
    /// No error occured
    VOIS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    VOIS_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    VOIS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    VOIS_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    VOIS_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    VOIS_ERR_NO_ITF,

    /// What ?
    VOIS_ERR_UNKNOWN,

    VOIS_ERR_QTY
} VOIS_ERR_T;

/// @} // End of the error group

#endif // _HAL_ERROR_H_
