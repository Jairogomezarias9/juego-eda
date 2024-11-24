
#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SherlokHumus


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  using vecbool = vector<bool>;
  using matbool = vector<vecbool>;

  matbool enemy_warriors;
  matbool enemy_builders;

  int CI[4]={ 1, 0, -1, 0};
  int CJ[4]={ 0, 1, 0, -1};

  bool noEsticSol(Pos p){
    if (((cell(p.i+1,p.j).id != -1) and (citizen(cell(p.i+1,p.j).id).player !=me()))
    or ((cell(p.i-1,p.j).id != -1) and (citizen(cell(p.i-1,p.j).id).player !=me()))
    or ((cell(p.i,p.j+1).id != -1) and (citizen(cell(p.i,p.j+1).id).player !=me()))
    or ((cell(p.i,p.j-1).id != -1) and (citizen(cell(p.i,p.j-1).id).player !=me()))) return true;
    return false;
  }

  bool noMoro(Citizen enemic, Citizen jo){
    if (life_lost_in_attack())
    if(enemic.weapon == Hammer){
      if (hammer_strength_attack() >= jo.life) return false;
      else return true;
    }
    else if (enemic.weapon == Gun){
      if (gun_strength_attack() >= jo.life) return false;
      else return true;
    }
    else{
      if (bazooka_strength_attack() >= jo.life) return false;
      else return true;
    }
  }

  bool elGuanyo(Citizen& enemic, Citizen& jo){
    if (enemic.weapon == Hammer && jo.weapon != Hammer or enemic.weapon == Gun && jo.weapon == Bazooka){
      if (enemic.life < jo.life) return true;
      else if (jo.weapon == Bazooka && enemic.life <= bazooka_strength_attack() or enemic.life <= gun_strength_attack()) return true;
      else return false;
    }
  }

  Dir dijkstra(Pos p, CitizenType type){
    matbool visitats(board_rows(), vecbool(board_cols(), false));
    priority_queue<pair<int, pair<Pos, Dir>>, vector<pair<int, pair<Pos, Dir>>>, greater<pair<int, pair<Pos, Dir> >> > pq;
    pq.push({0, {{p.i, p.j}, Up}});
    visitats[p.i][p.j] = true;
    bool primer = true;
    while (!pq.empty()){
      auto pq_top = pq.top();
      pq.pop();

      for (int i = 0; i < 4; ++i){
        int actual = pq_top.first;
        int iact = pq_top.second.first.i + CI[i];
        int jact = pq_top.second.first.j + CJ[i];
        if (pos_ok(iact, jact) && !visitats[iact][jact]){
          visitats[iact][jact] = true;
          /*if (primer){
            if(i==0) pq_top.second.second = Down;
            if(i==1) pq_top.second.second = Right;
            if(i==2) pq_top.second.second = Up;
            if(i==3) pq_top.second.second = Left;
          }*/

          if ((cell(iact,jact).type != Building) && (citizen(cell(iact,jact).id).player != me())){ 
            if (type == Builder){ //IF BUILDER
              if (noEsticSol(p)){ 
                if (cell(p).resistance == -1){
                  if (cell(iact, jact).resistance > 0 && cell(iact, jact).b_owner == me()){
                    if (pq_top.second.second == Up) return Up;
                      else if (pq_top.second.second == Down) return Down;
                      else if (pq_top.second.second == Right) return Right;
                      else if (pq_top.second.second == Left) return Left;
                  }
                  else{
                    if (pq_top.second.second == Up){
                      if (pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                      if (pos_ok(p+Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                      if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                    }
                    else if (pq_top.second.second == Down){
                        if(pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                        if (pos_ok(p+ Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                        if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                    }
                    else if (pq_top.second.second == Right){
                        if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                        if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                        if (pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                    }
                    else if (pq_top.second.second == Left){
                        if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                        if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                        if (pos_ok(p+Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                    }
                  }
                }
              }
              else{
                if (citizen(cell(p).id).life <= 0.5*builder_ini_life()){
                  if (cell(iact, jact).bonus == Food){
                    if (pq_top.second.second == Up) return Up;
                    else if (pq_top.second.second == Down) return Down;
                    else if (pq_top.second.second == Right) return Right;
                    else if (pq_top.second.second == Left) return Left;
                  }
                  else if (cell(iact, jact).bonus == Money){
                    if (pq_top.second.second == Up) return Up;
                    else if (pq_top.second.second == Down) return Down;
                    else if (pq_top.second.second == Right) return Right;
                    else if (pq_top.second.second == Left) return Left;
                  }
                }
                else{
                  if (cell(iact, jact).id != -1 && citizen(cell(iact, jact).id).type == Warrior && barricades(me()).size() < 3){
                    build(cell(p).id, pq_top.second.second);
                    if (pq_top.second.second == Up) return Down;
                    else if (pq_top.second.second == Down) return Up;
                    else if (pq_top.second.second == Right) return Left;
                    else return Right;
                  }
                  if (cell(iact, jact).bonus == Money){
                    if (pq_top.second.second == Up){
                      return Up;
                    }
                    else if (pq_top.second.second == Down){
                      return Down;
                    }
                    else if (pq_top.second.second == Right){
                      return Right;
                    }
                    else if (pq_top.second.second == Left){
                      return Left;
                    }
                  }
                  else if (cell(iact, jact).bonus == Food){
                    if (pq_top.second.second == Up){
                      return Up;
                    }
                    else if (pq_top.second.second == Down){
                      return Down;
                    }
                    else if (pq_top.second.second == Right){
                      return Right;
                    }
                    else if (pq_top.second.second == Left){
                      return Left;
                    }
                  }
                }
              }
            }
            else{ //IF WARRIOR
                if (is_day()){
                    if (citizen(cell(p).id).life <= 0.3*warrior_ini_life()){
                      if (cell(iact, jact).bonus == Food){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).weapon == Bazooka or (cell(iact, jact).weapon == Gun && cell(p).weapon == Hammer)){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).weapon == Gun){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).bonus == Money){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                    }
                    else{
                      if (cell(iact, jact).weapon == Bazooka or (cell(iact, jact).weapon == Gun && cell(p).weapon == Hammer)){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).weapon == Gun){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if(cell(iact, jact).id != -1 && noMoro(citizen(cell(iact, jact).id), citizen(cell(p).id))){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).bonus == Money){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                      else if (cell(iact, jact).bonus == Food){
                        if (pq_top.second.second == Up){
                            return Up;
                        }
                        else if (pq_top.second.second == Down){
                            return Down;
                        }
                        else if (pq_top.second.second == Right){
                            return Right;
                        }
                        else if (pq_top.second.second == Left){
                            return Left;
                        }
                      }
                    }
              }
              else{ //at night
                if (cell(iact, jact).id != -1 && cell(iact, jact).resistance == -1){ //SI troba un citizen (warrior o builder)
                  if (citizen(cell(iact, jact).id).type == Builder){ // Si es builder
                    if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                  }
                  else if (noMoro(citizen(cell(iact, jact).id), citizen(cell(p).id))){ // si es warrior i no em mata
                    if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                  }
                  else{ //es warrior i em mata, FUGA
                    if (pq_top.second.second == Up){
                      if (pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                      if (pos_ok(p+Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                      if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                    }
                    else if (pq_top.second.second == Down){
                      if(pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                      if (pos_ok(p+ Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                      if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                    }
                    else if (pq_top.second.second == Right){
                      if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                      if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                      if (pos_ok(p+Left) && (cell(p+Left).resistance == -1) && (cell(p+Left).b_owner == -1) && (cell(p+Left).id == -1) && cell(p+Left).type == Street) return Left;
                    }
                    else if (pq_top.second.second == Left){
                      if (pos_ok(p+Up) && (cell(p+Up).resistance == -1) && (cell(p+Up).b_owner == -1) && (cell(p+Up).id == -1) && cell(p+Up).type == Street) return Up;
                      if (pos_ok(p+Down) && (cell(p+Down).resistance == -1) && (cell(p+Down).b_owner == -1) && (cell(p+Down).id == -1) && cell(p+Down).type == Street) return Down;
                      if (pos_ok(p+Right) && (cell(p+Right).resistance == -1) && (cell(p+Right).b_owner == -1) && (cell(p+Right).id == -1) && cell(p+Right).type == Street) return Right;
                    }
                  }
                }
                else if (cell(iact, jact).weapon == Bazooka or (cell(iact, jact).weapon == Gun && cell(p).weapon == Hammer)){
                  if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                }
                else if (cell(iact, jact).weapon == Gun){
                  if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                }
                else if (cell(iact, jact).bonus == Food){
                  if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                }
                else if (cell(iact, jact).bonus == Money){
                  if (pq_top.second.second == Up){
                        return Up;
                    }
                    else if (pq_top.second.second == Down){
                        return Down;
                    }
                    else if (pq_top.second.second == Right){
                        return Right;
                    }
                    else if (pq_top.second.second == Left){
                        return Left;
                    }
                }
              } //FI ELSE AT NIGHT
            } 
            pq.push({actual+1,{{iact,jact},pq_top.second.second}});
          } 
        } 
      }
     primer = false;
    }
    return Up;
  } 
  /**
   * Play method, invoked once per each round.
   */
  
  virtual void play () {

    vector<int> mybuilders = builders(me());
    vector<int> mywarriors = warriors(me());

    //BUILDERS
    for (int i = 0; i < mybuilders.size(); ++i){
      Citizen cit = citizen(mybuilders[i]);
      Pos p = cit.pos;
      move(mybuilders[i], dijkstra(p, Builder));
    }

    //WARRIORS
    for (int i = 0; i < mywarriors.size(); ++i){
      Citizen cit = citizen(mywarriors[i]);
      Pos p = cit.pos;
      move(mywarriors[i], dijkstra(p, Warrior));
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
