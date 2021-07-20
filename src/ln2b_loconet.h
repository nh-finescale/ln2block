/*************************************************************************/
/* All Loconet related stuff                                               */
/*************************************************************************/

void initLoconet(){
   LocoNet.init(LN_TX_PIN);
}

void sendLNSensor(uint16_t LNaddress, uint8_t LNvalue) {
  /*
   * send Loconet message 
   */
    LocoNet.reportSensor(LNaddress,LNvalue);
}

void ringWecker(){
  /*
   * for the time being: send a LN sensor request. 
   */
   sendLNSensor(lncv[LNWecker], 1); 
   //sendLNSensor(LNWecker, 1); 
}

void setUStoer(boolean UStoerOn){
  /*
   * send the associated LN address the appropriate value
   */
  if (UStoerOn) {
    sendLNSensor(lncv[LNUStoer], 1); 
  } else {
    sendLNSensor(lncv[LNUStoer], 0); 
  }
}

void sendLNSwitch(uint16_t LNaddress, uint8_t LNOutput, uint8_t LNvalue) {
  /*
   * send Loconet message 
   */
    LocoNet.requestSwitch(LNaddress, LNOutput, LNvalue);
}

void sendLN(uint16_t LNaddress, uint8_t LNvalue) {
  if (lncvlist[LNaddress] == 1) {
    //it is a switch
    sendLNSwitch(LNaddress, 1, LNvalue);
  } else if (lncvlist[LNaddress] == 2) {
    //it is a sensor
    sendLNSensor(LNaddress,LNvalue);
  } else if (lncvlist[LNaddress] == 0) {
    //it is none, do not send anything
  }
} 

/*************************************************************************/
/*               Loconet Callback functions                              */
/*************************************************************************/

void notifySensor(uint16_t Address, uint8_t State )
{
  uint8_t kies = 0;
  uint8_t lncvRow = 0;
  
  for (lncvRow = 17; lncvRow <=23; lncvRow++) {
    if (lncv[lncvRow]==Address) {
      kies = lncvRow;
      lncvRow = 24;
      //break;
    } 
  }
  //if ((kies>0) &&(kies<99)) sendLNSwitch(kies,1,1);
  switch (kies) {
    case 0  :
      break;
    /*case 15   : //LNESig       
      if (State = 0x10) {
        receivedEvent = EinfahrtSignalFahrt;
      }
      else if (State = 0x00) {
        receivedEvent = EinfahrtSignalHalt;
      }
      break;*/
    /*case 16 : //LNASig
      if (State == 0x10) {
        receivedEvent = AusfahrtSignalFahrt;
      }
      else if (State == 0x00) {
        receivedEvent = AusfahrtSignalHalt;
      }
      break; */  
    case 17 : //LNTrackSensor  : // occupancy sensor triggered
      if (State == 0x10) {
        receivedEvent = GleiskontaktBefahren;
      }
      break;
    case 18 : //LNTrackSensorOutgoing  : // occupancy sensor exit triggered
      if (State == 0x10) {
        receivedEvent = GleiskontaktBefahren;
      }
      break;
    case 19 : //LNBlGT_RbT      :  
      if (State == 0x10) {
        receivedEvent = RbT_BlGT_Bedient;
      }
      break;
    case 20 : //LNBlGT_VbHT 
      if (State == 0x10) {
        receivedEvent = VbHT_BlGTBedient;
      }
      break;
    case 21 : //LNBlGT_EaT 
      if (State == 0x10) {
        receivedEvent = ErlaubnisAbgabeTasteBedient; 
        }
        break;
    case 22 : //LNAsT_RbT 
      if (State == 0x10) {
        receivedEvent = AsT_RbT_Bedient;
      }
      break;
    case 23 : //LNAsLT_RbT
      if (State == 0x10) {
        receivedEvent = AsLT_RbT_Bedient;
      }
      break;      
    default             :
      break;

  }
}

/*****************************************************************************/
/* This call-back function is called from LocoNet.processSwitchSensorMessage */
/* for all Switch Report messages                                            */
/*****************************************************************************/
void notifySwitchReport( uint16_t Address, uint8_t Output, uint8_t Direction ) {
}

/*****************************************************************************/
/* This call-back function is called from LocoNet.processSwitchSensorMessage */
/* for all Switch State messages                                             */
/*****************************************************************************/
void notifySwitchState( uint16_t Address, uint8_t Output, uint8_t Direction ) {
}

/*****************************************************************************/
/* This call-back function is called from LocoNet.processSwitchSensorMessage */
/* for all Switch Request messages                                           */
/*****************************************************************************/
void notifySwitchRequest( uint16_t Address, uint8_t Output, uint8_t Direction ) {

  uint8_t kies = 0;
  uint8_t lncvRow = 0;
  
  for (lncvRow = 15; lncvRow <=21; lncvRow++) {
    if (lncv[lncvRow]==Address) {
      kies = lncvRow;     
      lncvRow = 24;
      //break;
    } 
  }
    if ((Output ==0x10)&&(kies >= 0)) {
      switch (kies) { 
        case 0  : //not found
          break;
        case 15   : //LNESig       
          if (Direction = 0x00) {
            receivedEvent = EinfahrtSignalHalt;
          } else {//if (Direction = 0x10) {
            receivedEvent = EinfahrtSignalFahrt;
          } 
          break;
        case 16 : //LNASig
          if (Direction == 0x00) {
            receivedEvent = AusfahrtSignalHalt;
          } else {//if (Direction == 0x10) {
            receivedEvent = AusfahrtSignalFahrt;
            sendLNSensor(999,1);
          }       
          break;  
        case 21 : //LNBlGT_EaT 
          if (Direction == 0x00) {
          }
          else {//if (Direction == 0x10) {
            receivedEvent = ErlaubnisAbgabeTasteBedient; 
            }
            break;  
        default             :
          break;
          }
      }
}
