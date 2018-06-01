#ifndef __EVE81X_H
#define __EVE81X_H

// =====================================================================================
// Required Functions - Hardware driver or otherwise environment specific. Abstracted  |
// and found in ArduinoAL.h or whatever the abstraction layer is called in your world. |
// This library requires base support functions for SPI, delays, and hardware pin      |
// control.                                                                            |
// =====================================================================================
//
// Delays
// void MyDelay(uint32_t DLY);
//
// Pin control
// void SetPin(uint8_t, Boolean);
// void Eve_Reset_HW(void);
//
// SPI functions
// void SPI_Enable(void);
// void SPI_Disable(void);
// void SPI_Write(uint8_t data);
// void SPI_WriteByte(uint8_t data);
// void SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length);
// void SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length);
//
// #define WorkBuffSz 64 
//
// =====================================================================================

// Just in case this is for Arduino - Eve wants a DISPLAY() macro and Arduino already defines DISPLAY 
// for something else that we will not be using, so we can kill the Arduino define.
#undef DISPLAY    

// Once again, if this is being used for Arduino, it wants C++, but C is totally cool as long as it is
// wrapped in this extern declaration (also see the bottom of the file for the close bracket)           
#ifdef __cplusplus
extern "C" {
#endif

//#include <stdint.h>              // Find integer types like "uint8_t"  

#define HCMD_ACTIVE      0x00
#define HCMD_STANDBY     0x41
#define HCMD_SLEEP       0x42
#define HCMD_PWRDOWN     0x50
#define HCMD_CLKINT      0x48
#define HCMD_CLKEXT      0x44
#define HCMD_CLK48M      0x62
#define HCMD_CLK36M      0x61
#define HCMD_CORERESET   0x68

#define CMD_APPEND           0xFFFFFF1E
#define CMD_BGCOLOR          0xFFFFFF09
#define CMD_BUTTON           0xFFFFFF0D
#define CMD_CALIBRATE        0xFFFFFF15 // 4294967061UL
#define CMD_CLOCK            0xFFFFFF14
#define CMD_COLDSTART        0xFFFFFF32
#define CMD_CRC              0xFFFFFF18
#define CMD_DIAL             0xFFFFFF2D
#define CMD_DLSTART          0xFFFFFF00
#define CMD_FGCOLOR          0xFFFFFF0A
#define CMD_GAUGE            0xFFFFFF13
#define CMD_GETMATRIX        0xFFFFFF33
#define CMD_GETPROPS         0xFFFFFF25
#define CMD_GETPTR           0xFFFFFF23
#define CMD_GRADCOLOR        0xFFFFFF34
#define CMD_GRADIENT         0xFFFFFF0B
#define CMD_INFLATE          0xFFFFFF22
#define CMD_INTERRUPT        0xFFFFFF02
#define CMD_KEYS             0xFFFFFF0E
#define CMD_LOADIDENTITY     0xFFFFFF26
#define CMD_LOADIMAGE        0xFFFFFF24
#define CMD_LOGO             0xFFFFFF31
#define CMD_MEDIAFIFO        0xFFFFFF39
#define CMD_MEMCPY           0xFFFFFF1D
#define CMD_MEMCRC           0xFFFFFF18
#define CMD_MEMSET           0xFFFFFF1B
#define CMD_MEMWRITE         0xFFFFFF1A
#define CMD_MEMZERO          0xFFFFFF1C
#define CMD_NUMBER           0xFFFFFF38
#define CMD_PLAYVIDEO        0xFFFFFF3A
#define CMD_PROGRESS         0xFFFFFF0F
#define CMD_REGREAD          0xFFFFFF19
#define CMD_ROTATE           0xFFFFFF29
#define CMD_SCALE            0xFFFFFF28
#define CMD_SCREENSAVER      0xFFFFFF2F
#define CMD_SCROLLBAR        0xFFFFFF11
#define CMD_SETBITMAP        0xFFFFFF43
#define CMD_SETFONT          0xFFFFFF2B
#define CMD_SETMATRIX        0xFFFFFF2A
#define CMD_SETROTATE        0xFFFFFF36
#define CMD_SKETCH           0xFFFFFF30
#define CMD_SLIDER           0xFFFFFF10
#define CMD_SNAPSHOT         0xFFFFFF1F
#define CMD_SPINNER          0xFFFFFF16
#define CMD_STOP             0xFFFFFF17
#define CMD_SWAP             0xFFFFFF01
#define CMD_TEXT             0xFFFFFF0C
#define CMD_TOGGLE           0xFFFFFF12
#define CMD_TRACK            0xFFFFFF2C
#define CMD_TRANSLATE        0xFFFFFF27
#define CMD_VIDEOFRAME       0xFFFFFF41
#define CMD_VIDEOSTART       0xFFFFFF40

#define DLSWAP_FRAME         2UL

#define OPT_CENTER           1536UL
#define OPT_CENTERX          512UL
#define OPT_CENTERY          1024UL
#define OPT_FLAT             256UL
#define OPT_MONO             1UL
#define OPT_NOBACK           4096UL
#define OPT_NODL             2UL
#define OPT_NOHANDS          49152UL
#define OPT_NOHM             16384UL
#define OPT_NOPOINTER        16384UL
#define OPT_NOSECS           32768UL
#define OPT_NOTICKS          8192UL
#define OPT_RIGHTX           2048UL
#define OPT_SIGNED           256UL

// Definitions for FT8xx co processor command buffer
#define FT_DL_SIZE           (8*1024)  // 8KB Display List buffer size
#define FT_CMD_FIFO_SIZE     (4*1024)  // 4KB coprocessor Fifo size
#define FT_CMD_SIZE          (4)       // 4 byte per coprocessor command of EVE

// Memory block base addresses
#define RAM_G                    0x0
#define RAM_DL                   0x300000
#define RAM_REG                  0x302000
#define RAM_CMD                  0x308000

// Graphics Engine Registers - FT81x Series Programmers Guide Section 3.1
// Addresses defined as offsets from the base address called RAM_REG and located at 0x302000
// Discussion: Defining this way leads to an additional add operation in code that can be avoided by defining
// these addresses as 32 bit values, but this is easily paid for in clarity and coorelation to documentation.
// Further, you can add defines together in code and allow the precompiler to do the add operation (as here).
#define REG_CSPREAD               0x68
#define REG_DITHER                0x60
#define REG_DLSWAP                0x54
#define REG_HCYCLE                0x2C
#define REG_HOFFSET               0x30    
#define REG_HSIZE                 0x34
#define REG_HSYNC0                0x38
#define REG_HSYNC1                0x3C
#define REG_OUTBITS               0x5C
#define REG_PCLK                  0x70
#define REG_PCLK_POL              0x6C
#define REG_PLAY                  0x8C
#define REG_PLAYBACK_FORMAT       0xC4
#define REG_PLAYBACK_FREQ         0xC0
#define REG_PLAYBACK_LENGTH       0xB8
#define REG_PLAYBACK_LOOP         0xC8
#define REG_PLAYBACK_PLAY         0xCC
#define REG_PLAYBACK_READPTR      0xBC
#define REG_PLAYBACK_START        0xB4
#define REG_PWM_DUTY              0xD4
#define REG_ROTATE                0x58
#define REG_SOUND                 0x88
#define REG_SWIZZLE               0x64
#define REG_TAG                   0x7C
#define REG_TAG_X                 0x74
#define REG_TAG_Y                 0x78
#define REG_VCYCLE                0x40
#define REG_VOFFSET               0x44
#define REG_VOL_SOUND             0x84
#define REG_VOL_PB                0x80
#define REG_VSYNC0                0x4C
#define REG_VSYNC1                0x50
#define REG_VSIZE                 0x48 

// Touch Screen Engine Registers - FT81x Series Programmers Guide Section 3.3
// Addresses defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_TOUCH_CONFIG          0x168
#define REG_TOUCH_TRANSFORM_A     0x150
#define REG_TOUCH_TRANSFORM_B     0x154
#define REG_TOUCH_TRANSFORM_C     0x158
#define REG_TOUCH_TRANSFORM_D     0x15C
#define REG_TOUCH_TRANSFORM_E     0x160
#define REG_TOUCH_TRANSFORM_F     0x164

// Resistive Touch Engine Registers - FT81x Series Programmers Guide Section 3.3.3 - Document confused
// Addresses defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_TOUCH_ADC_MODE        0x108
#define REG_TOUCH_CHARGE          0x10C
#define REG_TOUCH_DIRECT_XY       0x18C
#define REG_TOUCH_DIRECT_Z1Z2     0x190 
#define REG_TOUCH_MODE            0x104
#define REG_TOUCH_OVERSAMPLE      0x114
#define REG_TOUCH_RAW_XY          0x11C
#define REG_TOUCH_RZ              0x120
#define REG_TOUCH_RZTHRESH        0x118
#define REG_TOUCH_SCREEN_XY       0x124
#define REG_TOUCH_SETTLE          0x110
#define REG_TOUCH_TAG             0x12C
#define REG_TOUCH_TAG_XY          0x128

// Capacitive Touch Engine Registers - FT81x Series Programmers Guide Section 3.3.4
// Addresses defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_CTOUCH_MODE           0x104
#define REG_CTOUCH_EXTEND         0x108
#define REG_CTOUCH_RAW_XY         0x11C
#define REG_CTOUCH_TOUCH_XY       0x124
#define REG_CTOUCH_TOUCH1_XY      0x11C
#define REG_CTOUCH_TOUCH2_XY      0x18C
#define REG_CTOUCH_TOUCH3_XY      0x190
#define REG_CTOUCH_TOUCH4_X       0x16C
#define REG_CTOUCH_TOUCH4_Y       0x120
#define REG_CTOUCH_TAG            0x12C
#define REG_CTOUCH_TAG1           0x134
#define REG_CTOUCH_TAG2           0x13C
#define REG_CTOUCH_TAG3           0x144
#define REG_CTOUCH_TAG4           0x14C
#define REG_CTOUCH_TAG_XY         0x128
#define REG_CTOUCH_TAG1_XY        0x130
#define REG_CTOUCH_TAG2_XY        0x138
#define REG_CTOUCH_TAG3_XY        0x140
#define REG_CTOUCH_TAG4_XY        0x148

// Co-processor Engine Registers - FT81x Series Programmers Guide Section 3.4
// Addresses defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_CMD_DL                0x100
#define REG_CMD_READ              0xF8
#define REG_CMD_WRITE             0xFC
#define REG_CMDB_SPACE            0x574
#define REG_CMDB_WRITE            0x578

// Special Registers - FT81x Series Programmers Guide Section 3.5 
// Addresses assumed to be defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_TRACKER               0x7000
#define REG_TRACKER_1             0x7004
#define REG_TRACKER_2             0x7008
#define REG_TRACKER_3             0x700C
#define REG_TRACKER_4             0x7010
#define REG_MEDIAFIFO_READ        0x7014
#define REG_MEDIAFIFO_WRITE       0x7018

// Miscellaneous Registers - FT81x Series Programmers Guide Section 3.6 - Document inspecific about base address
// Addresses assumed to be defined as offsets from the base address called RAM_REG and located at 0x302000
#define REG_CPU_RESET             0x20
#define REG_PWM_DUTY              0xD4
#define REG_PWM_HZ                0xD0
#define REG_INT_MASK              0xB0
#define REG_INT_EN                0xAC
#define REG_INT_FLAGS             0xA8
#define REG_GPIO                  0x94
#define REG_GPIO_DIR              0x90
#define REG_GPIOX                 0x9C
#define REG_GPIOX_DIR             0x98
#define REG_FREQUENCY             0x0C
#define REG_CLOCK                 0x08
#define REG_FRAMES                0x04
#define REG_ID                    0x00
#define REG_TRIM                  0x10256C
#define REG_SPI_WIDTH             0x180

// Primitive Type Reference Definitions - FT81x Series Programmers Guide Section 4.5 - Table 6
#define BITMAPS                    1
#define POINTS                     2
#define LINES                      3
#define LINE_STRIP                 4
#define EDGE_STRIP_R               5
#define EDGE_STRIP_L               6
#define EDGE_STRIP_A               7
#define EDGE_STRIP_B               8
#define RECTS                      9

// Bitmap Layout Format Definitions - FT81x Series Programmers Guide Section 4.7 - Table 7
#define ARGB1555                   0
#define L1                         1
#define L4                         2
#define L8                         3
#define RGB332                     4
#define ARGB2                      5
#define ARGB4                      6
#define RGB565                     7
#define TEXT8X8                    9
#define TEXTVGA                   10
#define BARGRAPH                  11
#define PALETTED565               14
#define PALETTED4444              15
#define PALETTED8                 16
#define L2                        17

// Bitmap Parameters
#define REPEAT                     1
#define BORDER                     0
#define NEAREST                    0
#define BILINEAR                   1

// These defined "macros" are supplied by FTDI - Manufacture command bit-fields from parameters
// FT81x Series Programmers Guide is refered to as "FT-PG"
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))                                                                                           // CLEAR - FT-PG Section 4.21
#define CLEAR_COLOR_RGB(red,green,blue) ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))                                                         // CLEAR_COLOR_RGB - FT-PG Section 4.23
#define COLOR_RGB(red,green,blue) ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))                                                               // COLOR_RGB - FT-PG Section 4.28
#define VERTEX2II(x,y,handle,cell) ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))                                              // VERTEX2II - FT-PG Section 4.48
#define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))                                                                                                 // VERTEX2F - FT-PG Section 4.47
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))                                                                                                                       // CELL - FT-PG Section 4.20
#define BITMAP_HANDLE(handle) ((5UL<<24) | (((handle) & 31UL) << 0))                                                                                                     // BITMAP_HANDLE - FT-PG Section 4.06
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&1048575UL)<<0))                                                                                                          // BITMAP_SOURCE - FT-PG Section 4.11
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))                                       // BITMAP_LAYOUT - FT-PG Section 4.07
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0)) // BITMAP_SIZE - FT-PG Section 4.09
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))                                                                                                                              // TAG - FT-PG Section 4.43
#define POINT_SIZE(sighs) ((13UL<<24)|(((sighs)&8191UL)<<0))                                                                                                             // POINT_SIZE - FT-PG Section 4.36
#define BEGIN(PrimitiveTypeRef) ((31UL<<24)|(((PrimitiveTypeRef)&15UL)<<0))                                                                                              // BEGIN - FT-PG Section 4.05
#define END() ((33UL<<24))                                                                                                                                               // END - FT-PG Section 4.30
#define DISPLAY() ((0UL<<24))                                                                                                                                            // DISPLAY - FT-PG Section 4.29

// Non FTDI Helper Macros
#define MAKE_COLOR(r,g,b) (( r << 16) | ( g << 8) | (b))

// Global Variables
extern uint16_t FifoWriteLocation;

// Function Prototypes
void FT81x_Init(void);
void Eve_Reset(void);

void HostCommand(uint8_t HostCommand); 
void wr32(uint32_t address, uint32_t parameter);
void wr16(uint32_t, uint16_t parameter);
void wr8(uint32_t, uint8_t parameter);
uint8_t rd8(uint32_t RegAddr);
uint16_t rd16(uint32_t RegAddr);
uint32_t rd32(uint32_t RegAddr);
void Send_CMD(uint32_t data);
void UpdateFIFO(void);
uint8_t Cmd_READ_REG_ID(void);

// Widgets and other significant screen objects
void Cmd_Slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range);
void Cmd_Spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale);
void Cmd_Gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void Cmd_Dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val);
void Cmd_Track(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag);
void Cmd_Number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num);
void Cmd_Gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1);
void Cmd_Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str);
void Cmd_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str);

void Cmd_SetBitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void Cmd_Memcpy(uint32_t dest, uint32_t src, uint32_t num);
void Cmd_GetPtr(void);
void Cmd_GradientColor(uint32_t c);
void Cmd_FGcolor(uint32_t c);
void Cmd_Translate(uint32_t tx, uint32_t ty);
void Cmd_Rotate(uint32_t a);
void Cmd_SetRotate(uint32_t rotation);
void Cmd_Scale(uint32_t sx, uint32_t sy);
void Cmd_Calibrate(uint32_t result);

uint16_t CoProFIFO_FreeSpace(void);
void Wait4CoProFIFO(uint32_t room);
void Wait4CoProFIFOEmpty(void);
void StartCoProTransfer(uint32_t address, uint8_t reading);
void CoProWrCmdBuf(const uint8_t *buffer, uint32_t count);
uint32_t WriteBlockRAM(uint32_t Add, const uint8_t *buff, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif
