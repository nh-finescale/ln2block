/*************************************************************************/
/* All Block related stuff                                               */
/*************************************************************************/
void sendBlockMsg(byte number) {
  Serial.write(END);
  Serial.write(number);
  Serial.write(END);
}

byte readBlockMsg() {
  byte number = 0;
  byte readvalue = 0;
  /*
     read multibyte messages, only get our relevant one
      sit in a loop reading bytes until we put together
     a whole packet.
  */
  while (1) {
    /*
        get a character to process
    */
    readvalue = Serial.read();
    switch (readvalue) {
      /*
         if it's an END character then we're done with
         the packet
      */
      case END:
        /*
           done, give back what you have read
        */
        return number;
      /*
          if it's the same code as an ESC character, wait
         and get another character and then figure out
         what to store in the packet based on that.
      */
      case ESC:
        readvalue = Serial.read();
        switch (readvalue) {
          case ESC_END:
            readvalue = END;
            break;
          case ESC_ESC:
            readvalue = ESC;
            break;
        }
      /*
         We are only interested in the 5 messages below
         so only these get passed back
      */
      case VORBLOCK               : if (number == 0) number = readvalue;
        break;
      case RUECKBLOCK             : if (number == 0) number = readvalue;
        break;
      case ERLAUBNISABGABE        : if (number == 0) number = readvalue;
        break;
      case ERLAUBNISANFRAGE       : if (number == 0) number = readvalue;
        break;
      case ANSTOSSMELDUNG         : if (number == 0) number = 0; //do nothing with this
        break;
      default                     : break;
    }
  }
}

void setBlockErlaubnis(byte allow) {

  switch (allow) {
    case KEINER:
      blockErlaubnis = KEINER; 
      digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
      break;
    case HOLEN:
      blockErlaubnis = HOLEN;
      digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
      break;
    case ERHALTEN:
      blockErlaubnis = ERHALTEN;
      digitalWrite(LED_YELLOW_BLOCKING_HERE, LOW);
      break;
    case ABGEGEBEN:
      blockErlaubnis = ABGEGEBEN;
      digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
      break;
    default:    break;
  }

}
void setBlockState(byte state) {
  /*
     Generic state setting of the block
  */
  switch (state) {
    case BLOCK_FREE:
      sendLNSensor(lncv[LNBlockMelder], 0);
      digitalWrite(LED_RED_BLOCK_OCCUPIED, HIGH);
      break;
    case BLOCK_OCC:
      sendLNSensor(lncv[LNBlockMelder], 1);
      digitalWrite(LED_RED_BLOCK_OCCUPIED, LOW);
      break;
    default:    break;
  }
}

void setAnfangsFeldState(byte state) {
  /*
     Generic state setting of the anfangsFeld
  */
  switch (state) {
    case frei:
        anfangsfeldState = frei;
      break;
    case belegt:
        anfangsfeldState = belegt;
      break;
    case fahrt:
        anfangsfeldState = fahrt;
      break;
    case fluegel_kupplung:
        anfangsfeldState = fluegel_kupplung;
      break;
    case einfahrsignal:
        anfangsfeldState = einfahrsignal;
      break;
    case fahrt_pre:
        anfangsfeldState = fahrt_pre;
      break;
    default:    break;

    #ifdef oledOn
      updateoled(6,anfangsfeldState);
    #endif
    
  }
}
void setEndFeldState(byte state) {
  /*
     Generic state setting of the endFeld
  */
  switch (state) {
    case frei_boot:
        endfeldState = frei_boot;
      break;
    case frei:
        endfeldState = frei;
      break;
    case belegt:
        endfeldState = belegt;
      break;
    case signal_gezogen:
        endfeldState = signal_gezogen;
      break;
    case anschalter_aktiv:
        endfeldState = anschalter_aktiv;
      break;
    case erste_achse:
        endfeldState = erste_achse;
      break;
    case geraeumt:
        endfeldState = geraeumt;
      break;
    default:    break;
    
    #ifdef oledOn
      updateoled(6,endfeldState);
    #endif
      
  }
}

/*************************************************************************/
/* All Panel messages related stuff                                               */
/*************************************************************************/

/*
   endfeld
*/
void handleEinfahrsignalFahrt() {
  #ifdef Tf71
    sendLNSensor(lncv[LNAnschaltMelder], 1);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNAnschaltMelder],1, 1);
  #endif
  setEndFeldState(signal_gezogen);
}

void handleEinfahrsignalHalt() {
  #ifdef Tf71
    sendLNSensor(lncv[LNAnschaltMelder], 0);
    sendLNSensor(lncv[LNVorblockMelder], 0);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNVorblockMelder], 1, 0); //RB
    sendLNSwitch(lncv[LNAnschaltMelder],1, 0);
  #endif
  setEndFeldState(frei);
}

void handleGleiskontaktEinfahrtBefahren() {
  setEndFeldState(erste_achse);
}

void handleAsT_RbT_Bedient() {
  #ifdef Tf71
    sendLNSensor(lncv[LNAnschaltMelder], 1);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNAnschaltMelder],1, 1);
  #endif
  setEndFeldState(anschalter_aktiv);
}

void handleAsLT_RbT_Bedient() {
  #ifdef Tf71
    sendLNSensor(lncv[LNAnschaltMelder], 0);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNAnschaltMelder],1, 0);
  #endif
  setEndFeldState(belegt);
}

void handleRaemungsmeldung() {
  #ifdef RB
    sendLNSwitch(lncv[LNRaeumungsMelder], 1, 1); //RB
  #endif
  setEndFeldState(geraeumt);
}

void handleEinfahrsignalZurueck() {
  #ifdef Tf71
    sendLNSensor(lncv[LNAnschaltMelder], 0);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNAnschaltMelder],1, 0);
  #endif
  setEndFeldState(belegt);
}

void handleEinfahrsignalFahrtAnfangsfeld() {
  setEndFeldState(einfahrsignal);
}

void handleEinfahrsignalHaltAnfangsfeld() {
  setEndFeldState(fahrt);
}

void handleVorblockEmpfangen() {
  /*
     Message of type VORBLOCK
     When A Block Vorblock message is received:
      - return an ACK message
      - set direction of Block inbound
      - set block occupied
      - display block occupied on LN2Block
      - send block occupied message to panel
      - let a Wecker ring
  */
  sendBlockMsg(VORBLOCK_ACK);
  setBlockErlaubnis(ABGEGEBEN);
  setBlockState(BLOCK_OCC);
  setEndFeldState(belegt);
  #ifdef Tf71
    sendLNSensor(lncv[LNRueckBlockMelder], 1);
  #endif;
  #ifdef RB
    sendLNSwitch(lncv[LNRueckBlockMelder], 1, 1); //RB
  #endif;
  ringWecker();
}

/*
   anfangsfeld
*/

void handleAusfahrsignalFahrt() {
  /*
     when any ausfahrt to the outgoing track is enabled:
     - block from sending another train
     - Enable Wiederholsperrenmelder on panel
     - send a anstossmeldung on the block line
     - VorblockMelder (relaisblock) on
  */
  setAnfangsFeldState(fahrt_pre);
  delay(500);
  #ifdef Tf71
    sendLNSwitch(lncv[LNASpM], 1, 1); //Tf71
  #endif;
  #ifdef RB
    sendLNSwitch(lncv[LNWSpM], 1, 1); //RB
  #endif;
  setAnfangsFeldState(fahrt);
  sendLNSensor(lncv[LNVorblockMelder], 1);
  sendBlockMsg(ANSTOSSMELDUNG);
}

void handleAusfahrsignalHalt() {
  /*
     after the train has already reached the track detector on the ausfahrt and the signal is closed;
     if ausfahrt is enabled,
     - send Vorblock to DORT
     - all other info is already processed
  */
  setAnfangsFeldState(belegt);
  #ifdef RB
    sendLNSwitch(lncv[LNWSpM], 1, 0); //RB WSpM goes off after block is occupied
    sendLNSwitch(lncv[LNVorblockMelder], 1, 1); //RB
  #endif;
  setBlockState(BLOCK_OCC);
  sendBlockMsg(VORBLOCK);
}

void handleGleiskontaktAusfahrtBefahren() {
  setAnfangsFeldState(fluegel_kupplung);
  //setting the signals to stop is triggered by the contact but done by the panel
}

void handleAufloesenAusfahrt() {
  /*
     When the Ausfahrt is cancelled:
     - block is released for another train (from here)
     - Wiederholsperre is disabled
  */
  setAnfangsFeldState(frei);
  setBlockState(BLOCK_FREE);
  #ifdef Tf71
    sendLNSwitch(lncv[LNASpM], 1, 0); //Tf71
  #endif;
  #ifdef RB
    sendLNSwitch(lncv[LNWSpM], 1, 0); //RB
  #endif;
  sendLNSensor(lncv[LNVorblockMelder], 0);
 }

void handleVbHT_BlGTBedient() {
  /*
   * When VbHT-BlGT is pressed because Ausfahrt is not detected:
   * 
   * - do like the Ausfahrt is detected
   */
   setAnfangsFeldState(belegt);
   #ifdef Tf71
     sendLNSwitch(lncv[LNASpM], 1, 0); //Tf71
   #endif;
   #ifdef RB
     sendLNSwitch(lncv[LNWSpM], 1, 0); //RB
   #endif;
   setBlockState(BLOCK_OCC);
   sendBlockMsg(VORBLOCK);
}
void handleRbT_BlGT_Bedient() {
  /*
    When a Rueckblock LN message  is received and a train has actuelly arrived:
    - send a Rueckblock message to block
    - keep the direction inbound
    - set block free
    - display block free on LN2Block
    - send free message to LN
    - let a Wecker ring
  */
  #ifdef Tf71
    sendLNSensor(lncv[LNRaeumungsMelder], 0);
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNRaeumungsMelder], 1, 0); //RB
    sendLNSwitch(lncv[LNRueckBlockMelder], 1, 0); //RB
  #endif
  setEndFeldState(frei);
  sendBlockMsg(RUECKBLOCK);
  setBlockState(BLOCK_FREE);
  setBlockErlaubnis(ABGEGEBEN);
}

void handleErlaubnisAbgabeTasteBedient() {
  /*
     when the Erlaubnis Abgabe is pressed on the panel:
     - set direction inbound
     - set block free (should already be that way)
     - display block occupied on LN2Block
     - Let Panel know it cannot send
     - let a Wecker ring
  */
  sendBlockMsg(ERLAUBNISABGABE);
  setBlockState(BLOCK_FREE);
  setBlockErlaubnis(ABGEGEBEN);
  #ifdef Tf71
    sendLNSwitch(lncv[LNTastensperre_EaT], 1,0); //Block the EaT on the panel
  #endif
    #ifdef RB
    sendLNSwitch(lncv[LNTastensperre_EaT], 1,0); //Block the EaT on the panel
  #endif
  ringWecker();
}

/*************************************************************************/
/* All Block Nessages Events handling                                               */
/*************************************************************************/

void handleRueckblockEmpfangen() {
  /*
      Message of type RUECKBLOCK
     When a Block Rueckblock message is received:
     - return an ACK message
     - keep the direction inbound
     - set block free
     - display block free on LN2Block (set by direction)
     - send block free message to panel
     - let a Wecker ring
  */
  sendBlockMsg(RUECKBLOCK_ACK);
  setBlockErlaubnis(ERHALTEN);
  setBlockState(BLOCK_FREE);
  setAnfangsFeldState(frei);
  #ifdef Tf71
     sendLNSwitch(lncv[LNASpM], 1, 0); //Tf71
  #endif;
  #ifdef RB
    sendLNSwitch(lncv[LNWSpM], 1, 0);
    sendLNSwitch(lncv[LNVorblockMelder], 1, 0);
  #endif;
  ringWecker();
}

void handleErlaubnisEmpfangen() {
  /*
     Message of type ERLAUBNISABGABE
     When a Block ErlaubnisAbgabe message from the other station is received:
     - return an ACK message on Block
     - set direction outbound (Erlaubnis = ERHALTEN)
     - set block free (should already be that way)
     - Unlock Tastensperre EaT so we can give Erlaubnis back when needed
     - let a Wecker ring
  */
  sendBlockMsg(ERLAUBNISABGABE_ACK);
  setBlockErlaubnis(ERHALTEN);
  #ifdef Tf71
    sendLNSwitch(lncv[LNTastensperre_EaT], 1,1); //Unlock the EaT on the panel
    sendLNSwitch(lncv[LNErlaubnisAbgabeRichting], 1, 0);
    delay (100);
    sendLNSwitch(lncv[LNErlaubnisAbgabeRichting], 0, 0); //for compatiblity purposes with Digitrax Chief, also send an off message
  #endif
  #ifdef RB
    sendLNSwitch(lncv[LNTastensperre_EaT], 1,1); //Unlock the EaT on the panel
    sendLNSwitch(lncv[LNErlaubnisAbgabeRichting], 1, 0);
    delay (100);
    sendLNSwitch(lncv[LNErlaubnisAbgabeRichting], 0, 0); //for compatiblity purposes with Digitrax Chief, also send an off message
  #endif
  delay (200);
  ringWecker();
}

void handleErlaubnisAnfrage() {
  /*
      Message of type ERLAUBNISANFRAGE
     When the ErlaubisAnfrage from the other station is received:
     - send an ACK on Block
     - act like a ErlaubnisAbgabeTasteBedient is pressed
  */

  sendBlockMsg(ERLAUBNISANFRAGE_ACK);
  delay(200);
  handleErlaubnisAbgabeTasteBedient();
}

void handleReceivedEvent() {
  /*
     03-10-2016
     handle all events from panel and block, process the event based on the state of the block
  */
  if (receivedEvent > 0) {
    switch (blockErlaubnis) {
      case KEINER:
        switch (receivedEvent) {
          case ERLAUBNISABGABE:     //block message ErlaubnisAbgabe received from other station
            handleErlaubnisEmpfangen();
            break;
          case ERLAUBNISANFRAGE:    //blockmessage treated like an ErlaubnisAnfrage from other station
            if (anfangsfeldState == frei) {
              handleErlaubnisAnfrage();
            }
            break;
          case ErlaubnisAbgabeTasteBedient:
            if (anfangsfeldState == frei) {
              handleErlaubnisAbgabeTasteBedient();
            }
            break;
          default:
            break;
        }
        receivedEvent = 0;
        break;
      case HOLEN:
        switch (receivedEvent) {
          case ERLAUBNISABGABE:     //block message ErlaubnisAbgabe received from other station
            handleErlaubnisEmpfangen();
            break;
          default:
            sendBlockMsg(ERLAUBNISANFRAGE);
            blockErlaubnis = KEINER;
            delay(500);
            break;
        }
        receivedEvent = 0;
        break;
      case ERHALTEN:
        switch (receivedEvent) {
          case ERLAUBNISABGABE:       // do nothing with this block message as we already have Erlaubnis
            break;
          case ERLAUBNISANFRAGE:     //blockmessage treated like a abgabe
            if (anfangsfeldState == frei) {
              handleErlaubnisAnfrage();
              delay(500);
              handleErlaubnisAbgabeTasteBedient();
            }
            break;
          case RUECKBLOCK:
            if (anfangsfeldState == belegt) {
              handleRueckblockEmpfangen();
            }
            break;
          case AusfahrtSignalFahrt:
            if (anfangsfeldState == frei) {
              handleAusfahrsignalFahrt();
            }
            break;
          case AusfahrtSignalHalt:
            /*
               if tracksensor has been triggered, its an exit,
               otherwise a cancel of the ausfahrt
               This is not compliant with Block practices, but comes in handy when an Exit needs to be recalled
            */
            if (anfangsfeldState == fluegel_kupplung) {
              handleAusfahrsignalHalt();
            }
            else if (anfangsfeldState == fahrt) {
              handleAufloesenAusfahrt();
            }
            break;
          case GleiskontaktBefahren:
            if (anfangsfeldState == fahrt) {
              handleGleiskontaktAusfahrtBefahren();
            }
            break;
          case EinfahrtSignalFahrt:
            if (anfangsfeldState == fahrt) {
              handleEinfahrsignalFahrtAnfangsfeld();
            }
            break;
          case EinfahrtSignalHalt:
            if (anfangsfeldState == einfahrsignal) {
              handleEinfahrsignalHaltAnfangsfeld();
            }
            break;
          case ErlaubnisAbgabeTasteBedient:
            if (anfangsfeldState == frei) {
              handleErlaubnisAbgabeTasteBedient();
            }
            break;
          case VbHT_BlGTBedient:
            if (anfangsfeldState == fahrt) {
              handleVbHT_BlGTBedient();
            }
            break;
          default:
            break;
        }
        receivedEvent = 0;
        break;
      case ABGEGEBEN:
        switch (receivedEvent) {
          case ERLAUBNISABGABE:     //block message received from other station
            handleErlaubnisEmpfangen();
            break;
          case VORBLOCK:
            if ((endfeldState == frei) || (endfeldState == frei_boot)) {
              handleVorblockEmpfangen();
            
              if (EntranceSignalGo) // if Entrysignal already clear go directly to einfahrt
              {
                handleEinfahrsignalFahrt();
              }
            }
            break;
          case EinfahrtSignalFahrt:
            EntranceSignalGo = true;
            if (endfeldState == belegt) {
              handleEinfahrsignalFahrt();
            } 
            break;
          case EinfahrtSignalHalt:
            /*
               if tracksensor has been triggered, its an entrance, otherwise cancel entrance
            */
            EntranceSignalGo= false;
            if (endfeldState == erste_achse) {
              handleEinfahrsignalHalt();
            } else if (endfeldState == signal_gezogen) {
              handleEinfahrsignalZurueck(); //not Fremoblock standard, but very convenient
            }
            break;
          case GleiskontaktBefahren:
            if (endfeldState == signal_gezogen) {
              handleGleiskontaktEinfahrtBefahren();
              // and directly as we cannot detect last axle
              handleRaemungsmeldung();
            }
            break;
          case RbT_BlGT_Bedient:
            if ((endfeldState == geraeumt) || (endfeldState == frei_boot)) {
              handleRbT_BlGT_Bedient();
            }
            break;
          case AsT_RbT_Bedient:
            if (endfeldState == belegt) {
              handleAsT_RbT_Bedient();
            }
            break;
          case AsLT_RbT_Bedient:
            if (endfeldState == anschalter_aktiv) {
              handleAsLT_RbT_Bedient();
            }
            break;
          default:
            break;
        }
        receivedEvent = 0;
        break;
      default:
        break;
    }
  }
  #ifdef oledOn
    updateoled(7, receivedEvent);
  #endif
  receivedEvent = 0;
}

byte handleBlockMsg() {
  byte blockMsgReceived  = 0 ;
  blockMsgReceived = readBlockMsg();
  return blockMsgReceived;
}

boolean handleBlockCable() {
  //
  // when block line is active, block exit button
  //
  if ((digitalRead(RX_INPUT_PIN) == 1) && (digitalRead(RX2_INPUT_PIN) == 1)) {
    if (BlockConnected == false) {
      //first time, BlockConnected is still set to false, so set all flags

      digitalWrite(LED_GREEN_BLOCK_ACTIVE, HIGH);
      setUStoer(Off);
      //BlockConnected is now set to true;
      return true;
    }
  }
  else {
    // no block line active, don't block exit button
    if (BlockConnected == true) {
      //first time, BlockConnected is still set to true, so set all flags

      digitalWrite(LED_GREEN_BLOCK_ACTIVE, LOW);
      setUStoer(On);
      delay(600); //to avoid an on/off connection disturbing everything

      //BlockConnected is now set to false;
      return false;
    }
  }
}

void initBlockState () {
setAnfangsFeldState(frei);
  setBlockState(BLOCK_FREE);
  setBlockErlaubnis(HOLEN);
  sendBlockMsg(ERLAUBNISANFRAGE);
  currentMillis = millis();
  previousMillis = currentMillis;
  while((currentMillis - previousMillis) <= 1000) { //1000ms instead of 500
    if (Serial.available())
    {
      receivedEvent = handleBlockMsg(); //process Block messages
      if (receivedEvent == ERLAUBNISABGABE) {
        handleErlaubnisEmpfangen();
        previousMillis = previousMillis - 1000;
      }
    }
    currentMillis = millis();
  }
  if (blockErlaubnis == HOLEN) {
    setBlockErlaubnis(KEINER);
    setAnfangsFeldState(frei);
    sendLNSwitch(lncv[LNTastensperre_EaT], 1,1);
  }  
}
/*************************************************************************/
/* All init related stuff                                               */
/*************************************************************************/
void initBlockCable() {
  BlockConnected = false;
  Serial.begin(9600);
  pinMode(RX_INPUT_PIN, INPUT);  // block data connection
  pinMode(RX2_INPUT_PIN, INPUT); // block connection test

  delay(750); //so the other side can catch up
}

void knight_rider() {
  digitalWrite(LED_BLOCK_POWER_BLUE, HIGH);
  delay(50);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, LOW);
  delay(50);
  digitalWrite(LED_BLOCK_POWER_BLUE, LOW);
  delay(50);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, LOW);
  delay(50);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
  delay(50);
  digitalWrite(LED_GREEN_BLOCK_ACTIVE, LOW);
  delay(50);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, HIGH);
  delay(50);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, LOW);
  delay(50);
  digitalWrite(LED_GREEN_BLOCK_ACTIVE, HIGH);
  delay(50);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, LOW);
  delay(50);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, HIGH);
  delay(50);
  digitalWrite(LED_BLOCK_POWER_BLUE, HIGH);
  delay(50);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
  delay(50);
  digitalWrite(LED_BLOCK_POWER_BLUE, LOW);
  delay(50);
}

void initLed() {
  pinMode     (LED_BLOCK_POWER_BLUE, OUTPUT);
  pinMode     (LED_YELLOW_BLOCKING_HERE, OUTPUT);
  pinMode     (LED_RED_BLOCK_OCCUPIED, OUTPUT);
  pinMode     (LED_GREEN_BLOCK_ACTIVE, OUTPUT);

  // all off
  digitalWrite(LED_BLOCK_POWER_BLUE, HIGH);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, HIGH);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, HIGH);
  digitalWrite(LED_GREEN_BLOCK_ACTIVE, HIGH);

  knight_rider();

  // set intital state, block on: on, rest off
  digitalWrite(LED_BLOCK_POWER_BLUE, HIGH);
  digitalWrite(LED_RED_BLOCK_OCCUPIED, LOW);
  digitalWrite(LED_YELLOW_BLOCKING_HERE, LOW);
  digitalWrite(LED_GREEN_BLOCK_ACTIVE, LOW);
}
