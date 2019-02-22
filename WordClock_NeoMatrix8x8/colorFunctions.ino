
// show colorshift through the phrase mask. for each NeoPixel either show a color or show nothing!
void applyMask() {

  for (byte i = 0; i < 64; i++) {
    boolean masker = bitRead(mask, 63 - i); // bitread is backwards because bitRead reads rightmost digits first. could have defined the word masks differently
    switch (masker) {
      case 0:
        matrix.setPixelColor(i, 0, 0, 0);
        break;
      case 1:
        matrix.setPixelColor(i, Wheel(((i * 256 / matrix.numPixels()) + j) & 255));
        //matrix.setPixelColor(i, WHITE);
        break;
    }
  }

  matrix.show(); // show it!
  delay(SHIFTDELAY);
  j++; // move the colors forward
  j = j % (256 * 5);

  // reset mask for next time
  mask = 0;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {

  WheelPos = 255 - WheelPos;
  uint32_t wheelColor;

  if (WheelPos < 85) {
    wheelColor = matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    wheelColor = matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    wheelColor = matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }

  // convert from 24-bit to 16-bit color - NeoMatrix requires 16-bit. perhaps there's a better way to do this.
  uint32_t bits = wheelColor;
  uint32_t blue = bits & 0x001F;     // 5 bits blue
  uint32_t green = bits & 0x07E0;    // 6 bits green
  uint32_t red = bits & 0xF800;      // 5 bits red

  // Return shifted bits with alpha set to 0xFF
  return (red << 8) | (green << 5) | (blue << 3) | 0xFF000000;
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < matrix.numPixels(); i++) {
      matrix.setPixelColor(i, Wheel(((i * 256 / matrix.numPixels()) + j) & 255));
    }
    matrix.show();
    delay(wait);
  }
}

typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorRGB;

//a color with 3 components: h, s and v
typedef struct 
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
} ColorHSV;

void plasmaMorph(uint8_t wait){

  float value;
  ColorRGB colorRGB;
  ColorHSV colorHSV;
  long paletteShift=128000;
  float PlasmaScaling = 10.0;

  uint16_t i, x, y;

  for (i = 0; i < wait; i++){ 
     for(y = 0; y < 8; y++)
      for( x = 0; x < 8; x++) {
        value = sin((x + paletteShift) / PlasmaScaling)
                + sin(dist(x, y, 64.0, 64.0) / PlasmaScaling)
                + sin((y + paletteShift / 7.0) / PlasmaScaling)
                + sin(dist(x, y, 192.0, 100.0) / PlasmaScaling);
              colorHSV.h=(unsigned char)((value) * 128)&0xff;
              colorHSV.s=255; 
              colorHSV.v=255;
        HSVtoRGB(&colorRGB, &colorHSV);
    
        matrix.drawPixel(x, y, matrix.Color(colorRGB.r, colorRGB.g, colorRGB.b));
    }
    paletteShift++;
    matrix.show();    
  }
}

//Converts an HSV color to RGB color
void HSVtoRGB(void *vRGB, void *vHSV) 
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  float f, p, q, t;
  int i;
  ColorRGB *colorRGB=(ColorRGB *)vRGB;
  ColorHSV *colorHSV=(ColorHSV *)vHSV;

  h = (float)(colorHSV->h / 256.0);
  s = (float)(colorHSV->s / 256.0);
  v = (float)(colorHSV->v / 256.0);

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  colorRGB->r = (int)(r * 255.0);
  colorRGB->g = (int)(g * 255.0);
  colorRGB->b = (int)(b * 255.0);
}

float
dist(float a, float b, float c, float d) 
{
  return sqrt((c-a)*(c-a)+(d-b)*(d-b));
}
