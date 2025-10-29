import pygame
import math
import time
import random
import sys

pygame.init()
#This value can be adjusted to configure the game window size; values less than 250 are not recommended
display_size = 1200
square_size = display_size / 8
screen = pygame.display.set_mode((display_size, display_size))
pygame.display.set_caption('Checkerboard')
font = pygame.font.Font('freesansbold.ttf', round(display_size / 15))

light = (232, 179, 107)
dark = (138, 87, 20)
highlight = ((128, 255, 255), (255, 255, 128), (255, 128, 128))
checker_color = ((255, 32, 32), (64, 64, 64))
title_rect = (230, 230, 230)
button = (32, 255, 32)
start_board = [[0, [1, 0], 0, [1, 0], 0, [1, 0], 0, [1, 0]],
         [[1, 0], 0, [1, 0], 0, [1, 0], 0, [1, 0], 0],
         [0, [1, 0], 0, [1, 0], 0, [1, 0], 0, [1, 0]],
         [0, 0, 0, 0, 0, 0, 0, 0],
         [0, 0, 0, 0, 0, 0, 0, 0],
         [[0, 0], 0, [0, 0], 0, [0, 0], 0, [0, 0], 0],
         [0, [0, 0], 0, [0, 0], 0, [0, 0], 0, [0, 0]],
         [[0, 0], 0, [0, 0], 0, [0, 0], 0, [0, 0], 0]]
board = start_board
num_paths = 0
pre = 0

def shade(color):
    return (color[0] / 2, color[1] / 2, color[2] / 2)

def square(xpos, ypos, color):
    xpos, ypos = convert(xpos, ypos)
    pygame.draw.rect(screen, color, pygame.Rect(xpos - square_size / 2, ypos - square_size / 2, square_size, square_size))

def draw_board(color1, color2):
    screen.fill(color1)
    for j in range(8):
        for i in range(8):
            if i % 2 != j % 2:
                square(i, j, color2)

def triangle(xpos, ypos, size, color):
    height = size * math.sqrt(3) / 2
    pygame.draw.polygon(screen, color, ((xpos - size / 2, ypos + height / 3), (xpos, ypos - 2 * height / 3), (xpos + size / 2, ypos + height / 3)))

def checker(xpos, ypos, color, state):
    xpos, ypos = convert(xpos, ypos)
    pygame.draw.circle(screen, checker_color[color], (xpos, ypos), square_size / 2.5)
    pygame.draw.circle(screen, shade(checker_color[color]), (xpos, ypos), square_size / 2.5, round(square_size / 30))
    pygame.draw.circle(screen, shade(checker_color[color]), (xpos, ypos), square_size / 3.5, round(square_size / 50))
    if state: triangle(xpos, ypos, square_size / 4, shade(checker_color[color]))

def convert(boardXpos, boardYpos):
    return square_size * (boardXpos + 0.5), square_size * (boardYpos + 0.5)

def show(board):
    for i in range(8):
        for j in range(8):
            if board[i][j] != 0:
                checker(j, i, board[i][j][0], board[i][j][1])

def valid_move(board, xpos, ypos):
    target = []
    direction = (-1, 1) if board[ypos][xpos][1] else (1 - 2 * board[ypos][xpos][0],)
    player = board[ypos][xpos][0]

    for xdir in (-1, 1):
        for ydir in direction:
            ydir *= -1
            if xpos + xdir in range(8) and ypos + ydir in range(8):
                if board[ypos + ydir][xpos + xdir] == 0:
                    target.append(((xpos + xdir, ypos + ydir),))

    positions = [((xpos, ypos),),]
    while len(positions) > 0:
        for xdir in (-1, 1):
            for ydir in direction:
                ydir *= -1
                if positions[0][0][0] + 2 * xdir in range(8) and positions[0][0][1] + 2 * ydir in range(8) and \
                board[positions[0][0][1] + ydir][positions[0][0][0] + xdir] != 0 and \
                board[positions[0][0][1] + ydir][positions[0][0][0] + xdir][0] != player and \
                board[positions[0][0][1] + 2 * ydir][positions[0][0][0] + 2 * xdir] == 0 and \
                (positions[0][0][0] + xdir, positions[0][0][1] + ydir) not in positions[0][1:]:
                    target.append(((positions[0][0][0] + 2 * xdir, positions[0][0][1] + 2 * ydir), (positions[0][0][0] + xdir, positions[0][0][1] + ydir)) + positions[0][1:])
                    positions.append(((positions[0][0][0] + 2 * xdir, positions[0][0][1] + 2 * ydir), (positions[0][0][0] + xdir, positions[0][0][1] + ydir)) + positions[0][1:])
        positions.pop(0)
    return target

def move(board, xpos, ypos, targetpos):
    board_copy = [row[:] for row in board]
    position = [board_copy[ypos][xpos][0], board_copy[ypos][xpos][1]]

    if targetpos[0][1] == (0, 7)[position[0]]:
        position[1] = 1
    board_copy[targetpos[0][1]][targetpos[0][0]] = position
    board_copy[ypos][xpos] = 0
    for take in targetpos[1:]:
        board_copy[take[1]][take[0]] = 0
    return board_copy

def mouse_pos():
    for event in pygame.event.get():
        if event.type == pygame.MOUSEBUTTONDOWN:
            pos = pygame.mouse.get_pos()
            return (math.floor(pos[0] / square_size), math.floor(pos[1] / square_size))
    return -1

def show_highlight(moves):
    for pos in moves:
        square(pos[0][0], pos[0][1], highlight[1])
        if len(pos) > 1:
            for take in pos[1:]:
                square(take[0], take[1], highlight[2])

def find_pieces(board, player):
    pieces = []
    for i in range(8):
        for j in range(8):
            if board[i][j] != 0 and board[i][j][0] == player:
                pieces.append((j, i))
    return pieces

def value(board, player):
    score = 0
    for i in range(8):
        for j in range(8):
            if board[i][j] != 0 and board[i][j][0] == player:
                score += board[i][j][1] + 2
    return score

def find_all(board, player):
    pieces = find_pieces(board, player)
    total = []
    for piece in pieces:
        targets = valid_move(board, piece[0], piece[1])
        for target in targets:
            total += [((piece[0], piece[1]), target)]
    return total

def rand_alg(board, moves, player):
    return moves[random.randint(0, len(moves)-1)]

def alg(board, moves, player, depth):
    if depth == 0:
        counter()
        return board_score(board, player), []

    best_score = -float('inf') if player == 1 else float('inf')
    best_moves = []

    for position in moves:
        new_board = move(board, position[0][0], position[0][1], position[1])
        opp_moves = find_all(new_board, not player)
        score = alg(new_board, opp_moves, not player, depth - 1)[0]

        if player == 1:
            if score > best_score:
                best_score = score
                best_moves = [position]
            elif score == best_score:
                best_moves.append(position)
        else:
            if score < best_score:
                best_score = score
                best_moves = [position]
            elif score == best_score:
                best_moves.append(position)

    return best_score, best_moves

def board_score(board, player):
    return value(board, player) - value(board, not player)

def counter(cont = 1):
    global num_paths
    if cont == 1:
        num_paths += 1
    elif cont == 0:
        num_paths = 0
    elif cont == -1:
        return num_paths

def title(prev):
    r = round(display_size / 10)
    pygame.draw.rect(screen, title_rect, pygame.Rect(display_size / 5, display_size / 5, 3 * display_size / 5, 3 * display_size / 5), 0, r, r, r, r)
    pygame.draw.rect(screen, shade(title_rect), pygame.Rect(display_size / 5, display_size / 5, 3 * display_size / 5, 3 * display_size / 5), round(display_size / 100), r, r, r, r)
    text = font.render({-1: 'Checkers Game', 0: "You Won!", 1: "Game Over"}[prev], True, (0, 0, 0))
    textRect = text.get_rect()
    textRect.center = (display_size / 2, display_size / 3)
    screen.blit(text, textRect)

    r = round(r / 2)
    pygame.draw.rect(screen, button, pygame.Rect(0.35 * display_size, 0.5 * display_size, 0.3 * display_size, 0.2 * display_size), 0, r, r, r, r)
    pygame.draw.rect(screen, shade(button), pygame.Rect(0.35 * display_size, 0.5 * display_size, 0.3 * display_size, 0.2 * display_size), round(display_size / 167), r, r, r, r)
    text = font.render('Play!', True, (0, 0, 0))
    textRect = text.get_rect()
    textRect.center = (display_size / 2, 0.6 * display_size)
    screen.blit(text, textRect)

    while 1:
        for event in pygame.event.get():
            if event.type == pygame.MOUSEBUTTONDOWN:
                pos = pygame.mouse.get_pos()
                if pos[0] in range(round(0.35 * display_size), round(0.65 * display_size)) and pos[1] in range(round(0.5 * display_size), round(0.7 * display_size)):
                    draw_board(light, dark)
                    show(board)
                    pygame.display.flip()
                    return
        pygame.display.flip()

def delay(sec):
    cur = time.time()
    while time.time() - cur < sec: pygame.event.get()
    return

def game():
    board = start_board
    draw_board(light, dark)
    show(board)
    pygame.display.flip()
    while 1:
        if len(find_all(board, 0)) == 0:
                print("Computer Wins!")
                return 1

        while 1:
            position = mouse_pos()
            if position != -1: break

        xpos, ypos = position
        if board[ypos][xpos] != 0 and board[ypos][xpos][0] == 0:
            draw_board(light, dark)
            square(xpos, ypos, highlight[0])
            moves = valid_move(board, xpos, ypos)
            show_highlight(moves)
            show(board)
            pygame.display.flip()

            while 1:
                position = mouse_pos()
                if position != -1: break

            for pos in moves:
                if position == pos[0]:
                    board = move(board, xpos, ypos, pos)
                    draw_board(light, dark)
                    show(board)
                    pygame.display.flip()
                    all_moves = find_all(board, 1)
                    if len(all_moves) == 0:
                        print("Human Wins!")
                        return 0
                    pre = time.time()
                    depth = 2
                    depth *= 2
                    counter(0)
                    pre_score, optimal_moves = alg(board, all_moves, 1, depth)
                    print(f"{counter(-1)} paths analysed (depth {int(depth/2)})")
                    print(f"{len(optimal_moves)} optimal positions found of {len(all_moves)} possible")
                    print(f"predicted score: {pre_score}")
                    choice = optimal_moves[random.randint(0, len(optimal_moves)-1)]
                    board = move(board, choice[0][0], choice[0][1], choice[1])
                    draw_board(light, dark)
                    square(choice[1][0][0], choice[1][0][1], highlight[0])
                    show(board)
                    print(f'{round(time.time() - pre, 2)} seconds')
                    print("--------------------------")
                    while time.time() - pre < 0.5: pass
                    pygame.display.flip()
                    break
            else:
                draw_board(light, dark)
                show(board)
                pygame.display.flip()

draw_board(light, dark)
show(start_board)
title(-1)
while 1: title(game())
