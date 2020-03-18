#include "Pokemon.h"

extern Equipe myTeam;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial OK !");
  
  myTeam = Equipe("Peter", 0, 5,
                           2, 7,
                           4, 13,
                           6, 17,
                           8, 21,
                           9, 25);

  Serial.print("My character's name is :");Serial.println(myTeam.name);
  for (int i=0;i<6;i++) {
    Serial.print("The Arduimon n°");
    Serial.print(i);
    Serial.print(" is called ");
    Serial.print(myTeam.pokemons[i].getName());
    Serial.print(" and is lvl ");
    Serial.print(myTeam.pokemons[i].getLvl());
    Serial.println(".");

    Serial.println("Here's its stats :");
    Serial.print("      HP : ");Serial.println(myTeam.pokemons[i].getActualStats(0));
    Serial.print("      ATK : ");Serial.println(myTeam.pokemons[i].getActualStats(1));
    Serial.print("      DEF : ");Serial.println(myTeam.pokemons[i].getActualStats(2));
    Serial.print("      ATKSPE : ");Serial.println(myTeam.pokemons[i].getActualStats(3));
    Serial.print("      DEFSPE : ");Serial.println(myTeam.pokemons[i].getActualStats(4));
    Serial.print("      VIT : ");Serial.println(myTeam.pokemons[i].getActualStats(5));
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
