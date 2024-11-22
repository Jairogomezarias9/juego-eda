#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Jairo


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

    // If nearly out of time, do nothing.
    double st = status(me());
    if (st >= 0.9) return;

    

    //movimiento magos

    //vector de las id de los magos
    vector<int> magos= wizards(me());





    //movimiento fantasma
    int fantasma=ghost(me());
    

    Pos derecha=unit(fantasma).pos+Right;
  
    
    if(pos_ok(derecha) and cell(derecha.i,derecha.j).is_empty()){
      move(fantasma,Left);


    }











  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
