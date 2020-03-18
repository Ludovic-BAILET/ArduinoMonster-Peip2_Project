#include <arduino.h>

#ifndef Pokemon_h
#define Pokemon_h

int const tailleListePokemon = 2;
int const tailleListeMove = 12;
int const tailleListeType = 18;
int const tailleListeStatut = 8;
int const tailleListeStats = 7;


/*-------------------------------------------------------------METHODES GENERALES--------------------------------------------------------------------------------*/

String getTypeName(int ID);
float typeCoeffMultiplier(int moveID, int targetID);
void viderBuffer();

/*-------------------------------------------------------------METHODES GENERALES--------------------------------------------------------------------------------*/
/*----------------------------------------------------------------CLASS STATUT-----------------------------------------------------------------------------------*/

class Statut {
	public:
		Statut(int ID);
		Statut();

		int getID();					void setID(int ID);
		String getName();

		bool getBrulureAtkDown();		void setBrulureAtkDown(bool brulureAtkDown);
		bool getParalysieVitDown();		void setParalysieVitDown(bool paralysieVitDown);
		int getSommeilTurns();			void setSommeilTurns(int sommeilTurns);
		int getConfusionTurns();		void setConfusionTurns(int confusionTurns);

	private:
		int ID;
		bool brulureAtkDown;
		bool paralysieVitDown;
		int sommeilTurns;
		int confusionTurns;
};

/*----------------------------------------------------------------CLASS STATUT-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------CLASS MOVE------------------------------------------------------------------------------------*/

class Move {
	public:
		Move(int ID);
		Move();

		int getID();				String getName();

		int getPwr();				int getPreci();			int getType();
		
		int getTypeOfDamage();		int getStatsTarget();

		int getStatsID();			int getStatsChange();		int getStatutID();			int getStatutRate();		

	private :
		int ID;
};

/*-----------------------------------------------------------------CLASS MOVE------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------CLASS  POKEMON----------------------------------------------------------------------------------*/

class Pokemon {
	public :

		Pokemon(int ID,int lvl);
		Pokemon();

		void setID(int ID);

		int getID();			String getName();
		int getXp();			void setXp(int xp);
		int getLvl();			void setLvl(int lvl);

		int getMaxStats(int statNumber);			int getActualStats(int ID);	void setActualStats(int ID, int stat);

		int getType(int typeNumber);
		
		Move getMove(int moveNumber);				void setMove(int moveNumber, Move move);

		Statut getStatut(int statutNumber);			void setStatut(int statutNumber, int statut);

		int getTurnToWait();						void setTurnToWait(int turnToWait);
		int getLastMove();							void setLastMove(int lastMove);

		int IAMove();
		float getTypeCoeffMultiplier(Pokemon &target, Move selectedMove);
		bool verifDebutStatuts();
		void verifFinStatuts(Pokemon &target);

		void useMoveStartingText(Move selectedMove);
		void useMoveChargeMove(Move selectedMove);
		void useMoveDamaging(Pokemon &target, Move selectedMove);
		void useMoveStatsChange(Pokemon &target, Move selectedMove);
		void useMoveStatutChange(Pokemon &target, Move selectedMove);
		void useMove(Pokemon &target, Move selectedMove);

	private :

		int ID;
		int xp;
		int lvl;

		int maxStats[6];
		int actualStats[6];
		int types[2];
		Move moveList[4];
		Statut statutList[2];
		int turnToWait;
		int lastMove;
};

/*---------------------------------------------------------------CLASS  POKEMON----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------CLASS EQUIPE-----------------------------------------------------------------------------------*/

class Equipe {
	public :
		Equipe(String name);
		Equipe(String name, Pokemon pokemon1, Pokemon pokemon2);


		bool changerPokemon(int pokemonFrontID);
		void normalizeEquipe();
		bool verifLoose();

		void battleStartingText(Equipe &target);
		void battleActionText(Equipe &target);
		int battleNoMoveChoice(Equipe &target, int input);
		void battleMovePhase(Equipe &target, int input);
		void battleChangePokemon(Equipe &target, int input);
		bool battleEndText(Equipe &target);
		void battlePokemonKoText(Equipe &target, int input);
		void battle(Equipe &target);


		int pokemonFrontID;
		String name;
		Pokemon pokemons[6];

	private :
};

/*----------------------------------------------------------------CLASS EQUIPE-----------------------------------------------------------------------------------*/

#endif