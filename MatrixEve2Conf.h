//User selectable configurations.
//#define EVE2_70
//#define EVE2_50
#define EVE2_43
//#define EVE2_38
//#define EVE2_35
//#define EVE2_29

#ifdef EVE2_70
#define HCYCLE     928
#define HOFFSET     88
#define HSYNC0       0
#define HSYNC1      48
#define VCYCLE     525
#define VOFFSET     32
#define VSYNC0       0
#define VSYNC1       3
#define PCLK         2
#define SWIZZLE      0
#define PCLK_POL     1
#define HSIZE      800
#define VSIZE      480
#define CSPREAD      0
#define DITHER       1
#endif

#ifdef EVE2_50
#define HCYCLE     928
#define HOFFSET     88
#define HSYNC0       0
#define HSYNC1      48
#define VCYCLE     525
#define VOFFSET     32
#define VSYNC0       0
#define VSYNC1       3
#define PCLK         2
#define SWIZZLE      0
#define PCLK_POL     1
#define HSIZE      800
#define VSIZE      480
#define CSPREAD      0
#define DITHER       1
#endif

#ifdef EVE2_43
#define HCYCLE     548
#define HOFFSET     43
#define HSYNC0       0
#define HSYNC1      41
#define VCYCLE     292
#define VOFFSET     12
#define VSYNC0       0
#define VSYNC1      10
#define PCLK         5
#define SWIZZLE      0
#define PCLK_POL     1
#define HSIZE      480
#define VSIZE      272
#define CSPREAD      1
#define DITHER       1
#endif

#ifdef EVE2_38
#define HCYCLE     524
#define HOFFSET     43
#define HSYNC0       0
#define HSYNC1      41
#define VCYCLE     292
#define VOFFSET     12
#define VSYNC0     152
#define VSYNC1      10
#define PCLK         5
#define SWIZZLE      0
#define PCLK_POL     1
#define HSIZE      480
#define VSIZE      272
#define CSPREAD      1
#define DITHER       1
#endif

#ifdef EVE2_35
#define HCYCLE     408
#define HOFFSET     68
#define HSYNC0       0
#define HSYNC1      10
#define VCYCLE     262
#define VOFFSET     18
#define VSYNC0       0
#define VSYNC1       2
#define PCLK         8
#define SWIZZLE      0
#define PCLK_POL     0
#define HSIZE      320
#define VSIZE      240
#define CSPREAD      1
#define DITHER       1
#endif

#ifdef EVE2_29
#define HCYCLE     408
#define HOFFSET     70
#define HSYNC0       0
#define HSYNC1      10
#define VCYCLE     262
#define VOFFSET    156
#define VSYNC0       0
#define VSYNC1       2
#define PCLK         8
#define SWIZZLE      0
#define PCLK_POL     0
#define HSIZE      320
#define VSIZE      102
#define CSPREAD      1
#define DITHER       1
#endif