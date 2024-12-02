#include "Player.hh"
#include <functional>

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

  vector<Dir> direccions = {Up, Down, Left, Right};
  vector<Dir> direccionsfantasma = {Up, Down, Left, Right, UL, RU, LD, DR};
  bool encontrado = false;

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
    return (u.player != me() and u.type == Wizard && !u.is_in_conversion_process());
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

 bool esta_en_campo_letal(Pos p)
{
    Pos pos_v = pos_voldemort(); // Obtiene la posición del personaje especial

    int di = abs(p.i - pos_v.i);
    int dj = abs(p.j - pos_v.j);

    int distancia_chebyshev = std::max(di, dj);

    // Si la distancia de Chebyshev es exactamente 2, la posición está en el campo letal
    return distancia_chebyshev == 2;
}

  pair<Pos, int> Bfs(Pos pos, function<bool(Pos)> is_target)
  {
    vector<vector<bool>> visited(board_rows(), vector<bool>(board_cols(), false)); // matriz de casillas visitadas
    vector<vector<Pos>> parent(board_rows(), vector<Pos>(board_cols(), {-1, -1})); // matriz para rastrear el camino
                                                                                   // cola para BFS
    queue<pair<Pos, int>> q;                                                       // Cola para BFS, almacenando la posición y la distanciaancia
    q.push({pos, 0});                                                              // empiezo en la posición inicial
    visited[pos.i][pos.j] = true;

    while (!q.empty())
    {
      auto [actual, distancia] = q.front();
      q.pop();
      if (is_target(actual))
      {
        // Reconstruir el camino hacia el objetivo
        Pos next_pos = actual;
        while (parent[next_pos.i][next_pos.j] != pos && parent[next_pos.i][next_pos.j].i != -1)
        {
          next_pos = parent[next_pos.i][next_pos.j];
        }
        return {next_pos, distancia};
      }
      // Explorar las cuatro direcciones
      for (int i = 0; i < 4; ++i)
      {
        Pos new_pos = actual + direccions[i];

        if (pos_ok(new_pos.i, new_pos.j) && !visited[new_pos.i][new_pos.j])
        {
          visited[new_pos.i][new_pos.j] = true;
          if (cell(new_pos).type != Wall && !esta_en_campo_letal(new_pos))
          {
            if(!esta_en_campo_letal(new_pos)){
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }
           else if (es_libro(new_pos))
            {
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }

            else if (es_mago_enemigo(new_pos))
            {

              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }
            else if (es_fantasma_enemigo(new_pos))
            {
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }

            else if (no_es_mio(new_pos))
            {
              if (!es_fantasma_enemigo(new_pos))
              {
                visited[new_pos.i][new_pos.j] = true;
                parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
                q.push({new_pos, distancia + 1});
              }
              if (!es_mago_enemigo(new_pos))
              {
                visited[new_pos.i][new_pos.j] = true;
                parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
                q.push({new_pos, distancia + 1});
              }
            }

            else if (is_empty(new_pos))
            {
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }
            else if (es_mio(new_pos))
            {
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              q.push({new_pos, distancia + 1});
            }
          }
        }
      }
    }
    // Si no se encontró un objetivo, retornar una posición inválida y prioridad -1
    return {{-1, -1}, -1};
  }

  vector<Pos> Bfsfantasma(Pos pos)
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
      if (es_libro(actual))
      {
        target = actual;
        break;
      }

      // Explorar las cuatro direcciones
      for (int i = 0; i < 8; ++i)
      {
        Pos new_pos = actual + direccionsfantasma[i];

        if (pos_ok(new_pos.i, new_pos.j) && !visited[new_pos.i][new_pos.j])
        {
          visited[new_pos.i][new_pos.j] = true;

          if (cell(new_pos).type != Wall && !esta_en_campo_letal(new_pos))
          {
            if(!esta_en_campo_letal(new_pos)){
              visited[new_pos.i][new_pos.j] = true;
              parent[new_pos.i][new_pos.j] = actual; // Guardar el padre para reconstruir el camino
              
            }
            if (es_libro(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }

            else if (!es_mago_enemigo(new_pos))
            {

              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }
            else if (!es_fantasma_enemigo(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }

            else if (no_es_mio(new_pos))
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

            else if (is_empty(new_pos))
            {
              parent[new_pos.i][new_pos.j] = actual; // guardar el padre para reconstruir el camino
              q.push(new_pos);
            }
            else if (es_mio(new_pos))
            {
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
    vector<Pos> path;

    if (cell(pos).id != -1 && (unit(cell(pos).id).type) == Wizard)
    {
      auto [next_pos_book, dist_book] = Bfs(pos, std::function<bool(Pos)>([this](Pos p)
                                                                          { return es_libro(p); }));
      auto [next_pos_wizard, dist_wizard] = Bfs(pos, std::function<bool(Pos)>([this](Pos p)
                                                                              { return es_mago_enemigo(p); }));
      auto [next_pos_ghost, dist_ghost] = Bfs(pos, std::function<bool(Pos)>([this](Pos p)
                                                                            { return es_fantasma_enemigo(p); }));

      // Comparar distancias y elegir el objetivo más cercano
      int min_distance = INT_MAX;
      Pos next_pos = pos;

      if (dist_book != -1 && dist_book < min_distance)
      {
        min_distance = dist_book;
        next_pos = next_pos_book;
      }
      if (dist_wizard != -1 && dist_wizard < min_distance)
      {
        min_distance = dist_wizard;
        next_pos = next_pos_wizard;
      }
      if (dist_ghost != -1 && dist_ghost < min_distance)
      {
        min_distance = dist_ghost;
        next_pos = next_pos_ghost;
      }

      // Si se encontró un objetivo
      if (min_distance != INT_MAX && !esta_en_campo_letal(next_pos))
      {
        if (next_pos.i == pos.i - 1 && next_pos.j == pos.j)
          return Up;
        else if (next_pos.i == pos.i + 1 && next_pos.j == pos.j)
          return Down;
        else if (next_pos.i == pos.i && next_pos.j == pos.j - 1)
          return Left;
        else if (next_pos.i == pos.i && next_pos.j == pos.j + 1)
          return Right;
      }
      else if(esta_en_campo_letal(next_pos)){
        // Si no se ha encontrado una dirección, retornar una dirección aleatoria que sea segura
       for (int i = 0; i < 4; ++i)
       {
        Pos adj_pos =pos+direccions[i];
        if (pos_ok(adj_pos) && !esta_en_campo_letal(adj_pos) && cell(adj_pos).type != Wall)
            return direccions[i];
         }

      }
    }

    else if (cell(pos).id != -1 && (unit(cell(pos).id).type) == Ghost)
    {
      vector<Pos> path = Bfsfantasma(pos);
      if (path.size() > 0 &&  !esta_en_campo_letal(path[1]))
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
        else if (nextPos.i == pos.i - 1 && nextPos.j == pos.j - 1)
          return UL;
        else if (nextPos.i == pos.i - 1 && nextPos.j == pos.j + 1)
          return RU;
        else if (nextPos.i == pos.i + 1 && nextPos.j == pos.j - 1)
          return LD;
        else if (nextPos.i == pos.i + 1 && nextPos.j == pos.j + 1)
          return DR;
      }
      else if(path.size()>0 and esta_en_campo_letal(path[1])){
        // Si no se ha encontrado una dirección, retornar una dirección aleatoria que sea segura
       for (int i = 0; i < 8; ++i)
       {
        Pos adj_pos =pos+direccionsfantasma[i];
        if (pos_ok(adj_pos) && !esta_en_campo_letal(adj_pos) && cell(adj_pos).type != Wall)
            return direccionsfantasma[i];
         }

      }
      else{
        int random_dir = random(0, 7);
        return direccionsfantasma[random_dir];
      }
    }
    int random_dir = random(0, 3);
    return direccions[random_dir];

  }
  // Función recursiva para resolver el problema con backtracking optimizado
bool backtrack(vector<int>& assignments, vector<int>& groupSums, const vector<int>& ingredients, int index, int targetSum) {
    if (index == ingredients.size()) {
        // Verificar si todos los grupos tienen la suma correcta
        for (int sum : groupSums) {
            if (sum != targetSum) {
                return false;
            }
        }
        return true;
    }

    // Intentar añadir el elemento actual a cada grupo
    for (int i = 0; i < groupSums.size(); ++i) {
        if (groupSums[i] + ingredients[index] <= targetSum) {
            // Asignar al grupo `i`
            groupSums[i] += ingredients[index];
            assignments[index] = i;

            if (backtrack(assignments, groupSums, ingredients, index + 1, targetSum)) {
                return true;
            }

            // Backtracking: deshacer la asignación
            groupSums[i] -= ingredients[index];
            assignments[index] = -1;
        }

        // Optimización: si el grupo está vacío, no probar con grupos futuros
        if (groupSums[i] == 0) {
            break;
        }
    }

    return false;
}

// Función principal para agrupar los ingredientes
vector<int> groupIngredients(const vector<int>& ingredients) {
    

    int totalSum = accumulate(ingredients.begin(), ingredients.end(), 0);

    // Cada grupo debe sumar lo mismo, y cada grupo tiene exactamente 3 elementos
    int numGroups = 5; // 5 grupos
    int targetSum = totalSum / numGroups;

   

    // Inicializar estructuras de datos
    vector<int> assignments(ingredients.size(), -1); // Asignación de grupos
    vector<int> groupSums(numGroups, 0);            // Sumas actuales de los grupos

    // Resolver con backtracking
     backtrack(assignments, groupSums, ingredients, 0, targetSum);
      return assignments;
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
      move(magos[i], movimiento(posmago)); // muevo el mago
      encontrado = false;
    }

    // movimiento fantasma
    int idfantasma = ghost(me());              // id del fantasma
    Pos posfantasma = unit(idfantasma).pos;    // posicion del fantasma
    spell(idfantasma,groupIngredients(spell_ingredients()));
    move(idfantasma, movimiento(posfantasma)); // muevo el fantasma
   
  }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);