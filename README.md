# Game Master for four wins in 3 dimensions

Game Master in C++ for Bots who play 4wins in 3 Dimensions.

This repo contains the game master and a random_player for debugging and testing purposes.


## Compiling 

```
 $ mkdir build && cd build
 $ cmake ..
 $ make
```


## Using 

```
  $ echo "" > black && ./random_player 0 ./black
  $ echo "" > white && ./random_player 0 ./white
  $ ./game_master
```

The two bots need to listen on the corresponding files than start the game master to start the game.

## Communicating with the Game Master

**Special Words**: 
    - `start` you have the first move
    - `end` the game is over you may terminate

**Prefixes**
    - `<` Message from the Game Master
    - `>` Message from the Bot

**Game Master Response**
    - `<n` where n is a column from 0 to 15

Please note that this is always the first charachter in the message and the content will follow without an space.
So an exemplary message might look like this `>4` this tells you that the other player put a stone on column 4. 
Furthermore it is good practise to flush the file after you read it. 

You can find a referance implementation in `src/random_player.cpp`.
