
/***********************************************************************************************************
  DESCRIPTION:
  This is a LocoNet SMF Block Panel sketch
  - It uses Loconet and the Fremo Block to connect a SMF Panel to Block
  - it uses the mrrwa Loconet Library 1.0

  Based on sketches from Heiko Herholz for Walburg
  03-11-2019  reconstructed so that block works again
  26-01-2019: initialisation reconfigured
  05-05-2018: Ost1/Ost2 ifdef statements added
  16-09-2018: Kranenburg working, all ends work
  23-09-2018: AsT, AsLT, VbHT added, Relaisblockmelder implementing
  30-09-2018: oled implemented
  11-12-2019: rework for speed
************************************************************************************************************/

//#define oledOn true
//#define Tf71 true
#define RB true

#ifdef oledOn
  #include <Boards.h>
  #include <Firmata.h>
  #include <FirmataConstants.h>
  #include <FirmataDefines.h>
  #include <FirmataMarshaller.h>
  #include <FirmataParser.h>
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiAvrI2c.h"
#endif

#include <LocoNet.h>
#include <EEPROM.h>
#include "ln2b_config.h"

#ifdef oledOn
  // 0X3C+SA0 - 0x3C or 0x3D
  #define I2C_ADDRESS 0x3C
  SSD1306AsciiAvrI2c oled;
#endif
/************************************************************************
     variables                                                        *
*************************************************************************/

uint16_t lncv[LNCV_COUNT];
uint16_t lnaddr1 = 0;
uint16_t lnaddr2 = 1;
uint8_t lncvlist[] ={
0,   //address
1,   //Schluessel
1,   //Stör
1,   //Schluessel
2,   //ASpM (Tf71)
2,   //BlockMelder (Tf71)
1,   //WSpM (WiederholsperrenMelder) (RB)
1,   //VbM Vorblockmelder (RB)
1,   //RbM Rückblockmelder (RB)
1,   //AsM (RB)
0,  //not_used in DB
2,  //RM (RB)
2,  //Wecker
1,  //LNTastensperre_EaT
1,  //LNErlaubnisAbgabeRichting
1,  //ESig
1,  //ASig
2,  //LNTrackSensor
2,  //LNTrackSensorOutgoing
2,  //BlGT-RBT
2,  //BlGT-VbHT
1,  //BlGT-EaT
2,  //AsT-RBT
2};  //AsLT-RBT

LocoNetCVClass lnCV;
boolean modeProgramming = false;
static   lnMsg    *LnPacket;

//create boot state
byte receivedEvent        = KEINER;  //no block message received
byte blockErlaubnis       = KEINER; //no one has Erlaubnis
byte endfeldState         = frei_boot;    //endfeldstate is in state boot free
byte anfangsfeldState     = frei;     //anfangfeldstate is free
boolean BlockConnected    = false;   //cable is not connected (checked during setup)
boolean EntranceSignalGo  = false;   //State of Entrance Signal

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

#include "ln2b_oled.h"
#include "ln2b_lncv.h"
#include "ln2b_loconet.h"
#include "ln2b_block.h"

/*************************************************************************/
/*          Setup                                                        */
/*************************************************************************/
void setup()
{
  //resetLNCVSettings();
  loadLNCVSettings();  
  initLed();
  
  #ifdef oledOn
    initoled();
    labeloled();
  #endif
  initLoconet();// Initialize the LocoNet interface
  
  #ifdef oledOn
    updateoled(2,0);
  #endif
  
  initBlockCable(); // Initialize Block cable
  BlockConnected = handleBlockCable();
  
  #ifdef oledOn
    updateoled(1,BlockConnected);
  #endif
  //receivedEvent = ErlaubnisAbgabeTasteBedient;
  
  #ifdef oledOn
  //  updateoled(1,receivedEvent);
  #endif
  
  //handleReceivedEvent(); //for the LN messages

  initBlockState();
  
} //end setup

/*************************************************************************/
/*          Program Loop                                                 */
/*************************************************************************/
void loop()
{
  LnPacket = LocoNet.receive() ; // Check for any received Loconet packets

  // process events
  if (LnPacket)
  {
    uint8_t packetConsumed(LocoNet.processSwitchSensorMessage(LnPacket));
    if (packetConsumed == 0)
    { //it's probably a LNCV message
      packetConsumed = lnCV.processLNCVMessage(LnPacket);
    }
    handleReceivedEvent(); //for the LN messages
  }
  BlockConnected = handleBlockCable(); //keep an eye on the Block cable

  if (Serial.available())
  {
    receivedEvent = handleBlockMsg();             //process block messages
    if (receivedEvent > 0) handleReceivedEvent(); //for the block messages
  }
}
