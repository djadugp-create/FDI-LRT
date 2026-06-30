/*************************************************************************
   Description:

   The underlying implementation of the ESP32-HUB75-MatrixPanel-I2S-DMA only
   supports output to HALF scan panels - which means outputting
   two lines at the same time, 16 or 32 rows apart if a 32px or 64px high panel
   respectively.
   This cannot be changed at the DMA layer as it would require a messy and complex
   rebuild of the library's internals.

   However, it is possible to connect QUARTER (i.e. FOUR lines updated in parallel)
   scan panels to this same library and
   'trick' the output to work correctly on these panels by way of adjusting the
   pixel co-ordinates that are 'sent' to the ESP32-HUB75-MatrixPanel-I2S-DMA
   library.

 **************************************************************************/

/* Use the Virtual Display class to re-map co-ordinates such that they draw
   correctly on a 32x16 1/4 or 64x32 1/8 Scan panel (or chain of such panels).
*/
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"

// Define custom class derived from VirtualMatrixPanel


// Panel configuration
#define PANEL_RES_X 32 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 16 // Number of pixels tall of each INDIVIDUAL panel module.

// Use a single panel for tests
#define NUM_ROWS 1 // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS per ROW

// Chain settings, do not cnahge
#define SERPENT true
#define TOPDOWN false
#define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_RIGHT_UP

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
CustomPxBasePanel   *FourScanPanel = nullptr;

/******************************************************************************
   Setup!
 ******************************************************************************/
void setup()
{
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X * 2,            // DO NOT CHANGE THIS
    PANEL_RES_Y / 2,            // DO NOT CHANGE THIS
    NUM_ROWS * NUM_COLS         // DO NOT CHANGE THIS
    //,_pins            // Uncomment to enable custom pins
  );

  // Change this if you see pixels showing up shifted wrongly by one column the left or right.
  mxconfig.clkphase = false; 
  
  // Uncomment this to use a TYPE595 decoder like DP32020/SM5368/TC75xx
// mxconfig.line_decoder = HUB75_I2S_CFG::TYPE595;

  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // let's adjust default brightness to about 75%
  dma_display->setBrightness8(40);    // range is 0-255, 0 - 0%, 255 - 100%

  // Allocate memory and start DMA display
  if ( not dma_display->begin() )
    Serial.println("****** !KABOOM! I2S memory allocation failed ***********");


  dma_display->clearScreen();
  delay(500);

  // create FourScanPanellay object based on our newly created dma_display object
  FourScanPanel = new CustomPxBasePanel ((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y,  VIRTUAL_MATRIX_CHAIN_TYPE);

}


void loop() {
  for (int i = 0; i < FourScanPanel->height(); i++)
  {
    for (int j = 0; j < FourScanPanel->width(); j++)
    {
      FourScanPanel->drawPixel(j, i, FourScanPanel->color565(255, 0, 0));
      delay(30);
    }
  }
  delay(2000);
  dma_display->clearScreen();
} // end loop
