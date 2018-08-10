// Eve2 Processor Agnostic Library (Condensed)
//
// This "library" consists of the files "Eve2_81x.c" and "Eve2_81x.h".
//
// In persuit of the common goal of simplicity and understandability I find that I am unable to
// make function prototypes that match Bridgetek example code.  I draw the line between the 
// Eve and all other hardware. The library is "clean" and includes no abstraction at all, unlike 
// much of the example code on the Internet which is sort of application and abstraction mixed 
// together in a confusing abuse of my eye-holes.  
// My intent is to be as straight forward and understandable as possible, so while function 
// names and parameter lists are different than Bridgetek code examples, they should be easily 
// recognizable.  I have also made every attempt to reference Bridgetek documentation against 
// the code to act as a translation to help in understanding.

// Notes on the operation of the Eve command processing engine - THE FIFO
//
// First be aware that the FTDI documentation variously refers to you as "User", "MCU", "Host".
// 
// The FIFO, like all FIFO's needs pointers to indicate the starting address of buffered data and
// the end address of buffered data.  There is wrapping involved, but the basic idea is clear.
// Eve takes data into it's FIFO using a fully defined write operation to a memory address - that 
// is, you need to take care of the wrapping - to you, it is not a FIFO - it is a piece of memory.
// Eve keeps track of it's own read address location, but relies on you to write the address
// of the end of buffered data.
// 
// So as commands are loaded into RAM - into the FIFO space - Eve will do nothing in response.
// Eve is happy to take your data and store it for you while it sits with it's read address and 
// write address set to the same value.  Once the commands are loaded, the next available address
// is manually written (by you) to the register in which Eve stores the FIFO write pointer
// (REG_CMD_WRITE).  
//
// Following this, Eve discovers that the addresses are different and begins processing commands while
// updating it's own read pointer until the read and write pointers are the same.
// 
// Be aware that Eve stores only the offset into the "FIFO" as 16 bits, so any use of the offset 
// requires adding the base address (RAM_CMD 0x308000) to the resultant 32 bit value.

#include <stdint.h>              // Find integer types like "uint8_t"  
#include "Eve2_81x.h"            // Header for this file with prototypes, defines, and typedefs
#include "MatrixEve2Conf.h"      // Header for this EVE2 Display configuration settings

// For Propeller, include this:
//#include "simpletools.h"         // Include simple tools for access to common libraries functions
// For Propeller, include only one of the following
//#include "THISLIB_AL.h"          // Include HAL prototypes for library creation - comment for application
//#include "EveLib_AL.h"           // Include HAL functions - comment for library creation

// For Arduino, include this:
#include "Arduino_AL.h"        // Include the hardware abstraction layer for your target processor

// Global Variables 
uint16_t FifoWriteLocation = 0;

// Call this function once at powerup to reset and initialize the Eve chip
void FT81x_Init(void)
{  
  uint8_t ready = false;
  
  Eve_Reset(); // Hard reset of the Eve chip

  // Wakeup Eve
  HostCommand(HCMD_ACTIVE);
  MyDelay(100);
  
  do{
  	  ready = Cmd_READ_REG_ID();
  	}while (!ready);
    
//  Log("Eve now ACTIVE\n");  // Read Eve device ID until it is 0x7c
  
  // turn off screen output during startup
  wr8(REG_GPIOX + RAM_REG, 0);             // Set REG_GPIOX to 0 to turn off the LCD DISP signal
  wr8(REG_PCLK + RAM_REG, 0);              // Pixel Clock Output disable

  // load parameters of the physical screen to the Eve
  // All of these registers are 32 bits, but most bits are reserved, so only write what is actually used
  wr16(REG_HCYCLE + RAM_REG, HCYCLE);         // Set H_Cycle to 548
  wr16(REG_HOFFSET + RAM_REG, HOFFSET);       // Set H_Offset to 43
  wr16(REG_HSYNC0 + RAM_REG, HSYNC0);         // Set H_SYNC_0 to 0
  wr16(REG_HSYNC1 + RAM_REG, HSYNC1);         // Set H_SYNC_1 to 41
  wr16(REG_VCYCLE + RAM_REG, VCYCLE);         // Set V_Cycle to 292
  wr16(REG_VOFFSET + RAM_REG, VOFFSET);       // Set V_OFFSET to 12
  wr16(REG_VSYNC0 + RAM_REG, VSYNC0);         // Set V_SYNC_0 to 0
  wr16(REG_VSYNC1 + RAM_REG, VSYNC1);         // Set V_SYNC_1 to 10
  wr8(REG_SWIZZLE + RAM_REG, SWIZZLE);        // Set SWIZZLE to 0
  wr8(REG_PCLK_POL + RAM_REG, PCLK_POL);      // Set PCLK_POL to 1
  wr16(REG_HSIZE + RAM_REG, HSIZE);           // Set H_SIZE to 480
  wr16(REG_VSIZE + RAM_REG, VSIZE);           // Set V_SIZE to 272
  wr8(REG_CSPREAD + RAM_REG, CSPREAD);        // Set CSPREAD to 1    (32 bit register - write only 8 bits)
  wr8(REG_DITHER + RAM_REG, DITHER);          // Set DITHER to 1     (32 bit register - write only 8 bits)

  // configure touch & audio
  wr16(REG_TOUCH_RZTHRESH + RAM_REG, 1200);          // set touch resistance threshold
  wr8(REG_TOUCH_MODE + RAM_REG, 0x02);               // set touch on: continous - this is default
  wr8(REG_TOUCH_ADC_MODE + RAM_REG, 0x01);           // set ADC mode: differential - this is default
  wr8(REG_TOUCH_OVERSAMPLE + RAM_REG, 15);           // set touch oversampling to max

  wr16(REG_GPIOX_DIR + RAM_REG, 0x8000);             // Set Disp GPIO Direction 
  wr16(REG_GPIOX + RAM_REG, 0x8000);                 // Enable Disp (if used)

  wr16(REG_PWM_HZ + RAM_REG, 0x00FA);                // Backlight PWM frequency
  wr8(REG_PWM_DUTY + RAM_REG, 0x00);                 // Backlight PWM duty (off)   

  // write first display list (which is a clear and blank screen)
  wr32(RAM_DL+0, CLEAR_COLOR_RGB(0,0,0));
  wr32(RAM_DL+4, CLEAR(1,1,1));
  wr32(RAM_DL+8, DISPLAY());
  wr8(REG_DLSWAP + RAM_REG, DLSWAP_FRAME);          // swap display lists
  wr8(REG_PCLK + RAM_REG, 5);                       // after this display is visible on the LCD

//  Log("First screen written\n");
}

// Reset Eve chip via the hardware PDN line
void Eve_Reset(void)
{
  Eve_Reset_HW();
}

// *** Host Command - FT81X Embedded Video Engine Datasheet - 4.1.5 **********************************************
// Host Command is a function for changing hardware related parameters of the Eve chip.  The name is confusing.
// These are related to power modes and the like.  All defined parameters have HCMD_ prefix
void HostCommand(uint8_t HCMD) 
{
//  Log("Inside HostCommand\n");

  SPI_Enable();
  
/*  SPI_Write(HCMD | 0x40); // In case the manual is making you believe that you just found the bug you were looking for - no. */       
  SPI_Write(HCMD);        
  SPI_Write(0x00);          // This second byte is set to 0 but if there is need for fancy, never used setups, then rewrite.  
  SPI_Write(0x00);   
  
  SPI_Disable();
}

// *** Eve API Reference Definitions *****************************************************************************
// FT81X Embedded Video Engine Datasheet 1.3 - Section 4.1.4, page 16
// These are all functions related to writing / reading data of various lengths with a memory address of 32 bits
// ***************************************************************************************************************
void wr32(uint32_t address, uint32_t parameter)
{
  SPI_Enable();
  
  SPI_Write((uint8_t)((address >> 16) | 0x80));   // RAM_REG = 0x302000 and high bit is set - result always 0xB0
  SPI_Write((uint8_t)(address >> 8));             // Next byte of the register address   
  SPI_Write((uint8_t)address);                    // Low byte of register address - usually just the 1 byte offset
  
  SPI_Write((uint8_t)(parameter & 0xff));         // Little endian (yes, it is most significant bit first and least significant byte first)
  SPI_Write((uint8_t)((parameter >> 8) & 0xff));
  SPI_Write((uint8_t)((parameter >> 16) & 0xff));
  SPI_Write((uint8_t)((parameter >> 24) & 0xff));
  
  SPI_Disable();
}

void wr16(uint32_t address, uint16_t parameter)
{
  SPI_Enable();
  
  SPI_Write((uint8_t)((address >> 16) | 0x80)); // RAM_REG = 0x302000 and high bit is set - result always 0xB0
  SPI_Write((uint8_t)(address >> 8));           // Next byte of the register address   
  SPI_Write((uint8_t)address);                  // Low byte of register address - usually just the 1 byte offset
  
  SPI_Write((uint8_t)(parameter & 0xff));       // Little endian (yes, it is most significant bit first and least significant byte first)
  SPI_Write((uint8_t)(parameter >> 8));
  
  SPI_Disable();
}

void wr8(uint32_t address, uint8_t parameter)
{
  SPI_Enable();
  
  SPI_Write((uint8_t)((address >> 16) | 0x80)); // RAM_REG = 0x302000 and high bit is set - result always 0xB0
  SPI_Write((uint8_t)(address >> 8));           // Next byte of the register address   
  SPI_Write((uint8_t)(address));                // Low byte of register address - usually just the 1 byte offset
  
  SPI_Write(parameter);             
  
  SPI_Disable();
}

uint32_t rd32(uint32_t address)
{
  uint8_t buf[4];
  uint32_t Data32;
  
  SPI_Enable();
  
  SPI_Write((address >> 16) & 0x3F);    
  SPI_Write((address >> 8) & 0xff);    
  SPI_Write(address & 0xff);
  
  SPI_ReadBuffer(buf, 4);
  
  SPI_Disable();
  
  Data32 = buf[0] + ((uint32_t)buf[1] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[3] << 24);
  return (Data32);  
}

uint16_t rd16(uint32_t address)
{
  uint8_t buf[2];
    
  SPI_Enable();
  
  SPI_Write((address >> 16) & 0x3F);    
  SPI_Write((address >> 8) & 0xff);    
  SPI_Write(address & 0xff);
  
  SPI_ReadBuffer(buf, 2);
  
  SPI_Disable();
  
  uint16_t Data16 = buf[0] + ((uint16_t)buf[1] << 8);
  return (Data16);  
}

uint8_t rd8(uint32_t address)
{
  uint8_t buf[1];
  
  SPI_Enable();
  
  SPI_Write((address >> 16) & 0x3F);    
  SPI_Write((address >> 8) & 0xff);    
  SPI_Write(address & 0xff);
  
  SPI_ReadBuffer(buf, 1);
  
  SPI_Disable();
  
  return (buf[0]);  
}

// *** Send_Cmd() - this is like cmd() in (some) Eve docs - sends 32 bits but does not update the write pointer ***
// FT81x Series Programmers Guide Section 5.1.1 - Circular Buffer (AKA "the FIFO" and "Command buffer" and "CoProcessor")
// Don't miss section 5.3 - Interaction with RAM_DL
void Send_CMD(uint32_t data)
{
  wr32(FifoWriteLocation + RAM_CMD, data);                         // write the command at the globally tracked "write pointer" for the FIFO

  FifoWriteLocation += FT_CMD_SIZE;                                // Increment the Write Address by the size of a command - which we just sent
  FifoWriteLocation %= FT_CMD_FIFO_SIZE;                           // Wrap the address to the FIFO space
}

// UpdateFIFO - Cause the CoProcessor to realize that it has work to do in the form of a 
// differential between the read pointer and write pointer.  The CoProcessor (FIFO or "Command buffer") does
// nothing until you tell it that the write position in the FIFO RAM has changed
void UpdateFIFO(void)
{
  wr16(REG_CMD_WRITE + RAM_REG, FifoWriteLocation);               // We manually update the write position pointer
}

// Read the specific ID register and return TRUE if it is the expected 0x7C otherwise.
uint8_t Cmd_READ_REG_ID(void)
{
  uint8_t readData[2];
  
  SPI_Enable();
  SPI_Write(0x30);                   // Base address RAM_REG = 0x302000
  SPI_Write(0x20);    
  SPI_Write(REG_ID);                 // REG_ID offset = 0x00
  SPI_ReadBuffer(readData, 1);       // There was a dummy read of the first byte in there
  SPI_Disable();
  
  if (readData[0] == 0x7C)           // FT81x Datasheet section 5.1, Table 5-2. Return value always 0x7C
  {
//    Log("\nGood ID: 0x%02x\n", readData[0]);
    return 1;
  }
  else
  {
//    Log("0x%02x ", readData[0]);
    return 0;
  }
}

// **************************************** Co-Processor/GPU/FIFO/Command buffer Command Functions ***************
// These are discussed in FT81x Series Programmers Guide, starting around section 5.10
// While display list commands can be sent to the CoPro, these listed commands are specific to it.  They are 
// mostly widgets like graphs, but also touch related functions like cmd_track() and memory operations. 
// Essentially, these commands set up parameters for CoPro functions which expand "macros" using those parameters
// to then write a series of commands into the Display List to create all the primitives which make that widget.
// ***************************************************************************************************************

// ******************** Screen Object Creation CoProcessor Command Functions ******************************

// *** Draw Slider - FT81x Series Programmers Guide Section 5.38 *************************************************
void Cmd_Slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range)
{
  Send_CMD(CMD_SLIDER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)h << 16) | w );
  Send_CMD( ((uint32_t)val << 16) | options );
  Send_CMD( (uint32_t)range );
}

// *** Draw Spinner - FT81x Series Programmers Guide Section 5.54 *************************************************
void Cmd_Spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale)
{    
  Send_CMD(CMD_SPINNER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)scale << 16) | style );
}

// *** Draw Gauge - FT81x Series Programmers Guide Section 5.33 **************************************************
void Cmd_Gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
  Send_CMD(CMD_GAUGE);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | r );
  Send_CMD( ((uint32_t)minor << 16) | major );
  Send_CMD( ((uint32_t)range << 16) | val );
}

// *** Draw Dial - FT81x Series Programmers Guide Section 5.39 **************************************************
// This is much like a Gauge except for the helpful range parameter.  For some reason, all dials are 65535 around.
void Cmd_Dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val)
{
  Send_CMD(CMD_DIAL);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | r );
  Send_CMD( (uint32_t)val );
}

// *** Make Track (for a slider) - FT81x Series Programmers Guide Section 5.62 ************************************
// tag refers to the tag # previously assigned to the object that this track is tracking.
void Cmd_Track(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag)
{
    Send_CMD(CMD_TRACK);
    Send_CMD( ((uint32_t)y << 16) | x );
    Send_CMD( ((uint32_t)h << 16) | w );
    Send_CMD( (uint32_t)tag );
}

// *** Draw Number - FT81x Series Programmers Guide Section 5.43 *************************************************
void Cmd_Number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num)
{
  Send_CMD(CMD_NUMBER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | font );
  Send_CMD(num);
}

// *** Draw Smooth Color Gradient - FT81x Series Programmers Guide Section 5.34 **********************************
void Cmd_Gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1)
{
  Send_CMD(CMD_GRADIENT);
  Send_CMD( ((uint32_t)y0<<16)|x0 );
  Send_CMD(rgb0);
  Send_CMD( ((uint32_t)y1<<16)|x1 );
  Send_CMD(rgb1);
}

// *** Draw Button - FT81x Series Programmers Guide Section 5.28 **************************************************
void Cmd_Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str)
{ 
  uint16_t DataPtr, LoopCount, StrPtr;
  
  uint16_t length = strlen(str);
  if(!length) 
    return;
  
  uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
  
  StrPtr = 0;
  for(DataPtr=0; DataPtr<(length/4); DataPtr++, StrPtr += 4)
    data[DataPtr] = (uint32_t)str[StrPtr+3]<<24 | (uint32_t)str[StrPtr+2]<<16 | (uint32_t)str[StrPtr+1]<<8 | (uint32_t)str[StrPtr];
  
  for(LoopCount=0; LoopCount<(length%4); LoopCount++, StrPtr++)
    data[DataPtr] |= (uint32_t)str[StrPtr] << (LoopCount * 8);
  
  Send_CMD(CMD_BUTTON);
  Send_CMD( ((uint32_t)y << 16) | x ); // Put two 16 bit values together into one 32 bit value - do it little endian
  Send_CMD( ((uint32_t)h << 16) | w );
  Send_CMD( ((uint32_t)options << 16) | font );
  
  for(LoopCount=0; LoopCount <= length/4; LoopCount++)
    Send_CMD(data[LoopCount]);

  free(data);
}

// *** Draw Text - FT81x Series Programmers Guide Section 5.41 ***************************************************
void Cmd_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str)
{
  uint16_t DataPtr, LoopCount, StrPtr;
  
  uint16_t length = strlen(str);
  if(!length) 
    return; 
  
  uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t)); // Allocate memory for the string expansion
  
  StrPtr = 0;
  for(DataPtr=0; DataPtr<(length/4); ++DataPtr, StrPtr=StrPtr+4)
    data[DataPtr] = (uint32_t)str[StrPtr+3]<<24 | (uint32_t)str[StrPtr+2]<<16 | (uint32_t)str[StrPtr+1]<<8 | (uint32_t)str[StrPtr];
  
  for(LoopCount=0; LoopCount<(length%4); ++LoopCount, ++StrPtr)
    data[DataPtr] |= (uint32_t)str[StrPtr] << (LoopCount*8);

  // Set up the command
  Send_CMD(CMD_TEXT);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | font );

  // Send out the text
  for(LoopCount = 0; LoopCount <= length/4; LoopCount++)
    Send_CMD(data[LoopCount]);  // These text bytes get sucked up 4 at a time and fired at the FIFO

  free(data);
}

// ******************** Miscellaneous Operation CoProcessor Command Functions ******************************

// *** Cmd_SetBitmap - generate DL commands for bitmap parms - FT81x Series Programmers Guide Section 5.65 *******
void Cmd_SetBitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
  Send_CMD( CMD_SETBITMAP );
  Send_CMD( addr );
  Send_CMD( ((uint32_t)width << 16) | fmt );
  Send_CMD( (uint32_t)height);
}

// *** Cmd_Memcpy - background copy a block of data - FT81x Series Programmers Guide Section 5.27 ****************
void Cmd_Memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
  Send_CMD(CMD_MEMCPY);
  Send_CMD(dest);
  Send_CMD(src);
  Send_CMD(num);
}

// *** Cmd_GetPtr - Get the last used address from CoPro operation - FT81x Series Programmers Guide Section 5.47 *
void Cmd_GetPtr(void)
{
  Send_CMD(CMD_GETPTR);
  Send_CMD(0);
}

// *** Set Highlight Gradient Color - FT81x Series Programmers Guide Section 5.32 ********************************
void Cmd_GradientColor(uint32_t c)
{
  Send_CMD(CMD_GRADCOLOR);
  Send_CMD(c);
}

// *** Set FG color - FT81x Series Programmers Guide Section 5.30 ************************************************
void Cmd_FGcolor(uint32_t c)
{
  Send_CMD(CMD_FGCOLOR);
  Send_CMD(c);
}

// *** Set BG color - FT81x Series Programmers Guide Section 5.31 ************************************************
void Cmd_BGcolor(uint32_t c)
{
  Send_CMD(CMD_BGCOLOR);
  Send_CMD(c);
}

// *** Translate Matrix - FT81x Series Programmers Guide Section 5.51 ********************************************
void Cmd_Translate(uint32_t tx, uint32_t ty)
{
  Send_CMD(CMD_TRANSLATE);
  Send_CMD(tx);
  Send_CMD(ty);
}

// *** Rotate Matrix - FT81x Series Programmers Guide Section 5.50 ***********************************************
void Cmd_Rotate(uint32_t a)
{
  Send_CMD(CMD_ROTATE);
  Send_CMD(a);
}

// *** Rotate Screen - FT81x Series Programmers Guide Section 5.53 ***********************************************
void Cmd_SetRotate(uint32_t rotation)
{
  Send_CMD(CMD_SETROTATE);
  Send_CMD(rotation);
}

// *** Scale Matrix - FT81x Series Programmers Guide Section 5.49 ************************************************
void Cmd_Scale(uint32_t sx, uint32_t sy)
{
  Send_CMD(CMD_SCALE);
  Send_CMD(sx);
  Send_CMD(sy);
}

// *** Calibrate Touch Digitizer - FT81x Series Programmers Guide Section 5.52 ***********************************
// * This business about "result" in the manual really seems to be simply leftover cruft of no purpose - send zero
void Cmd_Calibrate(uint32_t result)
{
  Send_CMD(CMD_CALIBRATE);
  Send_CMD(result);
}

// An interactive calibration screen is created and executed.  
// New calibration values are written to the touch matrix registers of Eve.
void Calibrate_Manual(uint16_t Width, uint16_t Height, uint16_t V_Offset, uint16_t H_Offset)
{
  uint32_t displayX[3], displayY[3];
  uint32_t touchX[3], touchY[3]; 
  uint32_t touchValue = 0, storedValue = 0;  
  int32_t tmp, k;
  int32_t TransMatrix[6];
  uint8_t count = 0;
  char num[2];

  // These values determine where your calibration points will be drawn on your display
  displayX[0] = (Width * 0.15) + H_Offset;
  displayY[0] = (Height * 0.15) + V_Offset;
  
  displayX[1] = (Width * 0.85) + H_Offset;
  displayY[1] = (Height / 2) + V_Offset;
  
  displayX[2] = (Width / 2) + H_Offset;
  displayY[2] = (Height * 0.85) + V_Offset;

  while (count < 3) 
  {
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(64, 64, 64));
    Send_CMD(CLEAR(1,1,1));

    // Draw Calibration Point on screen
    Send_CMD(COLOR_RGB(255, 0, 0));
    Send_CMD(POINT_SIZE(20 * 16));
    Send_CMD(BEGIN(POINTS));
    Send_CMD(VERTEX2F((uint32_t)(displayX[count]) * 16, (uint32_t)((displayY[count])) * 16)); 
    Send_CMD(END());
    Send_CMD(COLOR_RGB(255, 255, 255));
    Cmd_Text((Width / 2) + H_Offset, (Height / 3) + V_Offset, 27, OPT_CENTER, "Calibrating");
    Cmd_Text((Width / 2) + H_Offset, (Height / 2) + V_Offset, 27, OPT_CENTER, "Please tap the dots");
    num[0] = count + 0x31; num[1] = 0;                                            // null terminated string of one character
    Cmd_Text(displayX[count], displayY[count], 27, OPT_CENTER, num);

    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();                                                                 // Trigger the CoProcessor to start processing commands out of the FIFO
    Wait4CoProFIFOEmpty();                                                        // wait here until the coprocessor has read and executed every pending command.
    MyDelay(300);

    touchValue = rd32(REG_TOUCH_DIRECT_XY + RAM_REG);                             // Read for any new touch tag inputs
    if (!(touchValue & 0x80000000))
    {
      touchX[count] = (touchValue>>16) & 0x03FF;                                  // Raw Touchscreen Y coordinate
      touchY[count] = touchValue & 0x03FF;                                        // Raw Touchscreen Y coordinate
      
      //Log("\ndisplay x[%d]: %ld display y[%d]: %ld\n", count, displayX[count], count, displayY[count]);
      //Log("touch x[%d]: %ld touch y[%d]: %ld\n", count, touchX[count], count, touchY[count]);
      
      count++;
    }
  }

  k = ((touchX[0] - touchX[2]) * (touchY[1] - touchY[2])) - ((touchX[1] - touchX[2]) * (touchY[0] - touchY[2])); 
  
  tmp = (((displayX[0] - displayX[2]) * (touchY[1] - touchY[2])) - ((displayX[1] - displayX[2])*(touchY[0] - touchY[2])));
  TransMatrix[0] = CalcCoef(tmp, k);

  tmp = (((touchX[0] - touchX[2]) * (displayX[1] - displayX[2])) - ((displayX[0] - displayX[2])*(touchX[1] - touchX[2])));  
  TransMatrix[1] = CalcCoef(tmp, k);
  
  tmp = ((touchY[0] * (((touchX[2] * displayX[1]) - (touchX[1] * displayX[2])))) + (touchY[1] * (((touchX[0] * displayX[2]) - (touchX[2] * displayX[0])))) + (touchY[2] * (((touchX[1] * displayX[0]) - (touchX[0] * displayX[1])))));    
  TransMatrix[2] = CalcCoef(tmp, k);
    
  tmp = (((displayY[0] - displayY[2]) * (touchY[1] - touchY[2])) - ((displayY[1] - displayY[2])*(touchY[0] - touchY[2])));  
  TransMatrix[3] = CalcCoef(tmp, k);
    
  tmp = (((touchX[0] - touchX[2]) * (displayY[1] - displayY[2])) - ((displayY[0] - displayY[2])*(touchX[1] - touchX[2])));  
  TransMatrix[4] = CalcCoef(tmp, k);
    
  tmp = ((touchY[0] * (((touchX[2] * displayY[1]) - (touchX[1] * displayY[2])))) + (touchY[1] * (((touchX[0] * displayY[2]) - (touchX[2] * displayY[0])))) + (touchY[2] * (((touchX[1] * displayY[0]) - (touchX[0] * displayY[1])))));  
  TransMatrix[5] = CalcCoef(tmp, k);
  
  count = 0;
  do
  {
    wr32(REG_TOUCH_TRANSFORM_A + RAM_REG + (count * 4), TransMatrix[count]);  // Write to Eve config registers

//    uint16_t ValH = TransMatrix[count] >> 16;
//    uint16_t ValL = TransMatrix[count] & 0xFFFF;
//    Log("TM%d: 0x%04x %04x\n", count, ValH, ValL);
    
    count++;
  }while(count < 6);
}

// The following propositional functions are not terribly useful.  I note it here in case you are looking for them.
// Find Inflate used in Load_ZLIB() and Loadimage used in Load_JPG() (process.c)
// void Cmd_Loadimage( uint32_t addr, uint32_t options )
// void Cmd_Inflate( uint32_t addr, uint32_t options )

// ***************************************************************************************************************
// *** Utility and helper functions ******************************************************************************
// ***************************************************************************************************************

// Find the space available in the GPU AKA CoProcessor AKA command buffer AKA FIFO
uint16_t CoProFIFO_FreeSpace(void)
{
  uint16_t cmdBufferDiff, cmdBufferRd, cmdBufferWr, retval;
  
  cmdBufferRd = rd16(REG_CMD_READ + RAM_REG);
  cmdBufferWr = rd16(REG_CMD_WRITE + RAM_REG);
    
  cmdBufferDiff = (cmdBufferWr-cmdBufferRd) % FT_CMD_FIFO_SIZE; // FT81x Programmers Guide 5.1.1
  retval = (FT_CMD_FIFO_SIZE - 4) - cmdBufferDiff;
  return (retval);
}

// Sit and wait until there are the specified number of bytes free in the <GPU/CoProcessor> incoming FIFO
void Wait4CoProFIFO(uint32_t room)
{
   uint16_t getfreespace;
   
   do {
     getfreespace = CoProFIFO_FreeSpace();
   }while(getfreespace < room);
}

// Sit and wait until the CoPro FIFO is empty
void Wait4CoProFIFOEmpty(void)
{
  while( rd16(REG_CMD_READ + RAM_REG) != rd16(REG_CMD_WRITE + RAM_REG) );
}

// Every CoPro transaction starts with enabling the SPI and sending an address
void StartCoProTransfer(uint32_t address, uint8_t reading)
{
  SPI_Enable();
  if (reading){
    SPI_Write(address >> 16);
    SPI_Write(address >> 8);
    SPI_Write(address);
    SPI_Write(0);
  }else{
    SPI_Write((address >> 16) | 0x80); 
    SPI_Write(address >> 8);           
    SPI_Write(address);                
  }
}

// *** CoProWrCmdBuf() - Transfer a buffer into the CoPro FIFO as part of an ongoing command operation ***********
void CoProWrCmdBuf(const uint8_t *buff, uint32_t count)
{
  uint32_t TransferSize = 0;
  int32_t Remaining = count; // signed

  do {                
    // Here is the situation:  You have up to about a megabyte of data to transfer into the FIFO
    // Your buffer is LogBuf - limited to 64 bytes (or some other value, but always limited).
    // You need to go around in loops taking 64 bytes at a time until all the data is gone.
    //
    // Most interactions with the FIFO are started and finished in one operation in an obvious fashion, but 
    // here it is important to understand the difference between Eve RAM registers and Eve FIFO.  Even though 
    // you are in the middle of a FIFO operation and filling the FIFO is an ongoing task, you are still free 
    // to write and read non-FIFO registers on the Eve chip.
    //
    // Since the FIFO is 4K in size, but the RAM_G space is 1M in size, you can not, obviously, send all
    // the possible RAM_G data through the FIFO in one step.  Also, since the Eve is not capable of updating
    // it's own FIFO pointer as data is written, you will need to intermittently tell Eve to go process some
    // FIFO in order to make room in the FIFO for more RAM_G data.  That data might be part of an inflate
    // operation or jpeg decode or the like.  You write to the FIFO and it inflates into RAM_G.
    
    Wait4CoProFIFO(WorkBuffSz);                           // It is reasonable to wait for a small space instead of firing data piecemeal

    if (Remaining > WorkBuffSz)                            // Remaining data exceeds the size of our buffer
      TransferSize = WorkBuffSz;                           // So set the transfer size to that of our buffer
    else
    {
      TransferSize = Remaining;                            // Set size to this last dribble of data
      TransferSize = (TransferSize + 3) & 0xFFC;           // 4 byte alignment
    }
    
    StartCoProTransfer(FifoWriteLocation + RAM_CMD, false);// Base address of the Command Buffer plus our offset into it - Start SPI transaction
    
    SPI_WriteBuffer((uint8_t*)buff, TransferSize);         // write the little bit for which we found space
    buff += TransferSize;                                  // move the working data read pointer to the next fresh data

    FifoWriteLocation  = (FifoWriteLocation + TransferSize) % FT_CMD_FIFO_SIZE;  
    SPI_Disable();                                         // End SPI transaction with the FIFO
    
    wr16(REG_CMD_WRITE + RAM_REG, FifoWriteLocation);      // Manually update the write position pointer to initiate processing of the FIFO
    Remaining -= TransferSize;                             // reduce what we want by what we sent
    
  }while (Remaining > 0);                                  // keep going as long as we still want more
}

// Write a block of data into Eve RAM space a byte at a time.
// Return the last written address + 1 (The next available RAM address)
uint32_t WriteBlockRAM(uint32_t Add, const uint8_t *buff, uint32_t count)
{
  uint8_t index;
  uint32_t WriteAddress = Add;  // I want to return the value instead of modifying the variable in place
  
  for (index = 0; index < count; index++)
  {
    wr8(WriteAddress++, buff[index]);
  }
  return (WriteAddress);
}

int32_t CalcCoef(int32_t Q, int32_t K)
{
  int8_t sn = 0;

  if (Q < 0)                                       // We need to work with positive values
  {
    Q *= -1;                                       // So here we make them positive
    sn++;                                          // and remember that fact
  }

  if (K < 0)                                       
  {
    K *= -1;
    sn++;                                          // 1 + 1 = 2 = 0b00000010
  }  

  
  uint32_t I = ((uint32_t)Q / (uint32_t)K) << 16;  // get the integer part and shift it by 16
  uint32_t R = Q % K;                              // get the remainder of a/k;
  R = R << 14;                                     // shift by 14 
  R = R / K;                                       // divide
  R = R << 2;                                      // Make up for the missing bits  
  int32_t returnValue = I + R;                     // combine them

  if (sn & 0x01)                                   // If the result is supposed to be negative
    returnValue *= -1;                             // then return it to that state.
      
  return (returnValue);
}
