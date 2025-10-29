# Checkers AI
#### Video Demo:  [Checkers AI](https://www.youtube.com/watch?v=JkKV1pq_zlE)
#### Overview
This project is a two-player checkers game implemented in Python using the pygame library. It allows a human player to play against a computer-controlled opponent. The program simulates the rules of checkers (with non-forced jumps), draws an interactive board, highlights valid moves, and uses an algorithm to generate computer moves.

My goal with this project was to deepen my understanding of game design and algorithms while applying knowledge of Python and pygame graphics. It also allowed me to experiment with AI decision-making through a minimax-inspired recursive search with limited depth.

#### How to Play: Instructions for Input/Output
Upon running this program, you should see a pygame window open and display a checkerboard with a
title screen overlay. To begin, click the "Play!" button, and the title screen will disappear.
This game behaves like a normal game of checkers where jumps are not forced. The red pieces are
your pieces and the black pieces are the opponent. King pieces are marked by a small triangle at
their center. To select one of your pieces, click on it and it should become highlighted blue.
To deselect the piece, click elsewhere. To make a move, click one of the squares highlighted with
yellow after selecting one of your pieces. Squares highlighted with red indicate an opponent piece
that will be removed upon making that move. Moves involving taking multiple pieces at a time are
also possible. After clicking on a valid destination square, your move will be made. After about
0.5-1 seconds, the program will respond with the opponent move, which will be highlighted with blue
for clarity. Note that processing time may vary on different devices. After the opponent has
responded, you may make your next move using the previously mentioned procedure. You and the
opponent can continue playing until either side cannot make any more moves. When the game ends a
“Game Over” or “You Won” overlay will be shown according to the outcome of the game. At this point
you may click the “Play!” button to play another game of checkers.

#### Features
- Graphical display of an 8×8 checkerboard.
- Human vs. computer gameplay.
- Highlighted valid moves and captures.
- King promotion when reaching the opposite side.
- Computer AI using recursive evaluation (up to depth 4 moves).
- Replay option after a game ends.

#### Project Structure and Code Explanation

- Main game loop: Handles turns, checks for valid moves, and updates the display after every action.
- Board rendering functions (draw_board, checker, triangle, square): Handle the visuals of the board, checkers, and highlights.
- Move generation (valid_move, find_all): Determine all valid moves for a given piece or player.
- AI algorithm (alg, board_score): Implements a recursive minimax-like approach to evaluate moves by simulating future board states.
- Game state management (move, value): Handles applying moves, removing captured pieces, and promoting kings.
- Title screen and overlays (title): Display the start and end-game screens.

The AI evaluates moves by exploring possible futures (to a set depth) and choosing the best-scoring path based on piece count and king status.

#### Design Choices

I decided to use pygame because it provides an easy way to draw graphics and get mouse input. For the AI, I used a recursive search that makes the computer opponent challenging but beatable. The choice to not enforce mandatory jumps makes the game feel more approachable for casual players.
