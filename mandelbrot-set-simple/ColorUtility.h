#pragma once
//https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgbColor-to-hsvColor-and-hsvColor-to-rgbColor-in-range-0-255-for-both
//by David H
typedef struct
{
   double r; // a fraction between 0 and 1
   double g; // a fraction between 0 and 1
   double b; // a fraction between 0 and 1
} rgbColor;

typedef struct
{
   double hue; // angle in degrees
   double saturation; // a fraction between 0 and 1
   double value; // a fraction between 0 and 1
} hsvColor;

rgbColor hsv2rgb(hsvColor in);
