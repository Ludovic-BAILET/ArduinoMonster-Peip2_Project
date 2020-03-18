#include "Pokemon.h"
#include <arduino.h>
#include <string.h>

/*-----------------------------------------------------------------POKEMON DB------------------------------------------------------------------------------------*/

int pokeSpecs[tailleListePokemon][6] = { //Le i-ème pokemon / la j-ème spec du i-ème pokemon parmi [hp,atk,atkSpe,def,defSpe,vit]
	{110,64,64,80,80,60},
	{104,67,58,75,65,80},
};

int pokeTypes[tailleListePokemon][2] = { //Le i-ème pokemon / le j-ème type du i-eme pokemon parmi [0-17], cf Doc
	{10,11},
	{6,0},
};

int pokeMoves[tailleListePokemon][10][2] { //Le i-ème pokemon / le j-ème Move / la k-ème spec du j-ème move parmi [ID,lvl requis]
	{{0,0},{1,0},{2,7},{3,13},{4,20},{5,27},{6,34},{7,41},{8,48},{255,255}},
	{{9,0},{1,0},{10,9},{11,15},{255,255}},
};

String _pokeNames[tailleListePokemon] = { //Le nom du i-ème pokemon
	"DAN",
	"RUBEN",
};

/*-----------------------------------------------------------------POKEMON DB------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------MOVES DB-------------------------------------------------------------------------------------*/

int moveSpecs[tailleListeMove][2] = { //Le i-ème Move / la j-ème spec du i-eme Move parmi [pwr,preci]
	{35,95}, //0
	{0,100}, //1
	{0,90}, //2
	{35,100}, //3
	{0,75}, //4
	{55,95}, //5
	{0,255}, //6
	{0,75}, //7
	{120,100}, //8
	{40,100}, //9
	{40,100}, //10
	{0,100}, //11
};

int moveType[tailleListeMove] = { //Le i-ème Move / le j-ème type du i-eme Move parmi [0-17], cf Doc
	9, //0
	9, //1
	10, //2
	10, //3
	11, //4
	10, //5
	9, //6
	10, //7
	10, //8
	9, //9
	6, //10
	9, //11
};

int moveBools[tailleListeMove][2] { //Le i-ème Move / la j-ème spec du i-eme Move parmi [typeOfDamage,stats,statsTarget,statut]
	{0,0}, //0
	{0,1}, //1
	{0,1}, //2
	{1,0}, //3
	{0,0}, //4
	{1,0}, //5
	{0,0}, //6
	{0,0}, //7
	{1,0}, //8
	{0,0}, //9
	{1,0}, //10
	{0,1}, //11
};

int moveStatutsStats[tailleListeMove][4] = { //Le i-ème Move / la j-ème spec du i-eme Move parmi [statsID,statsChange,statutID,statutRate]
	{0,0,0,0}, //0
	{1,75,0,0}, //1
	{0,0,7,100}, //2
	{0,0,0,0}, //3
	{0,0,4,100}, //4
	{0,0,0,0}, //5
	{1,140,0,0}, //6
	{0,0,5,100}, //7
	{0,0,0,101}, //8 ; 101 signifie qu'il faut un tour AVANT d'attaquer
	{0,0,0,0}, //9
	{0,0,1,10}, //10
	{3,75,0,0}, //11
};

String _moveNames[tailleListeMove] = { //Le nom du i-ème Move
	"CHARGE", //0
	"RUGISSEMENT", //1
	"VAMPIGRAINE", //2
	"FOUET LIANE", //3
	"POUDRE TOXIK", //4
	"TRANCH'HERBE", //5
	"CROISSANCE", //6
	"POUDRE DODO", //7
	"LANCE-SOLEIL", //8
	"GRIFFE",//9
	"FLAMMECHE", //10
	"GROZ'YEUX", //11
};

/*------------------------------------------------------------------MOVES DB-------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------STATUTS DB------------------------------------------------------------------------------------*/

String _statutNames[tailleListeStatut] = { //Le nom du i-ème Statut
	"None",
	"BRULÛRE",
	"GEL",
	"PARALYSIE",
	"POISON",
	"SOMMEIL",
	"CONFUSION",
	"VAMPIGRAINE"
};

/*-----------------------------------------------------------------STATUTS DB------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------TYPES---------------------------------------------------------------------------------------*/

String _typeNames[tailleListeType] = { //Le nom du i-ème type
	"None",
	"ACIER",
	"COMBAT",
	"DRAGON",
	"EAU",
	"ELECTRIK",
	"FEU",
	"GLACE",
	"INSECTE",
	"NORMAL",
	"PLANTE",
	"POISON",
	"PSY",
	"ROCHE",
	"SOL",
	"SPECTRE",
	"TENEBRES",
	"VOL",
};

/*-------------------------------------------------------------------TYPES--------------------------------------------------------------------------------------*/

String getTypeName(int ID) {return _typeNames[ID];} //Renvoie le nom d'un type

float typeCoeffMultiplier(int moveID, int targetID) { //Renvoie le coefficient multiplicateur d'un type sur un autre
	if (targetID==0) {return 1;} //On procède par de multiples
	switch(targetID-1) {		 //disjonctions de cas
		case 0 : switch(moveID-1) {case 1:return 2;case 3:return 1;case 4:return 1;case 5:return 2;case 10:return 0;case 13:return 2;default:return 0.5;}
		case 1 : switch(moveID-1) {case 7:return 0.5;case 11:return 2;case 12:return 0.5;case 15:return 0.5;case 16:return 2;default:return 1;}
		case 2 : switch(moveID-1) {case 2:return 2;case 3:return 0.5;case 4:return 0.5;case 5:return 0.5;case 6:return 2;case 9:return 0.5;default:return 1;}
		case 3 : switch(moveID-1) {case 0:return 0.5;case 3:return 0.5;case 4:return 2;case 5:return 0.5;case 6:return 0.5;case 8:return 2;default:return 1;}
		case 4 : switch(moveID-1) {case 0:return 0.5;case 4:return 0.5;case 13:return 2;case 16:return 0.5;default:return 1;}
		case 5 : switch(moveID-1) {case 0:return 0.5;case 3:return 2;case 5:return 0.5;case 6:return 0.5;case 7:return 0.5;case 9:return 0.5;case 12:return 2;case 13:return 2;default:return 1;}
		case 6 : switch(moveID-1) {case 0:return 2;case 1:return 2;case 5:return 2;case 6:return 0.5;case 7:return 0.5;case 9:return 2;default:return 1;}
		case 7 : switch(moveID-1) {case 1:return 0.5;case 5:return 2;case 9:return 0.5;case 12:return 2;case 13:return 0.5;case 16:return 2;default:return 1;}
		case 8 : switch(moveID-1) {case 1:return 2;case 14:return 0;default:return 1;}
		case 9 : switch(moveID-1) {case 3:return 0.5;case 4:return 0.5;case 5:return 2;case 6:return 2;case 7:return 2;case 9:return 0.5;case 10:return 2;case 13:return 0.5;case 16:return 2;default:return 1;}
		case 10 : switch(moveID-1) {case 1:return 0.5;case 7:return 0.5;case 9:return 0.5;case 10:return 0.5;case 11:return 2;case 13:return 2;default:return 1;}
		case 11 : switch(moveID-1) {case 1:return 0.5;case 7:return 2;case 11:return 0.5;case 14:return 2;case 15:return 2;default:return 1;}
		case 12 : switch(moveID-1) {case 0:return 2;case 1:return 2;case 3:return 2;case 5:return 0.5;case 8:return 0.5;case 9:return 2;case 10:return 0.5;case 13:return 2;case 16:return 0.5;default:return 1;}
		case 13 : switch(moveID-1) {case 3:return 2;case 4:return 0;case 6:return 2;case 9:return 2;case 10:return 0.5;case 12:return 0.5;default:return 1;}
		case 14 : switch(moveID-1) {case 1:return 0;case 7:return 0.5;case 8:return 0;case 10:return 0.5;case 14:return 2;case 15:return 2;default:return 1;}
		case 15 : switch(moveID-1) {case 1:return 2;case 7:return 2;case 11:return 0;case 14:return 0.5;case 15:return 0.5;default:return 1;}
		case 16 : switch(moveID-1) {case 1:return 0.5;case 4:return 2;case 6:return 2;case 7:return 0.5;case 9:return 0.5;case 12:return 2;case 13:return 0;default:return 1;}
	}
	return 1;
}

/*-------------------------------------------------------------------TYPES--------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------STATS--------------------------------------------------------------------------------------*/

String _statNames[tailleListeStats] = { //Le nom de la i-ème stat
	{"HP"},{"ATK"},{"ATKSP"},{"DEF"},{"DEFSP"},{"VIT"}
};

/*-------------------------------------------------------------------STATS--------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------AUTRE--------------------------------------------------------------------------------------*/

void viderBuffer() { //Vide le buffer du moniteur série en lisant chaque caractère restant
	while(Serial.available () > 0){
    	Serial.read();
    }
}

bool ecrire1; //Les
bool ecrire2; //4
bool ecrire3; //variables
bool ecrire4; //d'écriture

/*-------------------------------------------------------------------AUTRE--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------CLASS STATUT----------------------------------------------------------------------------------*/

	/*Constructeurs*/
Statut::Statut(int ID) { //Construit un Statut en particulier
	this -> ID=ID;
	this -> brulureAtkDown=0;
	this -> paralysieVitDown=0;
	if (ID==5){this -> sommeilTurns=rand()% 4 + 1;} else {this -> sommeilTurns=0;} //S'il s'agit de sommeil, on initialise le nombre de tours de sommeil
	if (ID==6){this -> confusionTurns=rand()% 4 + 1;} else {this -> confusionTurns=0;} //S'il s'agit de confusion, on initialise le nombre de tours de confusion
}

Statut::Statut() { //Construit le Statut vide
	this -> ID=0;
	this -> brulureAtkDown=0;
	this -> paralysieVitDown=0;
	this -> sommeilTurns=0;
	this -> confusionTurns=0;
}
	/*Constructeurs*/

	/*Méthodes*/
int Statut::getID() {return this -> ID;}
void Statut::setID(int ID) {this -> ID = ID;}

bool Statut::getBrulureAtkDown() {return this -> brulureAtkDown;}			void Statut::setBrulureAtkDown(bool brulureAtkDown) {this -> brulureAtkDown = brulureAtkDown;}
bool Statut::getParalysieVitDown() {return this -> paralysieVitDown;}		void Statut::setParalysieVitDown(bool paralysieVitDown) {this -> paralysieVitDown = paralysieVitDown;}
int Statut::getSommeilTurns() {return this -> sommeilTurns;}				void Statut::setSommeilTurns(int sommeilTurns) {this ->sommeilTurns = sommeilTurns;}
int Statut::getConfusionTurns() {return this -> confusionTurns;}			void Statut::setConfusionTurns(int confusionTurns) {this ->confusionTurns = confusionTurns;}
String Statut::getName() {return _statutNames[this -> ID];}
	/*Méthodes*/

/*----------------------------------------------------------------CLASS STATUT----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------CLASS MOVE-----------------------------------------------------------------------------------*/

	/*Constructeurs*/
Move::Move(int ID) {this -> ID = ID;} //Construit un Move en particulier
Move::Move() {this -> ID = 255;} //Construit le Move vide
	/*Constructeurs*/

	/*Méthodes*/
int Move::getID() {return this -> ID;}

int Move::getPwr() {return moveSpecs[this -> ID][0];}
int Move::getPreci() {return moveSpecs[this -> ID][1];}
int Move::getType()  {return moveType[this -> ID];}

int Move::getTypeOfDamage() {return moveBools[this -> ID][0];}
int Move::getStatsTarget() {return moveBools[this -> ID][1];}

int Move::getStatsID() {return moveStatutsStats[this -> ID][0];}
int Move::getStatsChange() {return moveStatutsStats[this -> ID][1];}
int Move::getStatutID() {return moveStatutsStats[this -> ID][2];}
int Move::getStatutRate() {return moveStatutsStats[this -> ID][3];}

String Move::getName() {
	if (this-> getID()<255) { 			//Si le Move n'est pas 			Ceci est du au fait que les listes
		return _moveNames[this -> ID];  //le Move vide, on 				de données des Move ne contiennent
	}									//renvoie son nom 				pas de Move vide à l'index 0
	return "None";} //Sinon, on renvoie "None"
	/*Méthodes*/

/*-----------------------------------------------------------------CLASS MOVE------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------CLASS  POKEMON----------------------------------------------------------------------------------*/

	/*Constructeurs*/
Pokemon::Pokemon(int ID, int lvl) { //Construit un Pokemon en particulier
	this -> ID = ID;
	this -> lvl = lvl;

	this -> actualStats[0] 	= (10 + pokeSpecs[ID][0]*lvl/50);	this -> actualStats[1]  = 	5 + pokeSpecs[ID][1]*lvl/50;
	this -> actualStats[2] 	= 	5 + pokeSpecs[ID][2]*lvl/50;	this -> actualStats[3]  = 	5 + pokeSpecs[ID][3]*lvl/50;
	this -> actualStats[4] 	= 	5 + pokeSpecs[ID][4]*lvl/50;	this -> actualStats[5] 	= 	5 + pokeSpecs[ID][5]*lvl/50;

	this -> types[0] = pokeTypes[ID][0];
	this -> types[1] = pokeTypes[ID][1];

	this -> turnToWait = 0;
	this -> lastMove = -1;

	/*On attribue dynamiquement les dernièrs Move
	apprenables par le Pokemon selon son niveau*/
	int indexMaxMoveList;
	for (int i=0;true;i++) { 												//On récupère l'index
		if (pokeMoves[ID][i][1]>this -> lvl) {indexMaxMoveList=i-1;break;}  //du dernier
	}																		//Move apprenable

	for (int i=indexMaxMoveList;i>-1;i--) { 								//On attribue le Move
		this -> moveList[-i+indexMaxMoveList] = Move(pokeMoves[ID][i][0]);  //récupéré ainsi que 
		if (i==0 || i==indexMaxMoveList-3) {break;}							//les potentielles 3 
	}																		//Move précédentes
}

Pokemon::Pokemon() {this -> ID = 255;} //Construit le Pokemon vide
	/*Constructeurs*/

	/*Méthodes*/
void Pokemon::setID(int ID) { //Modifie l'ID du Pokemon courrant, et donc ses stats et types
	this -> ID = ID;

	this -> actualStats[0] 	= (10 + pokeSpecs[ID][0]*lvl/50);	this -> actualStats[1]  = 	5 + pokeSpecs[ID][1]*lvl/50;
	this -> actualStats[2] 	= 	5 + pokeSpecs[ID][2]*lvl/50;	this -> actualStats[3]  = 	5 + pokeSpecs[ID][3]*lvl/50;
	this -> actualStats[4] 	= 	5 + pokeSpecs[ID][4]*lvl/50;	this -> actualStats[5] 	= 	5 + pokeSpecs[ID][5]*lvl/50;

	this -> types[0] = pokeTypes[ID][0];
	this -> types[1] = pokeTypes[ID][1];
}

int Pokemon::getID() {return this -> ID;}						String Pokemon::getName() {return _pokeNames[this -> ID];}
int Pokemon::getXp() {return this -> xp;}						void Pokemon::setXp(int xp) {this -> xp = xp;}
int Pokemon::getLvl() {return this -> lvl;}						void Pokemon::setLvl(int lvl) {this -> lvl = lvl;}

int Pokemon::getMaxStats(int statNumber) { //Renvoie la stat statNumber maximale du Pokemon courrant
	if (statNumber==0) {										//La formule
		return (10 + pokeSpecs[this -> ID][0]*lvl/50);			//d'attribution 
	}															//des hp est
	else {														//différentes de
		return 5 + pokeSpecs[this -> ID][statNumber]*lvl/50;	//celles des
	}															//autres statistiques
}

int Pokemon::getActualStats(int ID) {return this -> actualStats[ID];}

void Pokemon::setActualStats(int statNumber, int newStatValue) { //Modifie la stats statNumber actuel d'un Pokemon par la stat newStatValue, quite à le mettre KO
	if (statNumber!=0) {								//Si on veut modifier une
		this -> actualStats[statNumber] = newStatValue;	//autre stat que les hp,
	}													//alors on la modifie
	else {													//Sinon, on 
		if (newStatValue<=0){								//modifie les hp
			this -> actualStats[0] = 0;						//du Pokemon
			Serial.print(this -> getName());				//courrant en
			Serial.println(F(" est KO !"));					//faisant en sorte
			delay(1000);									//de ne niexcéder
		}													//les hp max du
		else if (newStatValue>this -> getMaxStats(0)) {		//Pokemon courrant,
			this -> actualStats[0] = getMaxStats(0);		//ni d'aller en
		}													//dessousdu 0, cas
		else {												//dans lequel on 
			this -> actualStats[0] = newStatValue;			//précise que le
		}													//Pokemon courrant
	}														//est KO sur l'écran
}


int Pokemon::getType(int typeNumber) {return this -> types[typeNumber];}

Move Pokemon::getMove(int moveNumber) {return this -> moveList[moveNumber];}
void Pokemon::setMove(int moveNumber, Move move) {this -> moveList[moveNumber] = move;}

Statut Pokemon::getStatut(int statutNumber) {return this -> statutList[statutNumber];}
void Pokemon::setStatut(int statutNumber, int statut) {this -> statutList[statutNumber] = Statut(statut);}

int Pokemon::getTurnToWait() {return this -> turnToWait;}		void Pokemon::setTurnToWait(int turnToWait) {this -> turnToWait = turnToWait;}
int Pokemon::getLastMove() {return this -> lastMove;}			void Pokemon::setLastMove(int lastMove) {this -> lastMove = lastMove;}

int Pokemon::IAMove() { //Renvoie le Move choisi par l'IA (prochainement : IA différente selon un entier entré, propre à l'adversaire)
	int move; //On définie une variable dans laquelle on stock le Move choisie
	while (true) {									//On boucle jusqu'à
		move = rand() % 4;							//obtenir un nombre
		if (this -> moveList[move].getID()<255) {	//aléatoire correspondant
			this -> lastMove = move;				//à la position d'un Move
			return move;							//définie chez l'IA,
		}											//c'est à dire un Move
	}												//non vide
}


float Pokemon::getTypeCoeffMultiplier(Pokemon &target, Move selectedMove) { //Renvoie le coefficient multiplicateur final d'un Move sur un type
	float coeff1 = typeCoeffMultiplier(selectedMove.getType(), target.getType(0));								//On multiplie *les deux coefficients
	float coeff2 = typeCoeffMultiplier(selectedMove.getType(), target.getType(1));								//multiplicateurs du Move sur les deux
	if (selectedMove.getType()==this -> getType(0)||selectedMove.getType()==this -> getType(1)) {coeff1*=1.5;}	//types de l'adversaire (type nul compris)
	return coeff1*coeff2;																						//avec *le potentiel STAB du Move
}

bool Pokemon::verifDebutStatuts() { //Applique les effets de tous les statuts du Pokemon courrant au début d'un tour et détermine s'il est apte à attaquer
	bool peutAttaquer=1; //On définie une variable dans laquelle on stock l'aptitude du Pokemon courrant à attaquer

	/*On commence par appliquer
	les effets des statuts
	secondaires, pour par exemple
	être capable d'appliquer des
	dégâts de confusion*/

	switch(this -> statutList[1].getID()) { //La procédure dépends du statut secondaire
		case 6:	//CONFUSION
			this -> statutList[1].setConfusionTurns(this -> statutList[1].getConfusionTurns()-1);	//Si le Pokemon
			if (this -> statutList[1].getConfusionTurns()==0) {										//courant est
				Serial.print(this -> getName());													//confus, alors
				Serial.println(F(" n'est plus confus !"));											//on lui retire
				delay(1000);																		//un tour de
				this -> statutList[1].setID(1);														//confusion, et
				break;																				//dans le cas
			}																						//où ses tours
			Serial.print(this -> getName());Serial.println(F(" est confus !"));delay(1000);			//de confusion
			if (49>rand() % 100) {																	//restant sont
				Serial.print(this -> getName());													//nuls, alors
				Serial.println(F(" se blesse dans sa confusion !"));								//il est apte
				delay(1000);																		//à attaquer
				this -> setActualStats(0,this -> getActualStats(0)-(((this -> lvl*0.4+2)*this -> actualStats[1]*40/(this -> actualStats[3]*50))+2));
				peutAttaquer=0;																		//Confus, il se
			}																						//blesse à 50%
			break;
		default:
			break;
	}

	if (peutAttaquer==0) {return 0;} //Si le Pokemon courrant n'est finalement pas apte à attaquer, on s'arrête ici

	switch(this -> statutList[0].getID()) { //La procédure dépends du statut primaire
		case 1: //BRULÛRE
			if (this -> statutList[0].getBrulureAtkDown()==0) {							//Si le Pokemon courrant est brûlé,
				this -> actualStats[1]*=0.5;this -> statutList[0].setBrulureAtkDown(1);	//on lui divise son attaque par 2
			}																			//si ce n'est pas déjà fait
			break;
		case 2: //GEL
			Serial.print(this -> getName());									//Si le
			Serial.println(F(" est gelé !"));									//Pokemon
			delay(1000);														//courant est
			if (19>rand() % 100) {												//gelé, alors
				Serial.print(this -> getName());								//il a 20%
				Serial.println(F(" n'est plus gelé !"));						//de chances
				delay(1000);													//de se
				this -> statutList[0].setID(0);									//dégeler, et
			}																	//dans le cas
			else {																//contraire
				Serial.print(this -> getName());								//il ne
				Serial.println(F(" est gelé, et n'a pas pu attaquer !"));		//sera pas
				delay(1000);peutAttaquer=0;										//apte à
			}																	//attaquer
			break;
		case 3: //PARALYSIE
			Serial.print(this -> getName());												//Si le Pokemon
			Serial.println(F(" est paralysé !"));											//courrant est
			delay(1000);																	//paralysé, on
			if (this -> statutList[0].getParalysieVitDown()==0) {							//lui divise sa
				this -> actualStats[5]*=0.5;this -> statutList[0].setParalysieVitDown(1);	//vitesse par 2
			}																				//si ce n'est
			if (49>rand() % 100) {															//pas déjà fait,
				Serial.print(this -> getName());											//et dans tous
				Serial.println(F(" est paralysé, et n'a pas pu attaquer !"));				//les cas il a
				delay(1000);																//50% de chance
				peutAttaquer=0;																//d'être apte
			}																				//à attaquer
			break;
		case 5: //SOMMEIL
			Serial.print(this -> getName());													//Si le
			Serial.println(F(" est endormi !"));												//Pokemon
			delay(1000);																		//courrant
			this -> statutList[0].setSommeilTurns(this -> statutList[0].getSommeilTurns()-1);	//est endormi,
			if (this -> statutList[0].getSommeilTurns()==0) {									//on lui
				Serial.print(this -> getName());												//retire un
				Serial.println(F(" n'est plus endormi !"));										//tour de
				delay(1000);																	//sommeil, et
				this -> statutList[0].setID(0);													//dans le cas
			}																					//où ses tours
			else {																				//de sommeil
				Serial.print(this -> getName());												//restant sont
				Serial.println(F(" est endormi, et n'a pas pu attaquer !"));					//nuls, alors
				delay(1000);peutAttaquer=0;														//il redevient
			}																					//apte à attaquer
			break;
		default:
			break;
	}
	return peutAttaquer; //On renvoie l'aptitude à attaquer du Pokemon courrant
}

void Pokemon::verifFinStatuts(Pokemon &target) { //Applique les effets de tous les statuts du Pokemon courrant à la fin d'un tour, quitte à le rendre KO

	/*On commence par appliquer
	les effets des statuts
	secondaires, pour par exemple
	être capable d'appliquer le vol
	de vie de vampigraine avant de
	mettre le Pokemon courant KO
	par brûlure ou empoisonnement*/

	switch(this -> statutList[1].getID()) { //La procédure dépends du statut secondaire
		case 7: //VAMPIGRAINE
			Serial.print(this -> getName());													//Si le Pokemon
			Serial.println(F(" souffre de vampigraine !"));										//courant est
			delay(1000);																		//victime de
			this -> setActualStats(0,this -> actualStats[0]-this -> getMaxStats(0)/8);			//VAMPIGRAINE,
			if (target.getActualStats(0)>0) {													//alors la cible
				target.setActualStats(0,target.getActualStats(0)+this -> getMaxStats(0)/16);	//lui vole de
			}																					//la vie
		default:
			break;
	}

	if (this->getActualStats(0)==0) {return;} //SI le Pokemon courant est KO, on s'arrête ici

	switch(this -> statutList[0].getID()) { //La procédure dépends du statut primaire
		case 1: //BRULÛRE
			Serial.print(this -> getName());													//Si le Pokemon
			Serial.println(F(" souffre de sa brûlure !"));										//courant est
			delay(1000);																		//brûlé, alors
			this -> setActualStats(0,this -> actualStats[0] - this -> getMaxStats(0)/8);		//il perds des
			break;																				//HP
		case 4: //POISON
			Serial.print(this -> getName());													//Si le Pokemon
			Serial.println(F(" souffre du poison !"));											//courant est
			delay(1000);																		//empoisonné, alors
			this -> setActualStats(0,this -> actualStats[0] - this -> getMaxStats(0)/8);		//il perds des
			break;																				//HP
		default:
			break;
	}

	if (this -> turnToWait>0) {this -> turnToWait-=1;} //Si le nombre de tours à attendre est positif, on le décrémente de 1
}

void Pokemon::useMoveStartingText(Move selectedMove) { //Affiche le texte de début d'utilisation de Move
	Serial.print(this -> getName());
	Serial.print(F(" utilise "));
	Serial.println(selectedMove.getName());
	delay(1000);
}

void Pokemon::useMoveChargeMove(Move selectedMove) { //Affiche le text relatif à la charge d'une attaque, et incrémente le nombre de tours d'attente
	Serial.print(this -> getName());
	Serial.println(F(" charge son attaque"));
	delay(1000);
	this -> turnToWait=2;
}

void Pokemon::useMoveDamaging(Pokemon &target, Move selectedMove) {
	if (selectedMove.getPwr()!=0) { //Si le Move est censé appliquer des dégâts
		int pokemonAtk; 	//On définie deux variables dans laquelle on stock l'attaque (spéciale ou non)
		int targetDef;		//du Pokemon courant et la défense de la cible associée
		if (selectedMove.getTypeOfDamage()==0) { 		//On
			pokemonAtk = this -> getActualStats(1);		//associe
			targetDef = target.getActualStats(3);		//le
		}												//type
		else {											//d'attaque
			pokemonAtk = this -> getActualStats(2);		//et
			targetDef = target.getActualStats(4);		//de
		}												//défense

		/*Calcul des dommages
		selon l'attaque, la 
		défense et les types
		des deux Pokemon*/

		int damage = (((this -> lvl*0.4+2)*pokemonAtk*selectedMove.getPwr()/(targetDef*50))+2)*getTypeCoeffMultiplier(target,selectedMove);

		if (9>rand() % 100) { //Le Pokemon courant effectue des dégâts critiques avec 10% de chances, ce qui multiplie ses dégâts par 1.5
			damage *=1.5;
			Serial.println(F("Coup critique !"));
			delay(1000);
		}
		target.setActualStats(0,target.getActualStats(0)-damage); //On retire aux HP de la cible les dégâts occasionnés

		if (getTypeCoeffMultiplier(target,selectedMove)>1.5) {		//On
			Serial.println(F("C'est super efficace !"));			//affiche
			delay(1000);											//sur
		}															//l'écran
		if (getTypeCoeffMultiplier(target,selectedMove)<0.75) {		//l'efficacité
			Serial.println(F("Ce n'est pas très efficace !"));		//potentielle
			delay(1000);											//du
		}															//Move
	}
}

void Pokemon::useMoveStatsChange(Pokemon &target, Move selectedMove) {
	if (selectedMove.getStatsChange()!=0) { //Si le Move est censé modifier des stats
		Serial.print(_statNames[selectedMove.getStatsID()]);	//On affiche sur l'écran la stat modifiée
		Serial.print(F(" de "));
		if (selectedMove.getStatsTarget()==1) { //Si la cible de la modification de stat est l'adversaire, on modifie sa stat
			Serial.print(target.getName()); //On affiche sur l'écran le nom de l'adversaire, cible de la modification de stat
			target.setActualStats(selectedMove.getStatsID(),target.getActualStats(selectedMove.getStatsID())*(selectedMove.getStatsChange())/100);
		}
		else { //Sinon, on fait la même chose avec le Pokemon courant
			Serial.print(this -> getName());
			this -> setActualStats(selectedMove.getStatsID(),this -> getActualStats(selectedMove.getStatsID())*(selectedMove.getStatsChange())/100);
		}
		if (selectedMove.getStatsChange()>100) { 	//On
			Serial.println(F(" augmente !"));		//affiche
			delay(1000);							//sur
		}											//l'écran
		else {										//la
			Serial.println(F(" baisse !"));			//variation
			delay(1000);							//de
		}											//stat
	}
}

void Pokemon::useMoveStatutChange(Pokemon &target, Move selectedMove){
	if ((selectedMove.getStatutRate()!=0									//Si le Move est supposé appliquer un statut,
	|| (moveStatutsStats[selectedMove.getStatutID()][3]>rand() % 100))		//que le jet de posage de statut est bon et
	&& target.getActualStats(0)>0) {	 									//que la cible n'est pas KO
		Serial.print(target.getName());	//On affiche sur l'écran le nom de la cible
		if (selectedMove.getStatutID()<6) {							//disjonction
			target.setStatut(0,selectedMove.getStatutID());			//de cas,
		}															//on applique
		else {														//un statut
			target.setStatut(1,selectedMove.getStatutID());			//secondaire
		}															//ou primaire
		Serial.print(F(" est victime de ")); //On affiche sur l'écran le statut appliqué
		Serial.println(_statutNames[selectedMove.getStatutID()]);
		delay(1000);
	}
}

void Pokemon::useMove(Pokemon &target, Move selectedMove) { //Permet à un Pokemon d'effectuer un Move
	if (this -> verifDebutStatuts()) { //Si les statuts du Pokemon lui permettent d'attaquer,
		useMoveStartingText(selectedMove); //On affiche le text de début d'utilisation de Move
		if (selectedMove.getPreci()<rand() % 100 + 1) {			//On effectue le
			Serial.println(F("Mais il rate son attaque !"));	//jet de précision
			delay(1000);										//pour déterminer si
		}														//l'attaque est ratée
		else {
			if (selectedMove.getStatutRate()==101 && this -> turnToWait==0) { //Si le Pokemon doit se charger un tour avant d'attaquer, alors il se charge
				useMoveChargeMove(selectedMove);
			}
			else {
				useMoveDamaging(target, selectedMove); //On effectue des dégâts si le Move est supposé le faire

				useMoveStatsChange(target, selectedMove); //On applique un changement de stats si le Move est supposer le faire

				useMoveStatutChange(target, selectedMove); //On applique un statut si le Move est supposer l'appliquer
			}
		}
	}
	if (this -> actualStats[0]>0) {this -> verifFinStatuts(target);} //Si le Pokemon courrant n'est pas KO on lui applique les effets de fin de tour de ses statuts
}
	/*Méthodes*/

/*---------------------------------------------------------------CLASS  POKEMON----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------CLASS EQUIPE-----------------------------------------------------------------------------------*/

	/*Constructeurs*/
Equipe::Equipe(String name){ //Construit une Equipe en particulier, sans Pokemons initiaux
	this -> name = name;
	this -> pokemonFrontID=0;
}
Equipe::Equipe(String name, Pokemon pokemon1, Pokemon pokemon2) { //Construit une Equipe en particulier, avec deux Pokemons initiaux
	this -> name = name;
	this -> pokemons[0] = pokemon1;		this -> pokemons[1] = pokemon2;
	this -> pokemonFrontID=0;
}
	/*Constructeurs*/

	/*Méthodes*/

bool Equipe::changerPokemon(int pokemonFrontID) { //Permet de changer de Pokemon en combat
	if (this -> pokemons[pokemonFrontID].getID()==255) {					//Si le Pokemon sélectionné
		Serial.println("ERROR : Le Pokemon sélectionné n'est pas valide");	//est le Pokemon vide, alors
		return 0;															//on renvoie une erreur (false)
	}
	else if (this -> pokemonFrontID==pokemonFrontID) {			//Si le Pokemon sélectionné
		Serial.println(F("Ce pokemon est déjà en combat !"));	//est le même que celui sur
		return 0;												//le terrain, on renvoie false
	}
	else if (this -> pokemons[pokemonFrontID].getActualStats(0)>0) {	//Si le Pokemon
		this -> pokemons[this -> pokemonFrontID].setStatut(1,0);		//sélectionné
		this -> pokemonFrontID = pokemonFrontID;						//n'est pas
		Serial.print(this -> name);										//KO, alors
		Serial.print(F(" envoie "));									//on effectue
		Serial.println(this -> pokemons[pokemonFrontID].getName());		//le changement
		delay(1000);													//de Pokemon et
		return 1;														//on renvoie true
	}
	else {															//Si le 
		Serial.print(this -> pokemons[pokemonFrontID].getName());	//Pokemon
		Serial.println(F(" est OK !"));								//sélectionné
		delay(1000);												//est KO, on
		return 0;													//renvoie false
	}
}

void Equipe::normalizeEquipe() { //Rétablie les stats maximales de l'Equipe courrante, en prenant en compte les diminutions dues à une BRÛLURE ou PARALYSIE
	for (int i=0;i<6;i++) {
		this -> pokemons[i].setActualStats(1,this -> pokemons[i].getMaxStats(1));	//On
		this -> pokemons[i].setActualStats(2,this -> pokemons[i].getMaxStats(2));	//rétablie
		this -> pokemons[i].setActualStats(3,this -> pokemons[i].getMaxStats(3));	//les
		this -> pokemons[i].setActualStats(4,this -> pokemons[i].getMaxStats(4));	//stats
		this -> pokemons[i].setActualStats(5,this -> pokemons[i].getMaxStats(5));	//maximales
		if (this -> pokemons[i].getStatut(0).getID() == 1) {		//On applique
			this -> pokemons[i].setActualStats(1,this -> pokemons[i].getActualStats(1)/2);this -> pokemons[i].getStatut(1).setBrulureAtkDown(1);
		}															//les modifications
		if (this -> pokemons[i].getStatut(0).getID() == 3) {		//de stats dûes
			this -> pokemons[i].setActualStats(5,this -> pokemons[i].getActualStats(5)/2);this -> pokemons[i].getStatut(1).setParalysieVitDown(1);
		}															//à la BRÛLURE ou
		if (this -> pokemons[i].getStatut(1).getID() == 6 || this -> pokemons[i].getStatut(1).getID() == 7) {
			this -> pokemons[i].setStatut(1,0);						//à la PARALYSIE
		}
	}
}

bool Equipe::verifLoose() { //Vérifie si l'Equipe courrante a perdu
	for (int i=0;i<6;i++) {
		if (this -> pokemons[i].getActualStats(0)>0) {return 0;}
	}
	return 1;
}

void Equipe::battleStartingText(Equipe &target) { //Affiche sur l'écran le texte de début de Combat
	if (ecrire1==0) {
		Serial.print(F("Un combat est lancé par "));
		Serial.println(target.name);
		delay(1000);
		Serial.print(target.name);
		Serial.print(F(" envoie "));
		Serial.println(target.pokemons[target.pokemonFrontID].getName());
		delay(1000);
		Serial.print(this -> pokemons[this -> pokemonFrontID].getName());
		Serial.println(F(", en avant !!"));
		delay(1000);
		this -> normalizeEquipe(); //on normalise les stats de l'Equipe courrante
		ecrire1=1; //On empêche ce texte de se réécrire plusieurs fois de suite
	}
}

void Equipe::battleActionText(Equipe &target) { //Affiche sur l'écran le texte relatif au choix d'action
	if (ecrire2==0 && this -> pokemons[this -> pokemonFrontID].getTurnToWait()==0) { //Si le texte n'a pas encore été écrit et que l'on a le choix d'action
		Serial.print(F("Que voulez-vous faire ? "));
		Serial.print(this -> pokemons[this -> pokemonFrontID].getMove(0).getName());
		Serial.print(F(", "));
		Serial.print(this -> pokemons[this -> pokemonFrontID].getMove(1).getName());
		Serial.print(F(", "));
		Serial.print(this -> pokemons[this -> pokemonFrontID].getMove(2).getName());
		Serial.print(F(", "));
		Serial.println(this -> pokemons[this -> pokemonFrontID].getMove(3).getName());
		ecrire2=1; //On empêche ce texte de se réécrire plusieurs fois de suite
	}
}

int Equipe::battleNoMoveChoice(Equipe &target, int input) { //Force une action dans le cas où l'on n'a pas le choix d'action
	if (this -> pokemons[this -> pokemonFrontID].getTurnToWait()==1) { 	//Si le Pokemon au combat de l'Equipe courrante
		input = this -> pokemons[this -> pokemonFrontID].getLastMove(); 	//a chargé une attaque au tour précédent, alors
	}																	//il utilisera cette attaque au tour actuel

	return input; //On renvoie l'attaque qu'utilisera le Pokemon au combat de l'Equipe courrante
}

void Equipe::battleChangePokemon(Equipe &target, int input) { //Permet le changement de Pokemon en tant qu'action
	input=Serial.read()-49;
	if (this -> changerPokemon(input)) { 	//Si le changement de Pokemon est un succès, on
		normalizeEquipe();				 	//normalise les stats et statut de l'Equipe courrante

		/*L'adversaire utilise
		une attaque sur nous*/
		target.pokemons[target.pokemonFrontID].useMove(this -> pokemons[this -> pokemonFrontID], target.pokemons[target.pokemonFrontID].getMove(target.pokemons[target.pokemonFrontID].IAMove()));
		
		ecrire2=0; //On permet au texte de se réécrire
	}
	Serial.print(F("Vous avez "));Serial.print(this -> pokemons[this -> pokemonFrontID].getActualStats(0));Serial.println(F(" HP"));
	Serial.print(F("Votre adversaire a "));Serial.print(target.pokemons[target.pokemonFrontID].getActualStats(0));Serial.println(F(" HP"));
}

void Equipe::battleMovePhase(Equipe &target, int input) { //Permet aux deux Pokemons d'effectuer leur Move

	/*L'ordre d'attaque se décide par la
	stat de vitesse des deux Pokemons, et
	le Pokemon jouant en second attaque
	seulement s'il n'est pas KO*/

	//Cas où le Pokemon au combat de l'Equipe courrante est plus rapide que le Pokemon au combat de l'Equipe adverse
	if (this -> pokemons[this -> pokemonFrontID].getActualStats(5) >= target.pokemons[target.pokemonFrontID].getActualStats(5)) {
		this -> pokemons[this -> pokemonFrontID].useMove(target.pokemons[target.pokemonFrontID], this -> pokemons[this -> pokemonFrontID].getMove((input)));
		if (target.pokemons[target.pokemonFrontID].getActualStats(0)>0) { //Le Pokemon jouant en second attaque seulement s'il n'est pas KO
			target.pokemons[target.pokemonFrontID].useMove(this -> pokemons[this -> pokemonFrontID], target.pokemons[target.pokemonFrontID].getMove(target.pokemons[target.pokemonFrontID].IAMove()));
		}
	}
	//Cas où le Pokemon au combat de l'Equipe adverse est plus rapide que le Pokemon au combat de l'Equipe courrante
	else {
		target.pokemons[target.pokemonFrontID].useMove(this -> pokemons[this -> pokemonFrontID], target.pokemons[target.pokemonFrontID].getMove(target.pokemons[target.pokemonFrontID].IAMove()));
		if (this -> pokemons[this -> pokemonFrontID].getActualStats(0)>0) { //Le Pokemon jouant en second attaque seulement s'il n'est pas KO
			this -> pokemons[this -> pokemonFrontID].useMove(target.pokemons[target.pokemonFrontID], this -> pokemons[this -> pokemonFrontID].getMove((input)));
		}
	}
	this -> pokemons[this -> pokemonFrontID].setLastMove(input); //On met à jour le dernier Move utilisé

	ecrire2=0; //On permet au texte de se réécrire

	Serial.print(F("Vous avez "));Serial.print(this -> pokemons[this -> pokemonFrontID].getActualStats(0));Serial.println(F(" HP"));
	Serial.print(F("Votre adversaire a "));Serial.print(target.pokemons[target.pokemonFrontID].getActualStats(0));Serial.println(F(" HP"));
}

bool Equipe::battleEndText(Equipe &target) {
	if (this -> verifLoose()==1 || target.verifLoose() == 1) { //Si une des deux équipes a perdu
		if (this -> verifLoose()==1) { //On
			Serial.print(this -> name); //affiche
		}								//le
		else {							//nom
			Serial.print(target.name);	//du
		}								//perdant
		Serial.println(F(" a perdu !"));
			return 1; //On met fin combat
		}
	return 0; //Sinon, on ne fait rien
}

void Equipe::battlePokemonKoText(Equipe &target, int input) {
	if (target.pokemons[target.pokemonFrontID].getActualStats(0)==0) { //Si le Pokemon KO est celui de l'adversaire
		target.changerPokemon(target.pokemonFrontID+1); //L'dversaire change de Pokemon
	}
	else if (this -> pokemons[this -> pokemonFrontID].getActualStats(0)==0 && this -> verifLoose()==0) { //Si le Pokemon KO est le notre
		if (ecrire3==0) {
			Serial.println(F("Quel Pokemon envoyer ? "));ecrire3=1;
		}
		if  (Serial.available()>0) { //Si un Pokemon de remplacement a été choisi
			input = Serial.read()-49; //On récupère la position du Pokemon dans l'Equipe
			this -> changerPokemon(input); //On envoie le Pokemon au combat
			ecrire3 =0;
		}
	}
}

void Equipe::battle(Equipe &target) {
	int input=0; //On définie une variable dans laquelle on stock l'action choisie

	battleStartingText(target); //On affiche le text de début de combat

	if (target.pokemons[target.pokemonFrontID].getActualStats(0)!=0 		///S'il reste des
	&& this -> pokemons[this -> pokemonFrontID].getActualStats(0)!=0) {		//Pokemons en vie

		battleActionText(target); //On affiche le text de choix d'action

		if (Serial.available()>1 || this -> pokemons[this -> pokemonFrontID].getTurnToWait()==1) { //Si une action a été choisie ou si l'on a pas le choix d'action
			input = Serial.read()-49; //On récupère l'action choisie

			input = battleNoMoveChoice(target, input); //On modifie éventuellement l'action choisie si l'on a pas le choix d'action

			if (input==4) {battleChangePokemon(target, input);} //Si on souhaite changer de Pokemon, alors on change de Pokemon

			else {battleMovePhase(target, input);} //Sinon, on procède à la phase d'attaque

			if (battleEndText(target)) {return;} //Si l'une des deux équipes est vaincue, on met un terme au combat
		}
	}
	else battlePokemonKoText(target, input); //S'il ne reste pas de Pokemons en vie on force le changement de Pokemon

	viderBuffer(); //On vide le buffer pour éviter des soucis d'entrée sur le moniteur série
}
	/*Méthodes*/

/*----------------------------------------------------------------CLASS EQUIPE-----------------------------------------------------------------------------------*/