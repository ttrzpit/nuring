/* Serial Interface*/

#pragma once 

#include <windows.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <math.h>
#include <utility>
#include <iomanip>
#include <sstream>

#define MAX_DATA_LENGTH     14


class SerialInterface {

    private:

    HANDLE handler ;
    bool connected ;
    COMSTAT status ;
    DWORD errors ;
    uint8_t MAX_OUTGOING_DATA_LENGTH = 20 ;

    int nZeroesPayload = 3 ;
    int nZeroesNumber = 4 ;
    std::string outgoingPacket ;
    std::string packetHeader ;
    std::string packetFilename ;
    std::string packetNumber ;
    std::string packetPayloadA ;
    std::string packetPayloadB ;
    std::string packetFooter ;





    public:

        /* Initialize new serial controller
        @param  portName    [char] The name of the port to be used

        @returns            None */
    SerialInterface( char* portName , int baudRate ) {

        this->connected = false ;

        this->handler = CreateFileA( static_cast< LPCSTR >( portName ) ,
            GENERIC_READ | GENERIC_WRITE ,  // dwDesiredAccess
            // GENERIC_WRITE ,  // dwDesiredAccess
            0 ,                             // dwShareMode              0
            NULL ,                          // lpSecurityAttributes
            OPEN_EXISTING ,                 // dwCreationDisposition    OPEN_EXISTING
            FILE_ATTRIBUTE_NORMAL ,         // dwFlagsAndAttributes
            NULL );                         // hTemplateFile            NULL

        if ( this->handler == INVALID_HANDLE_VALUE ) {
            if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
                printf( "ERROR: Handle was not attached. Reason: %s is not available\n" , portName );
            } else {
                printf( "ERROR! Serial device may already be in use." );
            }
        } else {
            DCB dcbSerialParameters = { 0 };

            if ( !GetCommState( this->handler , &dcbSerialParameters ) ) {
                printf( "Failed to get current serial parameters" );
            } else {
                // dcbSerialParameters.BaudRate = CBR_256000;
                dcbSerialParameters.BaudRate = baudRate ;
                dcbSerialParameters.ByteSize = 8;
                dcbSerialParameters.StopBits = ONESTOPBIT;
                dcbSerialParameters.Parity = NOPARITY;
                // dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
                dcbSerialParameters.fDtrControl = DTR_CONTROL_DISABLE ;

                if ( !SetCommState( handler , &dcbSerialParameters ) ) {
                    printf( "ALERT: could not set Serial port parameters\n" );
                } else {
                    this->connected = true;
                    PurgeComm( this->handler , PURGE_RXCLEAR | PURGE_TXCLEAR );

                }
            }
        }
    }



    /* Close the serial controller*/
    ~SerialInterface() {
        if ( this->connected ) {
            this->connected = false ;
            CloseHandle( this->handler ) ;
        }
    }

    /* Write data to serial port
    @param      buffer      [char] Text to be sent
    @param      buf_size    [int] Size of buffer in bytes
    @returns                True/false  */
    void writeSerial( char* buffer , unsigned int buf_size ) {

        DWORD bytesSend ;

        if ( !WriteFile( this->handler , ( void* ) buffer , buf_size , &bytesSend , 0 ) ) {

            ClearCommError( this->handler , &this->errors , &this->status ) ;
            // return false ;
        } else {

            // return true ;
        }


    }




    /* Read serial port
    @param      buffer      [char] Text to be read
    @param      buf_size    [int] Size of buffer in bytes
    @returns                Status */
    int readSerial( char* buffer , unsigned int buf_size ) {

        DWORD bytesRead ;
        unsigned int toRead = 0 ;

        ClearCommError( this->handler , &this->errors , &this->status ) ;

        // Check size
        if ( this->status.cbInQue > 0 ) {

            if ( this->status.cbInQue > buf_size ) {
                toRead = buf_size ;
            } else {
                toRead = this->status.cbInQue ;
            }
        }

        if ( ReadFile( this->handler , buffer , toRead , &bytesRead , NULL ) ) {
            return bytesRead ;
        }

        return 0 ;

    }
} ;
