/*************************************************************************/
/*          EEPROM FUNCTIONS                                             */
/*************************************************************************/
void saveLNCVSettings() {
  int i = 0;

  EEPROM.write(0, VERSION);

  for (i = 0; i < LNCV_COUNT; i++)
  {
    EEPROM.write(i * 2 + 2, highByte(lncv[i]));
    EEPROM.write(i * 2 + 3, lowByte(lncv[i]));
  }
}

void resetLNCVSettings() {
#ifdef DEBUG
  Serial.print("RESET MODULE CONFIGURATION");
#endif
  lncv[0] = 1;      //address
  lncv[1] = 0;      //Schluessel
  lncv[2] = 0;     //Stör
  lncv[3] = 0;      //Schluessel
  lncv[4] = 0;    //ASpM (Tf71)
  lncv[5] = 0;     //BlockMelder (Tf71)
  lncv[6] = 0;   //WSpM (WiederholsperrenMelder) (RB)
  lncv[7] = 0;    //VbM Vorblockmelder (RB)
  lncv[8] = 0;    //RbM Rückblockmelder (RB)
  lncv[9] = 0;    //AsM (RB)
  lncv[10] = 1;     //not_used in DB
  lncv[11] = 0;  //RM (RB)
  lncv[12] = 0;   //Wecker
  lncv[13] = 0;    //LNTastensperre_EaT
  lncv[14] = 0;    //LNErlaubnisAbgabeRichting
    //input LN sensor messages
  lncv[15] = 0;    //ESig
  lncv[16] = 0;    //ASig
  lncv[17] = 0;    //LNTrackSensor
  lncv[18] = 0;    //LNTrackSensorOutgoing
  lncv[19] = 0;   //BlGT-RBT
  lncv[20] = 0;    //BlGT-VbHT
  lncv[21] = 0;    //BlGT-EaT
  lncv[22] = 0;    //AsT-RBT
  lncv[23] = 0;    //AsLT-RBT

  saveLNCVSettings();
}

void loadLNCVSettings() {
  int i = 0;

  //Check if there is no configuration stored or major version changed to reset
  if (EEPROM.read(0) != VERSION)
  {
    resetLNCVSettings();
    return;
  }

  for (i = 0; i < LNCV_COUNT; i++)
  {
    lncv[i] = word(EEPROM.read(i * 2 + 2), EEPROM.read(i * 2 + 3));
  }
  if (lncv[0] == 0) resetLNCVSettings();
}

/*************************************************************************/
/*          LNCV LOCONET PROGRAMMING FUNCTIONS                           */
/*************************************************************************/
void commitLNCVUpdate() {
  saveLNCVSettings();
}

int8_t notifyLNCVread(uint16_t ArtNr, uint16_t lncvAddress, uint16_t, uint16_t & lncvValue) {
  // Step 1: Can this be addressed to me?
  // All ReadRequests contain the ARTNR. For starting programming, we do not accept the broadcast address.
  if (modeProgramming)
  {
    if (ArtNr == ARTNR)
    {
      if (lncvAddress < LNCV_COUNT)
      {
        lncvValue = lncv[lncvAddress];
        return LNCV_LACK_OK;
      }
      else
      {
        // Invalid LNCV address, request a NAXK
        return LNCV_LACK_ERROR_UNSUPPORTED;
      }
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
}

int8_t notifyLNCVprogrammingStart(uint16_t & ArtNr, uint16_t & ModuleAddress) {
  // Enter programming mode. If we already are in programming mode,
  // we simply send a response and nothing else happens.

  if (ArtNr == ARTNR)
  {
    if (ModuleAddress == lncv[0])
    {
      modeProgramming = true;
      return LNCV_LACK_OK;
    }
    else if (ModuleAddress == 0xFFFF)
    {
      ModuleAddress = lncv[0];
      return LNCV_LACK_OK;
    }
  }
  return -1;
}

int8_t notifyLNCVwrite(uint16_t ArtNr, uint16_t lncvAddress, uint16_t lncvValue) {

  int n = 0;

  if (!modeProgramming)
  {
    return -1;
  }
  if (ArtNr == ARTNR)
  {
    //Valid LNCV number
    if (lncvAddress < LNCV_COUNT)
    {
      lncv[lncvAddress] = lncvValue;
      return LNCV_LACK_OK;
    }
    else
    {
      return LNCV_LACK_ERROR_UNSUPPORTED;
    }
  }
  else
  {
    return -1;
  }
}

void notifyLNCVprogrammingStop(uint16_t ArtNr, uint16_t ModuleAddress) {
  if (modeProgramming)
  {
    if (ArtNr == ARTNR && ModuleAddress == lncv[0])
    {
      modeProgramming = false;
      commitLNCVUpdate();
    }
    else
    {
      if (ArtNr != ARTNR)
      {
        return;
      }
      if (ModuleAddress != lncv[0])
      {
        return;
      }
    }
  }
}
