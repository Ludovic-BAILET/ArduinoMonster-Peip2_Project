void drawSDImage(int x, int y, String fileName, int h, int w) {
  char charFileName [fileName.length()];
  fileName.toCharArray(charFileName,fileName.length()+1);
  File fichierIMAGE = SD.open(charFileName);
  uint32_t startTime = millis();
  int row, col, count=0;
  uint16_t awColors[w];

  for (row=0; row<h; row++) { // For each scanline...
    for (col=0; col<w; col++) { // For each pixel...
      char pixel[6] = "0x";
      for (int i=0;i<4;i++) {pixel[2+i]=fichierIMAGE.read();}
      awColors[count] = strtol(pixel,NULL,16);
      count++;
    }
    fichierIMAGE.read();
    fichierIMAGE.read();
    count = 0;
    tft.writeRect(x, row+y, w, 1, awColors);
  }
  fichierIMAGE.close();
  Serial.print(fileName);
  Serial.print(" loaded in ");
  Serial.println(millis() - startTime);
}

void drawSDDetouredImage(int x, int y, String fileName, int h, int w) {
  char charFileName [fileName.length()];
  fileName.toCharArray(charFileName,fileName.length()+1);
  File fichierIMAGE = SD.open(charFileName);
  uint32_t startTime = millis();
  int row, col;

  for (row=0; row<h; row++) { // For each scanline...
    for (col=0; col<w; col++) { // For each pixel...
      char pixel[6] = "0x";
      for (int i=0;i<4;i++) {pixel[2+i]=fichierIMAGE.read();}
      if (strtol(pixel,NULL,16)!=0xf815) tft.drawPixel(x+col, y+row, strtol(pixel,NULL,16));
    }
    fichierIMAGE.read();
    fichierIMAGE.read();
  }
  fichierIMAGE.close();
  Serial.print(fileName);
  Serial.print(" loaded in ");
  Serial.println(millis() - startTime);
}

void drawArduimonOrTrainer(int ArduimonOrTrainerID, uint8_t x, uint16_t y, int switcher) {
  String fileName = "";
  
  if (switcher<2) fileName+="A";
  if (switcher==2) fileName+="T";
  if (switcher==3) fileName+="I";

  fileName+=ArduimonOrTrainerID;

  if (switcher==-3) fileName+="4";
  if (switcher==-2) fileName+="3";
  if (switcher==-1) fileName+="2";
  if (switcher==0) fileName+="1";
  if (switcher==1) fileName+="0";

  fileName+=".txt";

  if (switcher!=-1 && switcher!=3) drawSDDetouredImage(x, y, fileName, 100, 100);
  else if (switcher==3) drawSDDetouredImage(x, y, fileName, 20, 20);
  else drawSDDetouredImage(x, y, fileName, 25, 25);
}

void modifyOneSprite(int x16, int y16, int w, int h, String fileName) {
  int buffidx = 400*y16;
  char charFileName [fileName.length()];
  fileName.toCharArray(charFileName,fileName.length()+1);
  File fichierIMAGE = SD.open(charFileName);

  for (int i=0;i<h;i++) {
    buffidx+=x16;
    for (int j=0;j<w;j++) {
      char pixel[6] = "0x";
      for (int i=0;i<4;i++) {pixel[2+i]=fichierIMAGE.read();}
      uint16_t pixels = strtol(pixel,NULL,16);
      if (strtol(pixel,NULL,16)!=0xf815) MAP[buffidx] = pixels;
      buffidx++;
    }
    fichierIMAGE.read();
    fichierIMAGE.read();
    buffidx+=(400-w-x16);
  }
}

void addItemsOnMap() {
  for (int e=0;e<objectNumber;e++) {
    if (objetOverworld[e].getVisible() && objetOverworld[e].getArea()==Peter_area) {
      modifyOneSprite(objetOverworld[e].getX()*16, objetOverworld[e].getY()*16, 16, 16, "IOVERW.txt");
      COLLISION[objetOverworld[e].getY()][objetOverworld[e].getX()] = 1;
    }
  }
}

void addPNJOnMap() {
  for (int e=0;e<PNJNumber;e++) {
    if (PnjOverworld[e].getArea()==Peter_area) {
      String fileName = "TO";
      fileName+=PnjOverworld[e].getID();
      fileName+=PnjOverworld[e].getDir();
      fileName+=".txt";
      modifyOneSprite(PnjOverworld[e].getX()*16, PnjOverworld[e].getY()*16-4, 16, 20, fileName);
      COLLISION[PnjOverworld[e].getY()][PnjOverworld[e].getX()] = 1;
    }
  }
}

void changeMap(int mapID) {
  uint32_t startTime = millis();
  File fichierMAP;
  uint32_t count=0;

  switch(mapID) {
    case 0:fichierMAP = SD.open("MAP0.txt");break;
    case 1:fichierMAP = SD.open("MAP1.txt");break;
    default :return;
  }
  
  for (int i=0;i<400;i++) { //Pour chaque ligne     ////////////////////////////////////////////////////////////////////
    for (int j=0;j<400;j++) { //Pour chaque pixels d'une ligne     ////////////////////////////////////////////
      char pixel[6] = "0x";
      for (int i=0;i<4;i++) {pixel[2+i]=fichierMAP.read();}
      MAP[count] = strtol(pixel,NULL,16);
      count++;
    }
    fichierMAP.read();fichierMAP.read();
  }
  
  refresh = true;

  fichierMAP.close();
  Serial.print("Map loaded in ");
  Serial.println(millis() - startTime);

  changeCollision(mapID);
  addItemsOnMap();
  addPNJOnMap();
}

void changeCollision(int collisionID) {
  uint32_t startTime = millis();
  File fichierCOL;

  switch(collisionID) {
    case 0:fichierCOL = SD.open("COL0.txt");break;
    case 1:fichierCOL = SD.open("COL1.txt");break;
    default :return;
  }

  for (int i=0;i<25;i++) { //Pour chaque lignes
    for (int j=0;j<25;j++) { //Pour chaque tiles d'une ligne
      String colliString = fichierCOL.readStringUntil(',');
      COLLISION [i][j] = colliString.toInt();
    }
  }  

  Serial.print("Collisions changed in ");
  Serial.println(millis() - startTime);
}

void drawMap(int Peter_x16, int Peter_y16, uint16_t PETER []) {
  uint32_t startTime = millis();
  const int taille=400;
  int x,y;
  int row, col, buffidx;
  uint16_t awColors[240];

  if (Peter_x16<=112) {
    x=0;
    Peter_x16-=2;
  }
  else if (Peter_x16>=272) {
    x=-160; ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Peter_x16=-160+Peter_x16-2;
  }
  else {
    x=112-Peter_x16;
    Peter_x16 = 112-2;
  }
  if (Peter_y16<=160) {
    y=0;
    Peter_y16=Peter_y16-4;
  }
  else if (Peter_y16>=240) {
    y=-80; /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Peter_y16=-80+Peter_y16-4;
  }
  else {
    y=160-Peter_y16;
    Peter_y16=160-4;
  }
  
  buffidx = taille*(-y); ///////////////////////////////////////////////////////////////////////////////////////////////////////
  for (row=0; row<320; row++) { // For each scanline...
    for (col=0; col<taille; col++) { // For each pixel...
      if (col>=-x && col<-x+240) {
        awColors[col+x] = pgm_read_word(MAP + buffidx);
      }
      buffidx++;
    }
    tft.writeRect(0, row, 240, 1, awColors);
    if (row>=Peter_y16 && row<=Peter_y16+20 && (row)%2==0) {
      drawRGBDetouredBitmap(Peter_x16, Peter_y16, PETER, 20, 20);
    }
  }

  Serial.print("Map loaded in ");
  Serial.println(millis() - startTime);
}

void drawBackground(int backgroundID) {
  uint32_t startTime = millis();
  int row, col, buffidx=0;
  uint16_t awColors[240];

  for (row=0; row<320; row++) { // For each scanline...
    for (col=0; col<240; col++) { // For each pixel...
      switch(backgroundID) {
        case 0:
          awColors[col] = pgm_read_word(BATTLE_BACKGROUND1 + buffidx);
          break;
        case 1:
          awColors[col] = pgm_read_word(BATTLE_BACKGROUND1 + buffidx);
          break;
        default:return;
      }
      buffidx++;
    }
    tft.writeRect(0, row, 240, 1, awColors);
  }

  Serial.print("Background loaded in ");
  Serial.println(millis() - startTime);
}

void PeterMove(int dir) {
  if (Peter_move) {
    if (Peter_dir == dir) {
      switch(dir) {
        case 0:
          if (COLLISION[Peter_y][Peter_x-1]==0 || COLLISION[Peter_y][Peter_x-1]==2 || COLLISION[Peter_y][Peter_x-1]>=200) {
            if (COLLISION[Peter_y][Peter_x-1]==2) {
              drawMap(Peter_x*16-2, Peter_y*16, PETER_GRASS1);drawMap(Peter_x*16-4, Peter_y*16, PETER_GRASS1);
              drawMap(Peter_x*16-6, Peter_y*16, PETER_GRASS0);drawMap(Peter_x*16-8, Peter_y*16, PETER_GRASS0);
              drawMap(Peter_x*16-10, Peter_y*16, PETER_GRASS1);drawMap(Peter_x*16-12, Peter_y*16, PETER_GRASS1);
              drawMap(Peter_x*16-14, Peter_y*16, PETER_GRASS0);drawMap(Peter_x*16-16, Peter_y*16, PETER_GRASS0);
              Peter_x-=1;
            }
            else {
              drawMap(Peter_x*16-2, Peter_y*16, PETER1);drawMap(Peter_x*16-4, Peter_y*16, PETER1);
              drawMap(Peter_x*16-6, Peter_y*16, PETER0);drawMap(Peter_x*16-8, Peter_y*16, PETER0);
              drawMap(Peter_x*16-10, Peter_y*16, PETER1);drawMap(Peter_x*16-12, Peter_y*16, PETER1);
              drawMap(Peter_x*16-14, Peter_y*16, PETER0);drawMap(Peter_x*16-16, Peter_y*16, PETER0);
              Peter_x-=1;
            }
          }
          break;
        case 1:
          if (COLLISION[Peter_y][Peter_x+1]==0 || COLLISION[Peter_y][Peter_x+1]==2 || COLLISION[Peter_y][Peter_x+1]>=200) {
            if (COLLISION[Peter_y][Peter_x+1]==2) {
              drawMap(Peter_x*16+2, Peter_y*16, PETER_GRASS3);drawMap(Peter_x*16+4, Peter_y*16, PETER_GRASS3);
              drawMap(Peter_x*16+6, Peter_y*16, PETER_GRASS2);drawMap(Peter_x*16+8, Peter_y*16, PETER_GRASS2);
              drawMap(Peter_x*16+10, Peter_y*16, PETER_GRASS3);drawMap(Peter_x*16+12, Peter_y*16, PETER_GRASS3);
              drawMap(Peter_x*16+14, Peter_y*16, PETER_GRASS2);drawMap(Peter_x*16+16, Peter_y*16, PETER_GRASS2);
              Peter_x+=1;
            }
            else {
              drawMap(Peter_x*16+2, Peter_y*16, PETER3);drawMap(Peter_x*16+4, Peter_y*16, PETER3);
              drawMap(Peter_x*16+6, Peter_y*16, PETER2);drawMap(Peter_x*16+8, Peter_y*16, PETER2);
              drawMap(Peter_x*16+10, Peter_y*16, PETER3);drawMap(Peter_x*16+12, Peter_y*16, PETER3);
              drawMap(Peter_x*16+14, Peter_y*16, PETER2);drawMap(Peter_x*16+16, Peter_y*16, PETER2);
              Peter_x+=1;
            }
          }
          break;
        case 2:
          if (COLLISION[Peter_y-1][Peter_x]==0 || COLLISION[Peter_y-1][Peter_x]==2 || COLLISION[Peter_y-1][Peter_x]>=200) {
            if (COLLISION[Peter_y-1][Peter_x]==2) {
              drawMap(Peter_x*16, Peter_y*16-2, PETER_GRASS5);drawMap(Peter_x*16, Peter_y*16-4, PETER_GRASS5);
              drawMap(Peter_x*16, Peter_y*16-6, PETER_GRASS4);drawMap(Peter_x*16, Peter_y*16-8, PETER_GRASS4);
              drawMap(Peter_x*16, Peter_y*16-10, PETER_GRASS6);drawMap(Peter_x*16, Peter_y*16-12, PETER_GRASS6);
              drawMap(Peter_x*16, Peter_y*16-14, PETER_GRASS4);drawMap(Peter_x*16, Peter_y*16-16, PETER_GRASS4);
              Peter_y-=1;
            }
            else {
              drawMap(Peter_x*16, Peter_y*16-2, PETER5);drawMap(Peter_x*16, Peter_y*16-4, PETER5);
              drawMap(Peter_x*16, Peter_y*16-6, PETER4);drawMap(Peter_x*16, Peter_y*16-8, PETER4);
              drawMap(Peter_x*16, Peter_y*16-10, PETER6);drawMap(Peter_x*16, Peter_y*16-12, PETER6);
              drawMap(Peter_x*16, Peter_y*16-14, PETER4);drawMap(Peter_x*16, Peter_y*16-16, PETER4);
              Peter_y-=1;
            }
          }
          break;
        case 3:
          if (COLLISION[Peter_y+1][Peter_x]==0 || COLLISION[Peter_y+1][Peter_x]==2 || COLLISION[Peter_y+1][Peter_x]>=200) {
            if (COLLISION[Peter_y+1][Peter_x]==2) {
              drawMap(Peter_x*16, Peter_y*16+2, PETER_GRASS8);drawMap(Peter_x*16, Peter_y*16+4, PETER_GRASS8);
              drawMap(Peter_x*16, Peter_y*16+6, PETER_GRASS7);drawMap(Peter_x*16, Peter_y*16+8, PETER_GRASS7);
              drawMap(Peter_x*16, Peter_y*16+10, PETER_GRASS9);drawMap(Peter_x*16, Peter_y*16+12, PETER_GRASS9);
              drawMap(Peter_x*16, Peter_y*16+14, PETER_GRASS7);drawMap(Peter_x*16, Peter_y*16+16, PETER_GRASS7);
              Peter_y+=1;
            }
            else {
              drawMap(Peter_x*16, Peter_y*16+2, PETER8);drawMap(Peter_x*16, Peter_y*16+4, PETER8);
              drawMap(Peter_x*16, Peter_y*16+6, PETER7);drawMap(Peter_x*16, Peter_y*16+8, PETER7);
              drawMap(Peter_x*16, Peter_y*16+10, PETER9);drawMap(Peter_x*16, Peter_y*16+12, PETER9);
              drawMap(Peter_x*16, Peter_y*16+14, PETER7);drawMap(Peter_x*16, Peter_y*16+16, PETER7);
              Peter_y+=1;
            }
          }
          break;
        default:return;
      }
      if (COLLISION[Peter_y][Peter_x] == 2 && randomInt(16)==0) myTeam.startBattleAgainstWildArduimon();
    }
    else {
      Peter_dir = dir;
      PeterPlace(Peter_dir);
    }
  }
}

void drawPeterMove() {
  if (leftPressed) PeterMove(0);
  if (rightPressed) PeterMove(1);
  if (upPressed) PeterMove(2);
  if (downPressed) PeterMove(3);
}

void PeterPlace(int dir) {
  switch(dir) {
    case 0:
      if (COLLISION[Peter_y][Peter_x]==2) drawMap(Peter_x*16, Peter_y*16, PETER_GRASS0);
      else drawMap(Peter_x*16, Peter_y*16, PETER0);
      break;
    case 1:
      if (COLLISION[Peter_y][Peter_x]==2) drawMap(Peter_x*16, Peter_y*16, PETER_GRASS2);
      else drawMap(Peter_x*16, Peter_y*16, PETER2);
      break;
    case 2:
      if (COLLISION[Peter_y][Peter_x]==2) drawMap(Peter_x*16, Peter_y*16, PETER_GRASS4);
      else drawMap(Peter_x*16, Peter_y*16, PETER4);
      break;
    case 3:
      if (COLLISION[Peter_y][Peter_x]==2) drawMap(Peter_x*16, Peter_y*16, PETER_GRASS7);
      else drawMap(Peter_x*16, Peter_y*16, PETER7);
      break;
    default:return;
  }
}

void drawRGBDetouredBitmap(int x, int y, uint16_t BITMAP [], int h, int w) {
  int row, col, buffidx=0;
  for (row=0; row<h; row++) {
    for (col=0; col<w; col++) {
      if (pgm_read_word(BITMAP + buffidx)!=0xf815) tft.drawPixel(x+col, y+row, pgm_read_word(BITMAP + buffidx));buffidx++;
    }
  }
}

void refreshScreen() {
  if (refresh) {
    if (battlePhase>2 && battlePhase!=308) {
      if (Peter_area==0 || Peter_area==1) drawBackground(0);
    }
    else {
      PeterPlace(Peter_dir);
    }
    refresh = false;
  }
}

void drawChatBox() {
  const int frameIncreaser = 250;
  if (chatBox) {
    tft.setTextSize(2);
    if (chatBoxStartUse<3) {chatBoxStartUse+=1;chatBoxTimer=0;}
    if (chatBoxStartUse==3) {
      chatBoxStartUse+=1;
      chatBoxTimer=0;
      tft.fillRect(0,240,240,80,ILI9341_WHITE);
      tft.drawRect(0,240,240,80,ILI9341_BLACK);
      chatBoxTimer=0;
    }

    if (chatBoxStartUse==4) {
      unsigned int numeroLettre = chatBoxTimer/frameIncreaser;
      String chatBoxLetter;
      
      while ((chatBoxLetter=chatBoxText[numeroLettre])==" ") {
        numeroLettre+=1;
        chatBoxTimer+=frameIncreaser;
      }
      
      if (numeroLettre<21) tft.setCursor(5+11*numeroLettre, 244);
      else if (numeroLettre<42) tft.setCursor(5+11*(numeroLettre-21), 264);
      else if (numeroLettre<63) tft.setCursor(5+11*(numeroLettre-42), 284);
      else tft.setCursor(5+11*(numeroLettre-63), 304);
      
      tft.print(chatBoxText[numeroLettre]);
      
      if (chatBoxText.length()==numeroLettre) {chatBox=false;chatBoxStartUse+=1;}
      tft.setTextSize(1);
    }
  }
  
  chatBoxTimer+=1;

  if (actionPressed && chatBox==0 && not(actionLocked) && menuPhase==0) {
    chatBoxStartUse=0;
    refresh = true;
    if (battlePhase==0) {
      Peter_move=true;
    }
  }

}

void drawBattleTransition() {
  if (battleTransition) {
    actionLocked = true;
    if (true) {
      if (battleTransitionTimer<5000) {
        tft.drawRect((battleTransitionTimer/20),(battleTransitionTimer/20),240-(battleTransitionTimer/10),320-(battleTransitionTimer/10),ILI9341_BLACK);
        tft.drawRect((battleTransitionTimer/16),(battleTransitionTimer/16),240-(battleTransitionTimer/8),320-(battleTransitionTimer/8),ILI9341_RED);
      }
      if (battleTransitionTimer==5000) battleTransitionTimer=100000;
    }


    if (battleTransitionTimer==100000) {
      combat=1;
      drawBackground(Peter_area);
      if (ennemyTeamID==-1) drawArduimonOrTrainer(ennemyTeam.pokemons[0].getID(),135,7, 0);
      else drawArduimonOrTrainer(ennemyTeamID,135,7, 2);
      drawSDDetouredImage(5, 5, "ANBR0.txt", 50, 130);
      tft.setCursor(5,12);
      tft.print(ennemyTeam.name);
      for (int e=0;e<ennemyTeam.getNombrePokemon();e++) {
        drawSDDetouredImage(29+e*11, 34, "ABALL.txt", 8, 8);
      }
      drawArduimonOrTrainer(-1,0,220,2);
      drawSDDetouredImage(105, 185, "ANBR1.txt", 50, 130);
      tft.setCursor(115,192);
      tft.print(myTeam.name);
      for (int e=0;e<myTeam.getNombrePokemon();e++) {
        drawSDDetouredImage(143+e*11, 215, "ABALL.txt", 8, 8);
      }
    }
    if (battleTransitionTimer<200000) battleTransitionTimer+=1;
  }
}

void drawBattleArduimonOrSprite() {
  if (ennemyArduimon) {
    drawArduimonOrTrainer(ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getID(), 135, 7, 0);
    ennemyArduimon = false;
  }
  
  if (myArduimon) {
    drawArduimonOrTrainer(myTeam.pokemons[myTeam.pokemonFrontID].getID(), 0, 138, 1);
    myArduimon = false;
  }

  if (ennemySprite) {
    drawArduimonOrTrainer(ennemyTeamID,135,7, 2);
    ennemySprite = false;
  }

  if (mySprite) {
    drawArduimonOrTrainer(-1,0,138, 2);
    mySprite = false;
  }
}

void drawBattleMainMenu() {
  if (battlePhase==8) {
    pointer=0;
    tft.setTextSize(2);
    tft.fillRect(0,240,240,80,ILI9341_WHITE);
    tft.drawRect(0,240,240,80,ILI9341_BLACK);
    tft.drawLine(120, 240, 120, 320, ILI9341_BLACK);
    tft.drawLine(0, 280, 240, 280, ILI9341_BLACK);
    tft.setCursor(15,250);tft.print("Attaque");
    tft.setCursor(140,250);tft.print("Equipe");
    tft.setCursor(15,290);tft.print("Objets");
    tft.setCursor(140,290);tft.print("Fuite");
    for (int e=0; e<12; e++) {tft.drawLine(115-e, 255+e/2, 115-e, 265-e/2, ILI9341_RED);}
    tft.setTextSize(1);
    battlePhase+=1;
  }
}

void drawBattleMoveSet() {
  tft.fillRect(0,240,240,80,ILI9341_WHITE);
  tft.drawRect(0,240,240,80,ILI9341_BLACK);
  tft.drawLine(120, 240, 120, 320, ILI9341_BLACK);
  tft.drawLine(0, 280, 240, 280, ILI9341_BLACK);
  tft.setCursor(20,255);tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMove(0).getName());
  tft.setCursor(145,255);tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMove(1).getName());
  tft.setCursor(20,295);tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMove(2).getName());
  tft.setCursor(145,295);tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMove(3).getName());
}

void drawBattleMoveMenu() {
  if (battlePhase==10) {
    drawBattleMoveSet();
    for (int e=0; e<12; e++) {tft.drawLine(115-e, 255+e/2, 115-e, 265-e/2, ILI9341_RED);}
    battlePhase+=1;
  }
}

void drawBattlePointer() {
  if (pointer != previousPointer) {
    if (battlePhase>8 && battlePhase<12) {
      chatBoxTimer=0;
      if (battlePhase>=9 && battlePhase<=11) {
        for (int e=0; e<12; e++) {
          tft.drawLine(115-e, 255+e/2, 115-e, 265-e/2, ILI9341_WHITE);
          tft.drawLine(235-e, 255+e/2, 235-e, 265-e/2, ILI9341_WHITE);
          tft.drawLine(115-e, 295+e/2, 115-e, 305-e/2, ILI9341_WHITE);
          tft.drawLine(235-e, 295+e/2, 235-e, 305-e/2, ILI9341_WHITE);
          switch(pointer) {
            case 0:tft.drawLine(115-e, 255+e/2, 115-e, 265-e/2, ILI9341_RED);break;
            case 1:tft.drawLine(235-e, 255+e/2, 235-e, 265-e/2, ILI9341_RED);break;
            case 2:tft.drawLine(115-e, 295+e/2, 115-e, 305-e/2, ILI9341_RED);break;
            case 3:tft.drawLine(235-e, 295+e/2, 235-e, 305-e/2, ILI9341_RED);break;
            default:return;
          }
        }
      }
    }

    if (battlePhase==513 || battlePhase==523) {
      chatBoxTimer=0;
      tft.fillRect(165, 256+36*previousPointer, 12, 12, ILI9341_WHITE);
      tft.fillRect(165, 256+36*pointer, 12, 12, ILI9341_RED);
    }

    if (battlePhase==517 || battlePhase==527) {
      chatBoxTimer=0;
      tft.fillRect(4+(previousPointer%2)*120, 252+(previousPointer/2)*40, 12, 12, ILI9341_WHITE);
      tft.fillRect(4+(pointer%2)*120, 252+(pointer/2)*40, 12, 12, ILI9341_RED);
    }

    if (battlePhase==211 || battlePhase==33 || battlePhase==33+49 || (battlePhase>=232 && battlePhase<=234) || menuPhase==2 || (menuPhase>=9 && menuPhase<=16)
    || (menuPhase>=21 && menuPhase<=23)) {
      chatBoxTimer=0;
      if (previousPointer!=menuPhase-10) {
        if (previousPointer!=0) drawSDDetouredImage(63, 97+30*previousPointer, "TMUS.txt", 24, 169);
        else drawSDDetouredImage(30, 30, "TMUS1.txt", 71, 141);
      }

      if (pointer!=0) drawSDDetouredImage(63, 97+30*pointer, "TMS.txt", 24, 169);
      else drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);

    }
    if (battlePhase==231 || menuPhase==3) {
      chatBoxTimer=0;
      switch(previousPointer) {
        case 0:drawSDDetouredImage(59, 49, "IMUS.txt", 22, 139);break;
        case 1:drawSDDetouredImage(59, 78, "IMUS.txt", 22, 139);break;
        case 2:drawSDDetouredImage(59, 107, "IMUS.txt", 22, 139);break;
        case 3:drawSDDetouredImage(59, 136, "IMUS.txt", 22, 139);break;
        case 4:drawSDDetouredImage(59, 165, "IMUS.txt", 22, 139);break;
        default:return;
      }

      switch(pointer) {
        case 0:drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);break;
        case 1:drawSDDetouredImage(59, 78, "IMS.txt", 22, 139);break;
        case 2:drawSDDetouredImage(59, 107, "IMS.txt", 22, 139);break;
        case 3:drawSDDetouredImage(59, 136, "IMS.txt", 22, 139);break;
        case 4:drawSDDetouredImage(59, 165, "IMS.txt", 22, 139);break;
        default:return;
      }
    }
  }
  if (battlePhase<12 || battlePhase==211 || battlePhase==33 || battlePhase==33+49
  || (battlePhase>=231 && battlePhase<=234) || battlePhase==513 || battlePhase==523
  || battlePhase==517 || battlePhase==527) previousPointer = pointer;
}

void drawArduimonOrTrainerBackground() {
  if (Peter_area==0 || Peter_area==1) {
    if (ArduimonOrTrainerBackground==1) drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
    if (ArduimonOrTrainerBackground==2) drawRGBDetouredBitmap(0, 138, BATTLE_MYSPRITEBACKGROUND, 100, 100);
    if (ArduimonOrTrainerBackground==3) {
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawRGBDetouredBitmap(5, 5, BATTLE_ENNEMYSLIFEBARBACKGROUND, 50, 130);
    }
    if (ArduimonOrTrainerBackground==4) {
      drawRGBDetouredBitmap(0, 138, BATTLE_MYSPRITEBACKGROUND, 100, 100);
      drawRGBDetouredBitmap(105, 185, BATTLE_MYSLIFEBARBACKGROUND, 50, 130);
    }
  }
  ArduimonOrTrainerBackground=0;
}
void drawBattleLifeBar() {
  if (battleLifeBar==1 || battleLifeBar==-1) {
    float lifeBarRatio = ennemyArduimonLifeBar/(100*100);
    drawRGBDetouredBitmap(5, 5, BATTLE_ENNEMYLIFEBAR, 50, 130);
    tft.setTextSize(2);
    tft.setCursor(6,8);
    tft.print(ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getName());
    tft.setCursor(29,29);
    tft.setTextSize(1);
    tft.print(ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getLvl());
    if (lifeBarRatio<0.2) tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_RED);
    else if (lifeBarRatio<0.5) tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_ORANGE);
    else tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_GREEN);
    if (battleLifeBar==1) battleLifeBar = 0;
  }

  if (battleLifeBar==2 || battleLifeBar==-1) {
    float lifeBarRatio = myArduimonLifeBar/(100*100);
    float actualXp = myTeam.pokemons[myTeam.pokemonFrontID].getXp();
    float nextLevelXp = myTeam.pokemons[myTeam.pokemonFrontID].getNextLvlXp();
    float xpBarRatio = actualXp/nextLevelXp;
    drawRGBDetouredBitmap(105, 185, BATTLE_MYLIFEBAR, 50, 130);
    tft.setTextSize(2);
    tft.setCursor(116,188);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getName());
    tft.setCursor(135,222);
    tft.setTextSize(1);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getLvl());
    if (lifeBarRatio<0.2) tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_RED);
    else if (lifeBarRatio<0.5) tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_ORANGE);
    else tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_GREEN);
    if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<10) tft.setCursor(199,222);
    else if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<100) tft.setCursor(193,222);
    else tft.setCursor(187,222);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0));
    tft.setCursor(210,222);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0));
    tft.fillRect(129,232,xpBarRatio*98,2,ILI9341_RED);
    tft.fillRect(129+xpBarRatio*98,232,98-xpBarRatio*98,2,ILI9341_BLUE);
    battleLifeBar = 0;
  }

  if (battleLifeBar==3 || battleLifeBar==4) {
    float lifeBarRatio;
    float actualLife;
    float maxLife;
    float lifeRatio;
    if (battleLifeBar==3) {
      lifeBarRatio = ennemyArduimonLifeBar/(100*100);
      actualLife = ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getActualStats(0);
      maxLife = ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getMaxStats(0);
      lifeRatio = actualLife/maxLife;
      if (lifeBarRatio<0.2) tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_RED);
      else if (lifeBarRatio<0.5) tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_ORANGE);
      else tft.fillRect(29,40,lifeBarRatio*90,8,ILI9341_GREEN);
      tft.fillRect(29+lifeBarRatio*90,40,90-lifeBarRatio*90,8,ILI9341_WHITE);
      if (ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getActualStats(0) ==
      ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getMaxStats(0)) tft.fillRect(29,40,90,8,ILI9341_GREEN);
    }
    else {
      lifeBarRatio = myArduimonLifeBar/(100*100);
      actualLife = myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0);
      maxLife = myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0);
      lifeRatio = actualLife/maxLife;
      int actualLifeHP = lifeBarRatio*maxLife;
      if (lifeBarRatio<0.2) tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_RED);
      else if (lifeBarRatio<0.5) tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_ORANGE);
      else tft.fillRect(139,210,lifeBarRatio*90,8,ILI9341_GREEN);
      tft.fillRect(139+lifeBarRatio*90,210,90-lifeBarRatio*90,8,ILI9341_WHITE);
      tft.fillRect(187,222,18,8,ILI9341_WHITE);
      if (actualLifeHP<10) tft.setCursor(199,222);
      else if (actualLifeHP<100) tft.setCursor(193,222);
      else tft.setCursor(187,222);
      tft.setTextSize(1);
      tft.print(actualLifeHP);
    }

    if (lifeBarRatio<lifeRatio) {
      if (battleLifeBar==3) ennemyArduimonLifeBar+=1;
      else myArduimonLifeBar+=1;
    }
    else if (lifeBarRatio>lifeRatio) {
      if (battleLifeBar==3) ennemyArduimonLifeBar-=1;
      else myArduimonLifeBar-=1;
    }

    if (floor(lifeBarRatio*100+0.5)/100==floor(lifeRatio*100+0.5)/100){
      if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0) ==
      myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0)) tft.fillRect(139,210,90,8,ILI9341_GREEN);
      tft.fillRect(187,222,18,8,ILI9341_WHITE);
      if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<10) tft.setCursor(199,222);
      else if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<100) tft.setCursor(193,222);
      else tft.setCursor(187,222);
      tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0));
      battleLifeBar = 0;
    }
  }

  if (battleLifeBar==5) {
    float actualXp = myTeam.pokemons[myTeam.pokemonFrontID].getXp();
    float nextLevelXp = myTeam.pokemons[myTeam.pokemonFrontID].getNextLvlXp();
    float xpBarRatio = actualXp/nextLevelXp;
    tft.fillRect(129,232,xpBarRatio*98,2,ILI9341_RED);
    tft.fillRect(129+xpBarRatio*98,232,98-xpBarRatio*98,2,ILI9341_BLUE);
    tft.fillRect(135,222,18,7,ILI9341_WHITE);
    tft.setCursor(135,222);
    tft.setTextSize(1);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getLvl());
    tft.fillRect(210,222,18,7,ILI9341_WHITE);
    tft.setCursor(210,222);
    tft.setTextSize(1);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0));
    tft.fillRect(135,222,8,8,ILI9341_WHITE);
    tft.setCursor(135,222);
    tft.setTextSize(1);
    tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getLvl());
    battleLifeBar = 0;
  }
}

void drawBattleArduimonSwitchMenu(int nbrArduimon) {
    drawArduimonOrTrainer(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getID(), 34, 90+30*nbrArduimon, -1);
    tft.setCursor(80,99+30*nbrArduimon);
    tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getName());
    tft.setCursor(98,110+30*nbrArduimon);
    tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getLvl());
    if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getActualStats(0)<10) tft.setCursor(174,99+30*nbrArduimon);
    else if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getActualStats(0)<100) tft.setCursor(168,99+30*nbrArduimon);
    else tft.setCursor(162,99+30*nbrArduimon);
    tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getActualStats(0));
    tft.setCursor(187,99+30*nbrArduimon);
    tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getMaxStats(0));
    float actualHP1 = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getActualStats(0);
    float maxHP1 = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(nbrArduimon)].getMaxStats(0);
    tft.fillRect(138,110+30*nbrArduimon,(actualHP1/maxHP1)*90,8,ILI9341_GREEN);
}

void drawBattleBallRound() {
    for (int i=0;i<randomInt(3)+3;i++) {
      //Serial.print("ITERATION N°");Serial.println(i);
      for (int j=0;j<25;j++) drawSDDetouredImage(168, 75, "BALL25C.txt", 25, 25);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      for (int j=0;j<25;j++) drawSDDetouredImage(168, 75, "BALL25D.txt", 25, 25);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      for (int j=0;j<25;j++) drawSDDetouredImage(168, 75, "BALL25C.txt", 25, 25);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      for (int j=0;j<25;j++) drawSDDetouredImage(168, 75, "BALL25G.txt", 25, 25);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(168, 75, "BALL25C.txt", 25, 25);
    }
}

void drawEquipe() {
  tft.setTextColor(ILI9341_WHITE);

  drawSDImage(0, 0, "TEAMENU.txt", 320, 240);
  drawArduimonOrTrainer(myTeam.pokemons[myTeam.pokemonFrontID].getID(), 42, 42, -1);
  tft.setCursor(74,40);
  tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getName());
  tft.setCursor(95,65);
  tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getLvl());
  if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<10) tft.setCursor(117,90);
  else if (myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0)<100) tft.setCursor(111,90);
  else tft.setCursor(105,90);
  tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0));
  tft.setCursor(130,90);
  tft.print(myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0));
  float actualHP1 = myTeam.pokemons[myTeam.pokemonFrontID].getActualStats(0);
  float maxHP1 = myTeam.pokemons[myTeam.pokemonFrontID].getMaxStats(0);
  tft.fillRect(75,79,(actualHP1/maxHP1)*90,8,ILI9341_GREEN);

  for (int e=1;e<(myTeam.getNombrePokemon());e++) {
    drawBattleArduimonSwitchMenu(e);
  }

  tft.setTextColor(ILI9341_BLACK);
}

void drawInventaire() {
  tft.setTextColor(ILI9341_WHITE);

  drawSDImage(0, 0, "ITEMENU.txt", 320, 240);

  int page = pointer/6;
  for (int e=page;e<page+5;e++) {
    //Serial.print("Lobjet à la position n°");Serial.print(e);Serial.print(" a pour ID : ");
    //Serial.println(getGraphicItemPositionID(e));
    if (getGraphicItemPositionID(e)==-1) break;
    drawArduimonOrTrainer(getGraphicItemPositionID(e), 39, 50+29*e, 3);
    tft.setTextSize(1);
    tft.setCursor(65,59+29*e);
    tft.print(ObjectName[getGraphicItemPositionID(e)]);
    tft.setCursor(180,59+29*e);
    tft.print(Peter_items[getGraphicItemPositionID(e)]);
  }

  tft.setTextColor(ILI9341_BLACK);
}

void drawEvolution() {
  if (chatBoxTimer==1) drawSDImage(65, 65, "CADREV.txt", 110, 110);
  if (chatBoxTimer%30000==0) drawArduimonOrTrainer(myTeam.pokemons[evolution].getID(), 70, 70, 0);
  else if (chatBoxTimer%20000==0) drawArduimonOrTrainer(myTeam.pokemons[evolution].getID(), 70, 70, -3);
  else if (chatBoxTimer%10000==0) drawArduimonOrTrainer(myTeam.pokemons[evolution].getID(), 70, 70, -2);
}

void drawBattleSwitchMenu() {
  if (SDDraw==1) {
    drawEquipe();
    SDDraw = 0;
  }

  if (SDDraw==2) {
    drawInventaire();
    SDDraw = 0;
  }

  if (SDDraw==3) {
    if (chatBoxTimer==1) {
      drawSDDetouredImage(84, 123, "BALL15.txt", 15, 15);
    }
    if (chatBoxTimer==25001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(84, 114, "BALL15.txt", 15, 15);
    }
    if (chatBoxTimer==50001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(83, 104, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==75001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(85, 92, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==100001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(87, 85, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==125001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(91, 74, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==150001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(94, 68, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==175001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(100, 60, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==200001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawSDDetouredImage(107, 56, "BALL19.txt", 19, 19);
    }
    if (chatBoxTimer==225001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(116, 55, "BALL21.txt", 21, 21); 
    }
    if (chatBoxTimer==250001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(137, 56, "BALL21.txt", 21, 21); 
    }
    if (chatBoxTimer==275001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(147, 59, "BALL21.txt", 21, 21); 
    }
    if (chatBoxTimer==300001/2) {
      drawRGBDetouredBitmap(5, 55, BATTLE_THROWBACKGROUND, 83, 130);
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(157, 69, "BALL21.txt", 21, 21); 
    }
    if (chatBoxTimer==325001/2) {
      drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
      drawSDDetouredImage(168, 75, "BALL25C.txt", 25, 25);
      SDDraw = 0;
      battlePhase+=1;
    }  
  }

  if (SDDraw==4) {
    drawBattleBallRound();
    for (int i=0; i<6; i++) {
      String nameFile = "KLF";
      nameFile+=i;
      nameFile+=".txt";
      for (int j=0;j<3;j++)drawSDDetouredImage(155, 62, nameFile, 50, 50);
    }
    drawRGBDetouredBitmap(135, 7, BATTLE_ENNEMYSPRITEBACKGROUND, 100, 100);
    drawArduimonOrTrainer(ennemyTeam.pokemons[ennemyTeam.pokemonFrontID].getID(), 135, 7, 0);
    SDDraw = 0;
    battlePhase+=1;
  }

  if (SDDraw==5) {
    drawBattleBallRound();
    for (int j=0;j<25;j++) drawSDDetouredImage(168, 75, "BALL25C.txt", 25, 25);
    drawSDDetouredImage(168, 75, "BALL25F.txt", 25, 25);
    SDDraw = 0;
    battlePhase+=1;
  }

  if (SDDraw==6) {
    drawEquipe();
    drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
    SDDraw = 0;
  }

  if (SDDraw==7) {
    tft.setTextSize(2);
    tft.drawLine(160, 240, 160, 320, ILI9341_BLACK);
    tft.drawLine(160, 280, 240, 280, ILI9341_BLACK);
    tft.setCursor(185,253);tft.print("Oui");
    tft.setCursor(185,290);tft.print("Non");
    tft.setTextSize(2);
    SDDraw = 0;
  }

  if (SDDraw==8) {
    drawBattleMoveSet();
    SDDraw = 0;
  }

  if (SDDraw==9) {
    if (battlePhase==309) drawEvolution();
    if (battlePhase==310) {
      drawSDImage(65, 65, "CADREV.txt", 110, 110);
      drawArduimonOrTrainer(myTeam.pokemons[evolution].getID()+1, 70, 70, 0);
    }
    if (battlePhase==311) {
      drawSDImage(65, 65, "CADREV.txt", 110, 110);
      drawArduimonOrTrainer(myTeam.pokemons[evolution].getID(), 70, 70, 0);
    }
    if (battlePhase>309) SDDraw = 0;
  }
}

void drawMenu() {

  if (startPressed && menuPhase==0 && battlePhase==0 && Peter_move == true) {
    drawSDImage(130, 25, "MENU.txt", 200, 110);
    menuPhase = 1;
    pointer = 0;
    previousPointer = 0;
    for (int e=12;e>0;e--) tft.drawLine(144-e, 76+e/2, 144-e, 76-e/2, ILI9341_RED);
    Peter_move = false;
    startPressed = false;
  }

  if (backPressed && menuPhase==1) {
    menuPhase = 0;
    Peter_move = true;
    refresh = true;
    startPressed = false;
  }

  if (menuPhase==1) {
    if (previousPointer!=pointer) {
      drawSDImage(130, 25, "MENU.txt", 200, 110);
      for (int e=12;e>0;e--) tft.drawLine(144-e, 76+pointer*20+e/2, 144-e, 76+pointer*20-e/2, ILI9341_RED);
      previousPointer = pointer;
    }
  }

  if (actionPressed) {
    if (menuPhase==1) {
      switch(pointer) {
        case 0:
          Serial.println("EQUIPE");
          drawEquipe();drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
          menuPhase = 2;
          actionPressed = false;
          break;
        case 1:
          Serial.println("OBJETS");
          drawInventaire();
          drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);
          menuPhase = 3;
          actionPressed = false;
          pointer = 0;
          break;
        case 2:
          Serial.println("SAVE");
          save(0);
          activateChatBox("Partie sauvegardee !");
          chatBoxTimer = 0;
          menuPhase = 4;
          break;
        case 3:
          Serial.println("MAIN QUEST");
          drawSDDetouredImage(0, 0, "QUEST.txt", 320, 240);
          menuPhase = 5;
          break;
        case 4:
          Serial.println("CREDITS");
          drawSDImage(0, 0, "CREDITT.txt", 320, 240);
          menuPhase = 7;
          break;
        case 5:
          Serial.println("ECHANGE");
          drawSDImage(0, 0, "ECHMENU.txt", 320, 240);
          chatBoxStartUse=0;
          activateChatBox("En cours d'appairage avec la console associee ...");
          while (blueSerial.available()) blueSerial.read();
          menuPhase = 8;
          break;
        default:return;
      }
    }
  }

  if (menuPhase==2) {
    if (actionPressed) {
      if (pointer!=0) drawSDDetouredImage(63, 97 + 30*pointer, "TMS.txt", 24, 169);
      else drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
      menuPhase = 10 + pointer;
      actionPressed = false;
    }
  }

  if (menuPhase==3) {
    if (actionPressed) {
      actionPressed = false;
      if (getGraphicItemPositionID(pointer)<3) {
        menuPhase = 21+getGraphicItemPositionID(pointer);//USE BALL OR POTIONS
        drawEquipe();
        drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
      }
      else {
        chatBoxStartUse = 0;
        chatBoxTimer = 0;
        activateChatBox("Tout compte fait, je ne vois pas comment utiliser cela ici ...");
        menuPhase = 20;
      }
    }
  }

  if (menuPhase>=20 && menuPhase<=23) {
    if (menuPhase==20) {
      if (chatBoxTimer>5000*125) {
        drawInventaire();
        drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);
        menuPhase = 3;
        actionPressed = false;
        pointer = 0;
      }
    }
    else {
      if (actionPressed && myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)<myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)) {
        switch(menuPhase) {
          case 21://potion
            Peter_items[0]-=1;
            chatBoxText = myTeam.name;
            chatBoxText+=" utilise une potion sur ";
            chatBoxText+=myTeam.pokemons[pointer].getName();
            chatBoxStartUse=0;
            activateChatBox(chatBoxText);
            myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].setActualStats(0, myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)+20);
            chatBoxTimer = 0;
            break;
          case 22://eau de vie
            Peter_items[1]-=1;
            chatBoxText = myTeam.name;
            chatBoxText+=" utilise une eau de vie sur ";
            chatBoxText+=myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getName();
            chatBoxStartUse=0;
            activateChatBox(chatBoxText);
            myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].setActualStats(0, myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)+50);
            myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].setStatut(0,0);
            chatBoxTimer = 0;
            break;
          case 23://baie
            Peter_items[2]-=1;
            chatBoxText = myTeam.name;
            chatBoxText+=" utilise une baie sur ";
            chatBoxText+=myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getName();
            chatBoxStartUse=0;
            activateChatBox(chatBoxText);
            myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].setActualStats(0, myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)+10);
            chatBoxTimer = 0;
            break;
          default:return;
        }
        menuPhase+=3;
      }
      if (backPressed==true) {
        backPressed = false;
        drawInventaire();
        drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);
        menuPhase = 3;
        actionPressed = false;
        pointer = 0;
      }
    }
  }

  if (menuPhase>=24 && menuPhase<=26) {
    if (chatBoxTimer>5000*125) {
      float lifeBarRatio;
      float actualLife;
      float maxLife;
      float lifeRatio;
      lifeBarRatio = myArduimonLifeBar/(100*100);
      actualLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0);
      maxLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0);
      lifeRatio = actualLife/maxLife;

      int actualWrittenLife = lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0);

      Serial.print("RATIO DE VIE ACTUEL : ");Serial.println(lifeRatio);
      Serial.print("RATIO DE BAR DE VIE ACTUEL : ");Serial.println(lifeBarRatio);

      tft.setTextColor(ILI9341_WHITE);

      while (true) {

        if (pointer==0) {
          for (int i=0;i<10;i++) tft.fillRect(75+lifeBarRatio*90,79,2,8,ILI9341_GREEN);
          if (actualWrittenLife!=int(lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0))) {
            actualWrittenLife = lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0);
            if (lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)<10) tft.setCursor(117,90);
            else if (lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)<100) tft.setCursor(111,90);
            else tft.setCursor(105,90);
            drawSDImage(105, 90, "ELVBGD1.txt", 8, 18);
            tft.print(int(lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)));
            if (floor(lifeBarRatio*100+0.5)/100==floor(lifeRatio*100+0.5)/100) {
              if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)==myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)) {
                if (lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)<10) tft.setCursor(117,90);
                else if (lifeBarRatio*myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)<100) tft.setCursor(111,90);
                else tft.setCursor(105,90);
                drawSDImage(105, 90, "ELVBGD1.txt", 8, 18);
                tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0));
              }
              break;
            }
          }
        }
        else {
          for (int i=0;i<10;i++) tft.fillRect(138+lifeBarRatio*90,110+30*pointer,2,8,ILI9341_GREEN);
          if (int(actualLife)!=myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)) {
            if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)<10) tft.setCursor(174,99+30*pointer);
            else if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)<100) tft.setCursor(168,99+30*pointer);
            else tft.setCursor(162,99+30*pointer);
            drawSDImage(162, 128+30*pointer, "ELVBGD2.txt", 8, 18);
            tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)); 
            if (floor(lifeBarRatio*100+0.5)/100==floor(lifeRatio*100+0.5)/100) {
              if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)==myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0)) {
                if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)<10) tft.setCursor(174,99+30*pointer);
                else if (myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0)<100) tft.setCursor(168,99+30*pointer);
                else tft.setCursor(162,99+30*pointer);
                drawSDImage(162, 128+30*pointer, "ELVBGD2.txt", 8, 18);
                tft.print(myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0));
              }
              break;
            }
          }
        }


        if (lifeBarRatio<lifeRatio) {
          myArduimonLifeBar+=1;
        }
        else if (lifeBarRatio>lifeRatio) {
          myArduimonLifeBar-=1;
        }

        lifeBarRatio = myArduimonLifeBar/(100*100);
        actualLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0);
        maxLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0);
        lifeRatio = actualLife/maxLife;
      }
      tft.setTextColor(ILI9341_BLACK);
      drawInventaire();
      drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);
      menuPhase = 3;
      pointer = 0;
    }

    //if (chatBoxTimer==5000*125) {
      //float actualLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getActualStats(0);
      //float maxLife = myTeam.pokemons[myTeam.getGraphicPokemonPositionID(pointer)].getMaxStats(0);
      //myArduimonLifeBar = (actualLife/maxLife)*100*100;
      //drawEquipe();
    //}
    //if (chatBoxTimer==7500*125) {
      //drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
      //drawInventaire();
      //drawSDDetouredImage(59, 49, "IMS.txt", 22, 139);
      //menuPhase = 3;
      //pointer = 0;
    //}
  }

  if (menuPhase>=10 && menuPhase<=16) {
    int nbr = menuPhase-10;
    if (actionPressed && (nbr!=pointer)) {
        
      Pokemon save = Pokemon(
                myTeam.pokemons[nbr].getID(),
                myTeam.pokemons[nbr].getLvl(),
                myTeam.pokemons[nbr].getXp(),
                myTeam.pokemons[nbr].getActualStats(0),
                myTeam.pokemons[nbr].getMove(0).getID(),
                myTeam.pokemons[nbr].getMove(1).getID(),
                myTeam.pokemons[nbr].getMove(2).getID(), 
                myTeam.pokemons[nbr].getMove(3).getID(),
                myTeam.pokemons[nbr].getStatut(0).getID()
                );
      myTeam.pokemons[nbr] = myTeam.pokemons[pointer];
      myTeam.pokemons[pointer] = save;
      drawEquipe();
      if (pointer!=0) drawSDDetouredImage(63, 97 + 30*pointer, "TMS.txt", 24, 169);
      else drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
      menuPhase = 2;
    }
    if (backPressed || (actionPressed && nbr==pointer)) {
      if (pointer!=0) drawSDDetouredImage(63, 97 + 30*nbr, "TMUS.txt", 24, 169);
      else drawSDDetouredImage(30, 30, "TMUS1.txt", 71, 141);
      menuPhase = 2;
      backPressed = false;
    }
  }

  if (menuPhase==4 && chatBoxTimer>5000*125) {
    PeterPlace(Peter_dir);
    menuPhase = 0;
    chatBox = false;
    chatBoxStartUse = 0;
    Peter_move = true;
  }

  if (menuPhase==5) {
    String fileName = "QUEST";
    if (gamePhase<5) fileName+="0";
    fileName+=".txt";
    drawSDDetouredImage(0, 0, fileName, 320, 240);
    menuPhase = 6;
  }

  if (menuPhase==7) {
    drawSDDetouredImage(0, 0, "CREDITS.txt", 320, 240);
    menuPhase = 6;
  }

  if (menuPhase==8) {
    if (chatBoxTimer%200000==0) drawSDImage(90, 90, "LINK3.txt", 60, 60);
    else if (chatBoxTimer%150000==0) drawSDImage(90, 90, "LINK2.txt", 60, 60);
    else if (chatBoxTimer%100000==0) drawSDImage(90, 90, "LINK1.txt", 60, 60);
    else if (chatBoxTimer%50000==0) {
      drawSDImage(90, 90, "LINK2.txt", 60, 60);
      blueSerial.print("1\r\n");
    }

    if (chatBoxTimer>250000 && blueSerial.available()) {
      if (char(blueSerial.read())=='1') {
        blueSerial.print("1\r\n");
        menuPhase = 999;
        chatBoxStartUse=0;
        activateChatBox("Appairage termine ! Selectionnez l'Arduimon a echanger.");
      }
    }
  }

  if (menuPhase==9 || menuPhase==999 || menuPhase==1000 || menuPhase==1001 || menuPhase==1002) {
    char blueReceived = ' ';
    if (blueSerial.available()) blueReceived = blueSerial.read();
    if (chatBoxTimer==5000*125 && menuPhase==999) {
      drawEquipe();
      drawSDDetouredImage(30, 30, "TMS1.txt", 71, 141);
      pointer = 0;
      previousPointer = 1;
      while (blueSerial.available()) blueSerial.read();
      menuPhase = 9;
    }
    if (menuPhase==9 || menuPhase==1000) {
      if (blueReceived=='0') {
        chatBoxTimer = 0;
        chatBoxStartUse=0;
        activateChatBox("Votre partenaire a annule la procedure d'echange !");
        menuPhase = 1001;
      }
      if (menuPhase==9) {
        if (actionPressed) {
          menuPhase = 1000;
          while (blueSerial.available()) blueSerial.read();
        }
      }
      if (menuPhase==1000) {
        if (backPressed) {
          menuPhase = 9;
          backPressed = false;
        }
        blueSerial.print("2\r\n");
        if (blueReceived=='2') {
          blueSerial.print("2\r\n");
          while (blueSerial.available()) blueSerial.read();
          menuPhase = 1002;
          drawSDImage(0, 0, "ECHMENU.txt", 320, 240);
          chatBoxTimer = 0;
          chatBoxStartUse=0;
          activateChatBox("Echange en cours ...");
        }
      }
    }
    if (menuPhase==1001 && chatBoxTimer>5000*125) {
      menuPhase = 2;
      backPressed = true;
      Serial.println("L'AUTRE A QUITTE MDR"); 
    }
    if (menuPhase==1002 && chatBoxTimer>5000*125) {
      String ArduimonSent = "##############################,";
      ArduimonSent+=myTeam.pokemons[pointer].getID();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getLvl();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getXp();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getActualStats(0);ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getMove(0).getID();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getMove(1).getID();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getMove(2).getID();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getMove(3).getID();ArduimonSent+=",";
      ArduimonSent+=myTeam.pokemons[pointer].getStatut(0).getID();ArduimonSent+=",";
      ArduimonSent+="##############################,\r\n";
      blueSerial.print(ArduimonSent);

      for (int i=0; i<30;i++) {
        drawSDImage(60, 60, "ECHAN1.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN2.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN3.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN4.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN5.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN6.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN7.txt", 120, 120);
        drawSDImage(60, 60, "ECHAN8.txt", 120, 120);
      }

      //tradingIsOk = false;
      //while (not(tradingIsOk)) {

      String ArduimonReceived = "";
      while (blueSerial.available()) ArduimonReceived+=char(blueSerial.read());

      int index = 0;
      String ID = "";
      String lvl = "";
      String xp = "";
      String hp = "";
      String move1 = "";
      String move2 = "";
      String move3 = "";
      String move4 = "";
      String statut = "";

        //while (true) {
          //while (not(blueSerial.available())) {}
          //char read = blueSerial.read();
          //if (ArduimonReceived[0]=="#" && ArduimonReceived[ArduimonReceived.length()-1]=="#") {
            //blueSerial.print("3\r\n");
            //if (read=='3') {
              //tradingIsOk = true;
              //break;
            //}
            //else if (read=='4') {
              //blueSerial.print(ArduimonSent);
              //delay(2000);
            //}
            //else Serial.print("ATTENTION : ON A RECU UN CARACTERE ANORMAL QUI EST : ");Serial.println(read);
          //else {
            //blueSerial.print("4\r\n");
            //if (read=='4') blueSerial.print(ArduimonSent);
            //delay(1000);
            //break;
          //}
        //}
      //}

      Serial.println("LA CHAINE DE CARACTERE ENVOYEE : ");
      Serial.println(ArduimonSent);
      Serial.println("LA CHAINE DE CARACTERE RECUE : ");
      Serial.println(ArduimonReceived);

      while(ArduimonReceived[index]=='#') index++;
      index++;
      while(ArduimonReceived[index]!=',') {ID+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {lvl+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {xp+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {hp+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {move1+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {move2+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {move3+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {move4+=ArduimonReceived[index];index++;}
      index++;
      while(ArduimonReceived[index]!=',') {statut+=ArduimonReceived[index];index++;}

      Serial.print("ID : ");Serial.println(ID);
      Serial.print("lvl : ");Serial.println(lvl);
      Serial.print("xp : ");Serial.println(xp);
      Serial.print("hp : ");Serial.println(hp);
      Serial.print("move1 : ");Serial.println(move1);
      Serial.print("move2 : ");Serial.println(move2);
      Serial.print("move3 : ");Serial.println(move3);
      Serial.print("move4 : ");Serial.println(move4);
      Serial.print("statut : ");Serial.println(statut);   

      myTeam.pokemons[pointer] = Pokemon(ID.toInt(),
                                         lvl.toInt(),
                                         xp.toInt(),
                                         hp.toInt(),
                                         move1.toInt(),
                                         move2.toInt(),
                                         move3.toInt(),
                                         move4.toInt(),
                                         statut.toInt());
      chatBoxStartUse=0;
      activateChatBox("Echange termine !");
      menuPhase = 1003;
    }
    if (menuPhase==1003 && chatBoxTimer>5000*125) {
      menuPhase = 999;
    }
  }

                
  if ((menuPhase>=2 && menuPhase<=7) || ((menuPhase>=8 && menuPhase<=9) && chatBoxTimer>=5000*125)) {
    if (backPressed) {
      if (menuPhase==9) blueSerial.print("0\r\n");
      PeterPlace(Peter_dir);
      drawSDDetouredImage(130, 25, "MENU.txt", 200, 110);
      pointer = 0;
      previousPointer = 0;
      for (int e=12;e>0;e--) tft.drawLine(144-e, 76+e/2, 144-e, 76-e/2, ILI9341_RED);
      menuPhase = 1;
    }
  }
}

void drawBattleUpdate() {
  drawMenu();
  drawBattleTransition();
  drawArduimonOrTrainerBackground();
  drawBattleArduimonOrSprite();
  drawBattleMainMenu();
  drawBattleSwitchMenu();
  drawBattleMoveMenu();
  drawBattlePointer();
  drawBattleLifeBar();
}
