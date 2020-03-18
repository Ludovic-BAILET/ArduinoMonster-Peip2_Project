void buttonUpdate() {
  if (digitalRead(15)==0) {
    startPressed = true;
  }

  if (digitalRead(19)==0) {
    if (previousBackPressed<0) {backPressed = true; previousBackPressed = 10000;}
  }
  else {
    previousBackPressed-=1;
  }

  if (digitalRead(18)==0) {
    if (previousActionPressed<0) {actionPressed = true; previousActionPressed = 10000;}
  }
  else {
    previousActionPressed-=1;
  }

  if (digitalRead(20)==0) leftPressed = true;
  else if (digitalRead(21)==0) rightPressed = true;
  else if (digitalRead(22)==0) upPressed = true;
  else if (digitalRead(23)==0) downPressed = true;

  if (leftPressed==true) {
    if (leftPressed != previousLeftPressed && leftPressed && chatBoxTimer>500*125) {
      if ((battlePhase>0 && battlePhase<=11) || battlePhase==517 || battlePhase==527) {if (pointer>0) {pointer = (pointer-1)%4;} else {pointer=3;}}
    }
  }
     
  else if (rightPressed==true) {
    if (rightPressed != previousRightPressed && rightPressed && chatBoxTimer>500*125) {
      if ((battlePhase>0 && battlePhase<=11) || battlePhase==517 || battlePhase==527) {pointer = (pointer+1)%4;}
    }
  }
  
  else if (upPressed==true) {
    if (upPressed != previousUpPressed && upPressed && chatBoxTimer>500*125) {
      if ((battlePhase>0 && battlePhase<=11) || battlePhase==517 || battlePhase==527) {pointer = (pointer+2)%4;}
      if (battlePhase==211 || battlePhase==33 || battlePhase==33+49 || (battlePhase>=232 && battlePhase<=234)) {
        if (battlePhase>=232 && battlePhase<=234) {pointer = (pointer-1)%6;if (pointer==-1) pointer=myTeam.getNombrePokemon()-1;}
        else if (pointer!=1) pointer = (pointer-1)%6;
        else pointer = myTeam.getNombrePokemon()-1;
      }
      if (battlePhase==231 || menuPhase==3) {
        if (pointer>5 && pointer%5==0) {Serial.println("FAIRE LE SLIDING D'OBJETS DANS LE SAC");}///////////////FAIRE LE SLIDING D'OBJETS DANS LE SAC///////////////
        if (pointer>0) pointer-=1;
      }
      if (battlePhase==513 || battlePhase==523) {pointer = (pointer-1)%2;if (pointer<0) pointer+=2;}
      if (menuPhase==1) {pointer = (pointer-1)%5;if (pointer<0) pointer+=6;}
      if (menuPhase==2 || (menuPhase>=9 && menuPhase<=16) || (menuPhase>=21 && menuPhase<=23)) {pointer = (pointer-1)%myTeam.getNombrePokemon();if (pointer<0) pointer+=myTeam.getNombrePokemon();}
    }
  }
  
  else if (downPressed == true) {
    if (downPressed != previousDownPressed && downPressed && chatBoxTimer>1000*125) {
      if ((battlePhase>0 && battlePhase<=11) || battlePhase==517 || battlePhase==527) {pointer = (pointer+2)%4;}
      if (battlePhase==211 || battlePhase==33 || battlePhase==33+49 || (battlePhase>=232 && battlePhase<=234)) {
        if (battlePhase>=232 && battlePhase<=234) {if (pointer!=myTeam.getNombrePokemon()-1) {pointer = (pointer+1)%6;} else pointer = 0;}
        else if (pointer!=(myTeam.getNombrePokemon()-1)) pointer = (pointer+1)%6;
        else pointer = 1;
      } 
      if (battlePhase==231 || menuPhase==3) {
        int numberOfDifferentItems = getNumberOfDifferentItems()-1;
        if (pointer%5==4 && pointer<numberOfDifferentItems) {Serial.println("FAIRE LE SLIDING D'OBJETS DANS LE SAC");}///////////////FAIRE LE SLIDING D'OBJETS DANS LE SAC///////////////
        if (pointer<numberOfDifferentItems) pointer+=1;
      }
      if (battlePhase==513 || battlePhase==523) pointer = (pointer+1)%2;
      if (menuPhase==1) pointer = (pointer+1)%6;
      if (menuPhase==2 || (menuPhase>=9 && menuPhase<=16) || (menuPhase>=21 && menuPhase<=23)) {pointer = (pointer+1)%myTeam.getNombrePokemon();}
    }
  }

  if (Serial.available()) {
    String input = Serial.readString();
    if (input[0]=='/' && input[1]=='h' && input[2]=='e' && input[3]=='a' && input[4]=='l') {
      for (int e=0;e<myTeam.getNombrePokemon();e++) myTeam.pokemons[e].setActualStats(0,myTeam.pokemons[e].getMaxStats(0));
      Serial.println("L'équipe a été soignée.");
    }
    if (input[0]=='/' && input[1]=='s' && input[2]=='a' && input[3]=='v' && input[4]=='e' && input.length()>5) {
      String number = input[5];
      for (unsigned int e=6;e<number.length();e++) number+=input[e];
      save(number.toInt());
      Serial.print("La progression est sauvegardée à l'emplacement ");Serial.println(number.toInt());
    }
    if (input[0]=='/' && input[1]=='l' && input[2]=='o' && input[3]=='a' && input[4]=='d' && input.length()>5) {
      String number = input[5];
      for (unsigned int e=6;e<number.length();e++) number+=input[e];
      load(number.toInt());
      Serial.print("La progression est chargée depuis l'emplacement ");Serial.println(number.toInt());
    }
    if (input[0]=='x') {
      //Serial.print("Pointer = ");Serial.println(pointer);
      //Serial.print("PreviousPointer = ");Serial.println(previousPointer);
      //Serial.print("MenuPhase = ");Serial.println(menuPhase);
      //myTeam.pokemons[0].setXp(4500);
      //blueSerial.print("Yo les gars c'est 123456789\r\n");
      //Serial.println(myTeam.pokemons[0].getNextLvlXp());
    }
  }
  previousLeftPressed = leftPressed;previousRightPressed = rightPressed;previousUpPressed = upPressed;previousDownPressed = downPressed;
}

void action() {
  if (actionPressed && not(actionLocked) && battlePhase==0 && menuPhase==0) {
    int coeffX=0, coeffY=0;
    switch(Peter_dir) {
      case 0:coeffX=-1;break;
      case 1:coeffX=1;break;
      case 2:coeffY=-1;break;
      case 3:coeffY=1;break;
      default:break;
    }

    for (int e=0;e<objectNumber;e++) {
      if (objetOverworld[e].getX()==Peter_x+coeffX && objetOverworld[e].getY()==Peter_y+coeffY && objetOverworld[e].getVisible() && objetOverworld[e].getArea()==Peter_area) {
        objetOverworld[e].setVisible(false);
        changeMap(Peter_area);
        chatBoxText = myTeam.name;chatBoxText+=" obtient ";chatBoxText+=objetOverworld[e].getNumber();chatBoxText+=" ";
        chatBoxText+=objetOverworld[e].getName();chatBoxText+=" !";chatBoxStartUse = 0;activateChatBox(chatBoxText);
        Peter_items[objetOverworld[e].getID()]+=objetOverworld[e].getNumber();  
      }
    }

    for (int e=0;e<PNJNumber;e++) {
      if (PnjOverworld[e].getX()==Peter_x+coeffX && PnjOverworld[e].getY()==Peter_y+coeffY && PnjOverworld[e].getArea()==Peter_area) {
        if (Peter_dir==0) PnjOverworld[e].setDir(1);
        if (Peter_dir==1) PnjOverworld[e].setDir(0);
        if (Peter_dir==2) PnjOverworld[e].setDir(3);
        if (Peter_dir==3) PnjOverworld[e].setDir(2);
        String fileName = "TO";
        fileName+=PnjOverworld[e].getID();
        fileName+=PnjOverworld[e].getDir();
        fileName+=".txt";
        modifyOneSprite(PnjOverworld[e].getX()*16, PnjOverworld[e].getY()*16-4, 16, 20, fileName);
        PeterPlace(Peter_dir);
        if (PnjOverworld[e].getBeaten()<2) {
          myTeam.startBattleAgainstTrainer(PnjOverworld[e].getID());
          PnjOverworld[e].setBeaten(PnjOverworld[e].getBeaten()+1);
        }
        else {
          chatBoxText = trainersLoosingText[e];
          activateChatBox(chatBoxText);
        }
      }
    }
    
    if (COLLISION[Peter_y+coeffY][Peter_x+coeffX]>=10 && COLLISION[Peter_y+coeffY][Peter_x+coeffX]<100) {
      switch(COLLISION[Peter_y+coeffY][Peter_x+coeffX]) {
        case 10:
          activateChatBox("Mdr tu parles aux pancartes t'as un probleme toi");
          break;
        case 11:
          activateChatBox("Vous pouvez absolument tout reproduire sur la console portable ! :D");
          //activateChatBox("Foret de Iannis, ne t'y aventure pas sans Arduimon !");
          break;
        case 12:
          activateChatBox("Residence de Quentin, ne pas entrer !");
          break;
      }
    }
  }
}

void changeMapUpdate() {
  switch(COLLISION[Peter_y][Peter_x]) {
    case 200 :
      if (Peter_dir==0) {
        Peter_x = 24;
        Peter_area = 0;
        changeMap(0);
      }
      break;
    case 201 :
      if (Peter_dir==1) {
        Peter_x = 0;
        Peter_area = 1;
        changeMap(1);
      }
      break;
    default:return;
  }
}

void save(int switcher) {
  String save = "";String fileName = "SAVE";fileName+=switcher;
  char charFileName [(fileName).length()];
  fileName.toCharArray(charFileName,fileName.length()+1);
  if (SD.exists(charFileName)) SD.remove(charFileName);
  File fichierSAVE = SD.open(charFileName, FILE_WRITE);

  save+=Peter_x;save+=",";
  save+=Peter_y;save+=",";
  save+=Peter_dir;save+=",";
  save+=Peter_area;save+=",";

  for (int e=0;e<objectNumber;e++) {
    save+=Peter_items[e];
    save+=",";
  }

  save+="\n";

  for (int e=0;e<myTeam.getNombrePokemon();e++) {
    save+=myTeam.pokemons[e].getID();save+=",";
    save+=myTeam.pokemons[e].getLvl();save+=",";
    save+=myTeam.pokemons[e].getXp();save+=",";
    save+=myTeam.pokemons[e].getActualStats(0);save+=",";
    save+=myTeam.pokemons[e].getMove(0).getID();save+=",";
    save+=myTeam.pokemons[e].getMove(1).getID();save+=",";
    save+=myTeam.pokemons[e].getMove(2).getID();save+=",";
    save+=myTeam.pokemons[e].getMove(3).getID();save+=",";
    save+=myTeam.pokemons[e].getStatut(0).getID();save+=",";
    save+="\n";
  }

  Serial.print(save);
  fichierSAVE.print(save);

  fichierSAVE.close();
}

void load(int switcher) {
  int previousArea = Peter_area;
  String save = "";String fileName = "SAVE";fileName+=switcher;
  char charFileName [(fileName).length()];
  fileName.toCharArray(charFileName,fileName.length()+1);
  File fichierSAVE = SD.open(charFileName, FILE_READ);

  Peter_x = fichierSAVE.readStringUntil(',').toInt();
  Peter_y = fichierSAVE.readStringUntil(',').toInt();
  Peter_dir = fichierSAVE.readStringUntil(',').toInt();
  Peter_area = fichierSAVE.readStringUntil(',').toInt();

  for (int e=0;e<objectNumber;e++) {
    Peter_items[e] = fichierSAVE.readStringUntil(',').toInt();
  }

  for (int e=0;e<myTeam.getNombrePokemon();e++) {
    myTeam.pokemons[e] = Pokemon(fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt(),
                                 fichierSAVE.readStringUntil(',').toInt());
  }
  if (Peter_area!=previousArea) changeMap(Peter_area);
  refresh = true;
  fichierSAVE.close();
}

void sendCommand(int8_t command, int16_t dat) {
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for(uint8_t i=0; i<8; i++) {
    MP3Serial.write(Send_buf[i]) ;//send bit to serial mp3
    Serial.print(Send_buf[i],HEX);//send bit to serial monitor in pc
  }
  Serial.println("");
}

void playMusic() {
  if (musicID!=0x0000) {
    sendCommand(CMD_PLAY_WITHVOLUME, musicID);
    musicID = 0x0000;
  }
}
