
Algoritmo Aproximativo para TSP
-------------------------------

Para executar basta rodar em um terminal

```bash
make && ./tsp data/tsp1_253.txt -b
# ou então
gcc -o tsp tsp.c && ./tsp data/tsp1_253.txt -b
```

A opção `-b` é uma flag de executar ou não o algoritmo de força bruta,
visto que pode demorar anos. 

_default: não executar_
