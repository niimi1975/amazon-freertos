/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_CLI_UART.h"
#include "iot_uart.h"

extern IotUARTHandle_t xConsoleUart;
#define cmdTIME_OUT_MS    ( 500 )

int32_t uart_read( char * const pcInputBuffer,
                   uint32_t xInputBufferLen );
void uart_write( const char * const pcOutputBuffer,
                 uint32_t xOutputBufferLen );

xConsoleIO_t uartConsoleIO =
{
    .read  = uart_read,
    .write = uart_write
};

int32_t uart_read( char * const pcInputBuffer,
                   uint32_t xInputBufferLen )
{
    int32_t status;
    uint16_t readSize = 0;

    /* Read to input buffer. */
    status = iot_uart_read_async( xConsoleUart, ( uint8_t * ) pcInputBuffer, xInputBufferLen );

    if( status == IOT_UART_SUCCESS )
    {
        vTaskDelay( pdMS_TO_TICKS( cmdTIME_OUT_MS ) );

        status = iot_uart_cancel( xConsoleUart );
        configASSERT( status == IOT_UART_SUCCESS );

        status = iot_uart_ioctl( xConsoleUart, eGetRxNoOfbytes, &readSize );
        configASSERT( status == IOT_UART_SUCCESS );

        status = ( int32_t ) readSize;
    }
    else
    {
        /**
         * Returns a negative error code as per the CLI interface contract.
         */
        status = ( 0L - status );
    }

    return status;
}

void uart_write( const char * const pcOutputBuffer,
                 uint32_t xOutputBufferLen )
{
    int32_t status;

    status = iot_uart_write_sync( xConsoleUart, ( uint8_t * ) pcOutputBuffer, xOutputBufferLen );

    if( status != IOT_UART_SUCCESS )
    {
        configPRINTF( ( "Failed to write to uart %d", status ) );
    }
}
