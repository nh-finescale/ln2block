#ifdef oledOn
/*************************************************************************/
/* All OLED related stuff                                               */
/*************************************************************************/
void clearOledField(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1 ) {
  uint8_t wide = oled.fontWidth()+1;
  uint8_t high = oled.fontHeight()+1;
  // oled.clear (c_start, c_end, Row_start, row_end)
  
  oled.clear((c0*wide),(c1*wide),(r0*high),(r1*high));
  }

void initoled() {

  oled.begin(&Adafruit128x64, I2C_ADDRESS);

  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(Adafruit5x7);
}

void labeloled() {

  //fill the oled with the labels
  //0-12345678901234567890123
  //1 Cable : X      LN : X
  //2 State : X    PROG : X
  //3 =======================
  //4 AnfFld : X  EndFld : X
  //5 
  //6 Event  : XXX 
  //7
  //8
  //
  
  clearOledField(1,8,1,1);
  oled.print("Cable : ");
<<<<<<< .mine
  oled.print(BlockConnected);
  oled.println("    LN: OK");
  oled.print("State : ");
=======
  clearOledField(16,20,1,1);
  oled.println("  LN: ");
  clearOledField(1,8,2,2);
  oled.print("State :");
>>>>>>> .r45
  clearOledField(14,20,2,2);
  oled.print("    PROG:");
  clearOledField(1,24,3,3);
  oled.println("========================");
  clearOledField(1,8,4,4);
  oled.print("AnfFld: ");
  clearOledField(13,21,4,4);
  oled.print("EndFeldstate: ");
  clearOledField(1,8,6,6);
  oled.print("Event : ");
}

void updateoled(uint8_t field, uint32_t value){
  //fill the oled with the labels
  //0-12345678901234567890123
  //1 Cable : X      LN : XX
  //2 State : X    PROG : X
  //3 =======================
  //4 AnfFld : X  EndFld : X
  //5 
  //6 Event  : XXX 
  //7
  //8
  switch (field) {
    case 0 :
        //reset all field: TODO
        break;
    case 1 :
        //cable
        clearOledField(9,9,1,1);
        oled.print(field); 
        break;
    case 2 :
        //LN
        clearOledField(21,22,1,1);
        oled.print("OK");
        break;
    case 3 :
        //Block State
        clearOledField(9,9,2,2);
        oled.print(field);
        break;
    case 4 :
        //PROGRAMMING STATE
        clearOledField(21,22,2,2);
        if (modeProgramming) {oled.print("ON");} else {oled.print("OF");}
        break;
    case 5 :
        //Anfangfeldstate
        clearOledField(10,10,4,4);
        oled.print(field);
        break;
    case 6 :
        //Endfeldstate
        clearOledField(22,22,4,4);
        oled.print(field);
        break;
    case 7 :
        //Received event
        clearOledField(10,12,6,6);
        oled.print(field);
        break;
    default:
        break;
  } 
}
#endif
