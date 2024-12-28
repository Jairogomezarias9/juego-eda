#!/bin/bash
read -p "nom jugador: " player
read -p "nom rival: " rival
read -p "n seeds: " n
# Loop from 1 to n
rm logs/wins.txt

for i in $(seq 1 $n); do
    ./Game MC_v0 ignagir3 $rival $player -s $i < default.cnf > default.res 2>logs/log.txt
    tail -n 6 logs/log.txt | head -n 5 >> logs/wins.txt
done

echo "W $player"
grep -c "$player got top score" logs/wins.txt
echo "W $rival"
grep -c "$rival got top score" logs/wins.txt