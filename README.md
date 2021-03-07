# Sblerky's Game Arcade
## _A little Game and Watch compilation for Nintendo Switch_
## Presentation video : https://youtu.be/MeM5cFiT9bY

Sblerky's Game Arcade is a homebrew game for Nintendo Switch. It offers 2 games for now :

- Ball
- Flagman

## Ball

In this game you have to use the joystick or the D-pad to juggle the two balls.

![alt text](https://i.imgur.com/mVjB2l4.jpg)

## Flagman

For this game you have to use the A,B,X,Y buttons to emulate the flagman's behavior.

![alt text](https://i.imgur.com/n8E1q1B.jpg)

## Leaderboard

The leaderboard is accessible by pressing the minus button on the main menu. Scores data are stored in 
`sd:/switch/Sblerky/scores.json`. Scores are encrypted with a base 64 encoder and Vigenère cipher.

![alt text](https://i.imgur.com/J7xNf9K.jpg)

#### Building sources

If you already have your homebrew development environment setup, you just have to clone the repo and build the project :

```sh
git clone https://github.com/Sblerky/GameArcade.git
cd GameArcade
make
```
## License

GNU General Public License (GPL)


