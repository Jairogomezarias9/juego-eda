#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Jairo

struct PLAYER_NAME : public Player
{

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player *factory()
  {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  const int dx[4] = {-1, 1, 0, 0};
  const int dy[4] = {0, 0, -1, 1};

  const int dxx[8] = {0, 1, 1, 1, 0, 1, -1, -1};
  const int dyy[8] = {-1, -1, 1, 1, 1, -1, 0, -1};

  vector<Dir> direccions = {Up, Down, Left, Right};

  bool esta_su_mago_convirtiendose(Pos p)
  {
    int id = cell(p).id;
    if (id == -1)
    {
      return false;
    }
    Unit u = unit(id);
    return (u.player = !me() && u.type == Wizard && u.is_in_conversion_process());
  }
  bool es_fantasma_enemigo(Pos p)
  {
    int id = cell(p).id;
    if (id == -1)
    {
      return false;
    }
    Unit u = unit(id);
    return (u.player != me() and u.type == Ghost);
  }
  bool es_mago_enemigo(Pos p)
  {
    int id = cell(p).id;
    if (id == -1)
    {
      return false;
    }
    Unit u = unit(id);
    return (u.player != me() and u.type == Wizard);
  }

  bool es_libro(Pos p)
  {
    return (pos_ok(p) and cell(p).book);
  }

  bool is_empty(Pos p)
  {
    return (cell(p).id == -1 and cell(p).owner == -1);
  }

  bool no_es_mio(Pos p)
  {
    return cell(p).owner != me();
  }

  bool es_mio(Pos p)
  {
    return cell(p).owner == me();
  }

  // no está voldemort en las casillas adyacentes
  bool a_salvo(Pos p)
  {
    return false;
  }

  vector<Pos> Bfs(Pos pos)
  {
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false)); // matriz de casillas visitadas
    vector<vector<Pos>> parent(board_rows(), vector<Pos>(board_cols(), {-1, -1})); // matriz para rastrear el camino
    queue<Pos> q;                                                                  // cola para BFS
    q.push(pos);                                                                   // empiezo en la posición inicial
    visited[pos.i][pos.j] = true;

    Pos target = {-1, -1}; // posición del objetivo encontrado

    while (!q.empty())
    {
      Pos actual = q.front();
      q.pop();

      // Verificar si hemos encontrado un libro, mago o fantasma
      if (es_libro(actual) || es_mago_enemigo(actual) || es_fantasma_enemigo(actual))
      {
        target = actual;
        break;
      }

      // Explorar las cuatro direcciones
      for (int i = 0; i < 4; ++i)
      {
        Pos new_pos = actual + direccions[i];

        if (pos_ok(new_pos.i, new_pos.j) && !visited[new_pos.i][new_pos.j])
        {
          visited[new_pos.i][new_pos.j] = true;
          if (cell(new_pos).type != Wall and new_pos != pos_voldemort())
          {
            if (es_mago_enemigo(new_pos))
            {
              if (!esta_su_mago_convirtiendose(new_pos))
              {
                parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
                q.push(new_pos);
              }
            }
             if (es_fantasma_enemigo(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);

            }

             if (es_libro(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }

             if (no_es_mio(new_pos))
            {
              if (!es_fantasma_enemigo(new_pos))
              {
                parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
                q.push(new_pos);
              }
               if (!es_mago_enemigo(new_pos))
              {
                parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
                q.push(new_pos);
              }
            }

             if (is_empty(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }
             if(es_mio(new_pos)){
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);

            }
          }
        }
      }
    }

    // Reconstruir el camino desde el objetivo hasta la posición inicial
    vector<Pos> path;
    if (target.i != -1 && target.j != -1)
    {
      for (Pos at = target; at.i != -1 && at.j != -1; at = parent[at.i][at.j])
      {
        path.push_back(at);
      }
      reverse(path.begin(), path.end()); // invertir el camino para que vaya desde la posición inicial hasta el objetivo
    }

    return path; // devolver el camino
  }

  Dir movimiento(Pos pos)
  {

    vector<Pos> path = Bfs(pos);
    if (path.size() > 1)
    {
      Pos nextPos = path[1]; // la siguiente posición en el camino
      if (nextPos.i == pos.i - 1 && nextPos.j == pos.j)
        return Up;
      else if (nextPos.i == pos.i + 1 && nextPos.j == pos.j)
        return Down;
      else if (nextPos.i == pos.i && nextPos.j == pos.j - 1)
        return Left;
      else if (nextPos.i == pos.i && nextPos.j == pos.j + 1)
        return Right;
    }
    // Si no se encuentra un objetivo, retornar una dirección aleatoria
    Random_generator rng;
    int random_dir = rng.random(0, 3);
    return direccions[random_dir];
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play()
  {

    // movimiento magos

    // vector de las id de los magos
    vector<int> magos = wizards(me());

    vector<vector<int>> tablero;

    int size = magos.size();
    for (int i = 0; i < size; ++i)
    {

      Pos posmago = unit(magos[i]).pos;    // posicion del mago
                                           // unidad del mago
      move(magos[i], movimiento(posmago)); // muevo el mago
    }

    // movimiento fantasma
    int idfantasma = ghost(me());              // id del fantasma
    Pos posfantasma = unit(idfantasma).pos;    // posicion del fantasma
                                               // unidad del fantasma
    move(idfantasma, movimiento(posfantasma)); // muevo el fantasma
  }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
