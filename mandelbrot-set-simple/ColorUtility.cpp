#include "ColorUtility.h"

//https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgbColor-to-hsvColor-and-hsvColor-to-rgbColor-in-range-0-255-for-both
//by David H
 rgbColor hsv2rgb(hsvColor in)
{
   double hh, p, q, t, ff;
   long i;
   rgbColor out;

   if (in.saturation <= 0.0)
   {
      // < is bogus, just shuts up warnings
      out.r = in.value;
      out.g = in.value;
      out.b = in.value;
      return out;
   }
   hh = in.hue;
   if (hh >= 360.0) hh = 0.0;
   hh /= 60.0;
   i = (long)hh;
   ff = hh - i;
   p = in.value * (1.0 - in.saturation);
   q = in.value * (1.0 - (in.saturation * ff));
   t = in.value * (1.0 - (in.saturation * (1.0 - ff)));

   switch (i)
   {
   case 0:
      out.r = in.value;
      out.g = t;
      out.b = p;
      break;
   case 1:
      out.r = q;
      out.g = in.value;
      out.b = p;
      break;
   case 2:
      out.r = p;
      out.g = in.value;
      out.b = t;
      break;

   case 3:
      out.r = p;
      out.g = q;
      out.b = in.value;
      break;
   case 4:
      out.r = t;
      out.g = p;
      out.b = in.value;
      break;
   case 5:
   default:
      out.r = in.value;
      out.g = p;
      out.b = q;
      break;
   }
   return out;
}
