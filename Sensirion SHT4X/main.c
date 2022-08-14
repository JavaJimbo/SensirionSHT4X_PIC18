/*
 * Copyright (c) 2020, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * THIS PROJECT WAS ADAPTED FROM sht4x-5.3.0.zip ON GITHUB AT:
 *  https://github.com/Sensirion/embedded-sht/releases
 * 
 * Project: SensirionSHT4X - Measures temperature and humidity using Sensirion SHT4x sensors.
 *          For PIC 18F44K22 on PIC-P40-28 Proto board with Adafruit TCA9548A I2C multiplexer.
 *          MPLAB X IDE v6.00
 *          Compiled with XC8 v2.36
 * 
 * 8-13-22 JBS: Reading serial number, temperature, and percent RH using SHT4x 1% sensors.
 *              Got two multiplexers working with four sensors on each.
 * 8-14-22 JBS: Saved project on Github
 */

#include "Project.h"
#include "Config.h"
#include "sht4x.h"
#include <stdio.h> 

#define MULTIPLEXER 0x70
#define NUMBER_OF_SENSORS 8
#define NUMBER_OF_MULTIPLEXERS 2

float ConvertCtoF(float TempCelsius)
{
    return ((TempCelsius * 9 / 5) + 32);
}

unsigned char RxBuffer[MAXBUFFER];
unsigned char RxBufferFull = false;

int main(void) 
{
    unsigned long trial = 0;
    float floTempC, floTempF, floRH;
    unsigned char sensorNumber = 0, channel, multiplexerID = 0;
    int16_t errors = 0;
    uint32_t sensorSerialNumber = 0;
    unsigned char i;
    
    /* Initialize the i2c bus for the current platform */
    InitializeHardware();
    DelayMs(100);
    printf("\r\rSTARTING SHT4X TEST #1");
    InitializeI2Cmaster();
    printf("\r\rI2C master Initialized. Checking serial numbers...");
    
    for (i = 0; i < NUMBER_OF_MULTIPLEXERS; i++) DisableMultiplexer(i);

    /* Busy loop for initialization, because the main loop does not work without
     * a sensor.
     */
    multiplexerID = 0;
    channel = 0;
    sensorNumber = 0;
    do {        
        if ((sensorNumber % 4) == 0 && sensorNumber > 0) 
        {
            errors = (int16_t) DisableMultiplexer(multiplexerID);
            if (errors)
            {
                printf("\rMultiplexer ERROR: %d", errors);
                while(1);
            }               
            multiplexerID++;
            channel = 0;
        }
        
        errors = (int16_t) SetMultiplexerChannel(multiplexerID, channel++);        
        if (errors)
        {
            printf("\rMultiplexer ERROR: %d", errors);
            while(1);
        }       
        
        errors = sht4x_probe(&sensorSerialNumber);
        if (errors)
        {
            printf("\rERROR No response SHT sensor #%d", (int)sensorNumber);
            while(1);
        }
        else printf("\r#%d Sensor serial Number: %lu", (int)sensorNumber, (unsigned long) sensorSerialNumber);        
        sensorNumber++;
    } while (sensorNumber < 8);

    DelayMs(4000);
    printf("\r\r");
    multiplexerID = 0;
    channel = 0;   
    while(FOREVER) 
    {        
        multiplexerID = 0;
        channel = 0;
        sensorNumber = 0;
        for (i = 0; i < NUMBER_OF_MULTIPLEXERS; i++) DisableMultiplexer(i);
        DelayMs(1000);
        printf("\r\rTrial #%lu...", trial++);
        
        do {            
            if ( ((sensorNumber % 4) == 0) && sensorNumber > 0)
            {
                DelayMs(100);
                errors = (int16_t) DisableMultiplexer(multiplexerID);
                DelayMs(100);
                if (errors)
                {
                    printf("\rMultiplexer ERROR: %d", errors);
                    while(1);
                }               
                multiplexerID++;
                channel = 0;
            }            
            
            errors = (int16_t) SetMultiplexerChannel(multiplexerID, channel++);  
            DelayMs(10);
            if (errors)
            {
                printf("\rMultiplexer ERROR: %d", errors);
                while(1);
            }     
            
            printf("\rSensor #%d", (int)sensorNumber);        
            errors = sht4x_measure_blocking_read (&floTempC, &floRH);
            DelayMs(10);
            if (!errors) 
            {
                floTempF = ConvertCtoF (floTempC);
                printf("Temp: %0.1f %cF, RH: %0.1f%%", floTempF, 248, floRH);
            }
            else printf("ERROR reading sensor = %d",  errors);  
            sensorNumber++;
        } while (sensorNumber < 8);
    }
    return 0;
}
