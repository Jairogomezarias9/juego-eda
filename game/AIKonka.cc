#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Konka

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory() {
    return new PLAYER_NAME;
  }

  /**
   * Direcciones de movimiento: los magos no se mueven en diagonal, pero los fantasmas sí.
   */
  const vector<Dir> mago_moves = {Down, Right, Up, Left};
  const vector<Dir> ghost_moves = {Down, DR, Right, RU, Up, UL, Left, LD};

  /**
   * Funciones auxiliares para evaluar condiciones
   */
  bool is_enemy(Pos p, int my_strength) {
    int id = cell(p).id;
    if(id == -1) return false;
    Unit u = unit(id);
    //NO SOY YO, ES MAGO, NO ESTA CONVERTIDO, Y NO ME MATARA AL 100% ATACO
    return u.player != me() and u.type == Wizard and !u.is_in_conversion_process() and 2*my_strength >= magic_strength(u.player);
  }

  bool is_my_wizard_converting(Pos p) {
    int id = cell(p).id;
    if(id == -1) return false;
    Unit u = unit(id);
    return u.player == me() && u.type == Wizard && u.is_in_conversion_process();
  }

  bool is_enemy_ghost(Pos p) {
    int id = cell(p).id;
    if(id == -1) return false;
    Unit u = unit(id);
    //NO SOY YO, ES MAGO, NO ESTA CONVERTIDO, Y NO ME MATARA AL 100% ATACO
    return u.player != me() and u.type == Ghost;
  }

  bool is_book(Pos p) {
    return pos_ok(p) && cell(p).book;
  }

  bool is_empty(Pos p) {
    return cell(p).id == -1 and cell(p).owner == -1;
  }
  bool is_conquered(Pos p) {
    return cell(p).id == -1 and cell(p).owner != me();
  }
  bool is_mine(Pos p) {
    return cell(p).id == -1 and cell(p).owner == me();
  }

  //FUNCIONES PARA VOLDEMORT

  bool is_safe(Pos p) {
    Pos voldemort_pos = pos_voldemort();  // Obtener la posición de Voldemort

    // Uso movimiento ghost ya que su campo de actuacion es identico a Voldemort
    for (int i = 0; i < int((ghost_moves.size())); i++) {
        Pos adj = voldemort_pos + ghost_moves[i];
        if (pos_ok(adj) and adj == p) return false;  // Si p es una casilla adyacente, no es segura
    }
    // Si no está adyacente a Voldemort, es segura
    return true;
  }

  Dir avoid_voldemort(Pos current) {
    Pos voldemort_pos = pos_voldemort();  // Posición actual de Voldemort
    Dir best_dir = Left;                 // Dirección inicial predeterminada
    int max_distance = -1;               // Máxima distancia encontrada

    // Evaluar cada dirección posible
    for (int i = 0; i < int(mago_moves.size()); i++) {
        Dir direccion_actual = mago_moves[i];
        Pos next = current + mago_moves[i];  // Calcular la posición adyacente

        // Verificar si la posición es válida y está en un pasadizo
        if (pos_ok(next) && cell(next).type == Corridor) {
            // Calcular la distancia Manhattan entre la posición adyacente y Voldemort
            int distance_to_voldemort = abs(next.i - voldemort_pos.i) + abs(next.j - voldemort_pos.j);

            // Actualizar si encontramos una posición más lejana
            if (distance_to_voldemort > max_distance) {
                max_distance = distance_to_voldemort;
                best_dir = direccion_actual;
            }
        }
    }

    return best_dir;  // Retorna la mejor dirección encontrada
  }

  //Seguro que hay un camino porque miro que no este fuera del tablero
  //Y que no sea pared pasadizo, siempre habra algun sitio donde ir.
  Dir move_random(const Pos& current, const vector<Dir>& dirs) {
    // Intentar hasta encontrar una dirección válida
    for (int i = 0; i < int(dirs.size()); i++) {
        // Elegir una dirección aleatoria
        Dir random_dir = dirs[random(0, dirs.size() - 1)];
        Pos next = current + random_dir;

        // Verificar si la posición es válida
        if (pos_ok(next) && cell(next).type == Corridor) {
            return random_dir;  // Retorna la dirección válida
        }
    }
    //Por defecto si no encontramos nada 
    //Siguiendo la logica de escape de laberinto minotauro
    return Left;
  }

  /**
   * BFS para los magos
   * Este BFS busca según las prioridades definidas para los magos.
   */
  pair<Dir, int> bfs_mago(Pos start, int my_strength, int priority, int rango) {
    const int max_depth = rango;  // Límite máximo de recorrido
    vector<vector<bool>> visited(60, vector<bool>(60, false));
    queue<pair<Pos, int>> q;
    q.push({start, 0});
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front().first;
        int dist = q.front().second;
        q.pop();

        for (int i = 0; i < int(mago_moves.size()); i++) {
            Dir d = mago_moves[i];
            Pos next = current + d;

            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;

                // Verificar el objetivo según la prioridad
                switch (priority) {
                    case 1:  // Prioridad: Buscar enemigos debiles
                        if (is_enemy(next, my_strength)) return {d, dist + 1};
                        break;

                    case 2:  // Prioridad: Salvar magos
                        if (is_my_wizard_converting(next)) return {d, dist + 1};
                        break;

                    case 3:  // Prioridad: Buscar libros
                        if (is_book(next)) return {d, dist + 1};
                        break;

                    case 4:  // Prioridad: Buscar libros
                        if (is_enemy_ghost(next)) return {d, dist + 1};
                        break;

                    case 5:  // Prioridad: Buscar casillas vacías
                        if (is_empty(next) or is_conquered(next)) return {d, dist + 1};
                        break;

                    default:  // Caso por defecto: No hacer nada
                        break;
                }

                // Agregar la posición a la cola si no hemos alcanzado el límite
                if (dist + 1 < max_depth) {
                    q.push({next, dist + 1});
                }
            }
        }
    }

    // Movimiento aleatorio si no se encontró un objetivo con la prioridad especificada
    Dir movimiento_desesperado = move_random(start, mago_moves);
    return {movimiento_desesperado, 0};
  }

  /**
   * BFS para los fantasmas
   * Este BFS busca según las prioridades definidas para los fantasmas.
   */
  pair<Dir, int> bfs_fantasma(Pos start, int priority, int rango) {
    const int max_depth = rango;  // Límite máximo de recorrido
    vector<vector<bool>> visited(60, vector<bool>(60, false));
    queue<pair<Pos, int>> q;
    q.push({start, 0});
    visited[start.i][start.j] = true;

    while (!q.empty()) {
        Pos current = q.front().first;
        int dist = q.front().second;
        q.pop();

        for (int i = 0; i < int(ghost_moves.size()); i++) {
            Dir d = ghost_moves[i];
            Pos next = current + d;

            if (pos_ok(next) && !visited[next.i][next.j] && cell(next).type == Corridor) {
                visited[next.i][next.j] = true;

                // Verificar el objetivo según la prioridad
                switch (priority) {
                    case 1:  // Prioridad: Buscar libros
                        if (is_book(next)) return {d, dist + 1};
                        break;

                    case 2:  // Prioridad: Buscar casillas vacías
                        if (is_empty(next) or is_conquered(next)) return {d, dist + 1};
                        break;

                    default:  // Caso por defecto: No hacer nada
                        break;
                }

                // Agregar la posición a la cola si no hemos alcanzado el límite
                if (dist + 1 < max_depth) {
                    q.push({next, dist + 1});
                }
            }
        }
    }

    // Movimiento aleatorio si no se encontró un objetivo con la prioridad especificada
    Dir movimiento_desesperado = move_random(start, ghost_moves);
    return {movimiento_desesperado, 0};
  }

  /**
   * Mover magos
   */
  void move_wizards() {
    vector<int> mago_ids = wizards(me());  // Obtener los IDs de los magos del jugador

    for (int i = 0; i < int(mago_ids.size()); i++) {
        int id = mago_ids[i];  // ID del mago actual
        Unit wizard = unit(id);  // Obtener la información del mago
        int my_strength = magic_strength(me());  // Fuerza mágica del jugador

        // 1. Prioridad: Buscar enemigos débiles (prioridad 1)
        pair<Dir, int> bfs_enemigos = bfs_mago(wizard.pos, my_strength, 1, 20);
        if (bfs_enemigos.second > 0) {
            move(id, bfs_enemigos.first);
            continue;  // Si encontró un enemigo, no evalúa otras prioridades
        }

        // 2. Prioridad: Wizard converting (prioridad 3)
        pair<Dir, int> bfs_salvar = bfs_mago(wizard.pos, my_strength, 2, 4);
        if (bfs_salvar.second > 0) {
            move(id, bfs_salvar.first);
            continue;  // Si encontró un libro, no evalúa otras prioridades
        }

        // 3. Prioridad: Buscar libros (prioridad 3)
        pair<Dir, int> bfs_libros = bfs_mago(wizard.pos, my_strength, 3, 10);
        if (bfs_libros.second > 0) {
            move(id, bfs_libros.first);
            continue;  // Si encontró un libro, no evalúa otras prioridades
        }

        // 3. Prioridad: Buscar fantasmas enemigos (prioridad 4)
        pair<Dir, int> bfs_fantasmas = bfs_mago(wizard.pos, my_strength, 4, 10);
        if (bfs_fantasmas.second > 0) {
            move(id, bfs_fantasmas.first);
            continue;  // Si encontró un fantasma enemigo, no evalúa otras prioridades
        }

        // 5. Prioridad: Buscar casillas vacías o conquistadas por enemigos (prioridad 6)
        pair<Dir, int> bfs_casillas = bfs_mago(wizard.pos, my_strength, 6, 10);
        if (bfs_casillas.second > 0) {
            move(id, bfs_casillas.first);
            continue;  // Si encontró una casilla vacía o conquistada, no evalúa otras prioridades
        }

        // 6. Movimiento aleatorio si no se encontró ningún objetivo
        Dir movimiento_desesperado = move_random(wizard.pos, mago_moves);
        move(id, movimiento_desesperado);
    }
}

  /**
   * Mover fantasmas
   */
  void move_ghost() {
    int id = ghost(me());
    Unit ghost = unit(id);

    // Busca el mejor movimiento con BFS
    pair<Dir, int> bfs_libro = bfs_fantasma(ghost.pos, 1, 20);
    if (bfs_libro.second > 0) {
        move(id, bfs_libro.first);
    }
    pair<Dir, int> bfs_celda = bfs_fantasma(ghost.pos, 2, 10);
    if (bfs_celda.second > 0) {
        move(id, bfs_celda.first);
    }
    //TRY TO SOLVE SPELL if no tiene que esperar
    if(ghost.resting_rounds() == 0) spell(id,{0,2,4,1,3,2,0,1,4,1,4,0,3,3,2});
  }

  /**
   * Método principal que se llama en cada ronda
   */
  virtual void play() {
    move_ghost();
    move_wizards();
  }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);