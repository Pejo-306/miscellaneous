import turtle
import random


class Cell(object):
    STEP = 20

    LEFT = 0
    DOWN = 1
    RIGHT = 2
    UP = 3

    def __init__(self, row, col):
        self.row = row
        self.col = col
        self.walls = [True, True, True, True]
        self.visited = False

    @staticmethod
    def oposite_direction(direction):
        return (direction + 2) % 4

    def has_wall(self, direction):
        assert self.LEFT <= direction <= self.UP
        return self.walls[direction]

    def drill_wall(self, direction):
        assert self.LEFT <= direction <= self.UP
        self.walls[direction] = False

    def build_wall(self, direction):
        assert self.LEFT <= direction <= self.UP
        self.walls[direction] = True

    def draw(self):
        turtle.goto(self.col * self.STEP, self.row * self.STEP)
        turtle.setheading(270)
        for direction in range(4):
            if self.has_wall(direction):
                # draw wall
                turtle.pendown()
            else:
                # no wall
                turtle.penup()
            turtle.forward(self.STEP)
            turtle.left(90)
        turtle.penup()

    def set_visited(self, visited=True):
        self.visited = visited

    def __str__(self):
        return "Cell [{row}, {col}]".format(row=self.row, col=self.col)


class Board(object):
    def __init__(self, rows, cols):
        self.rows = rows
        self.cols = cols

        self.cells = [
            Cell(i // self.cols, i % self.cols)
            for i in range(self.rows * self.cols)
        ]

    def __getitem__(self, index):
        n, m = index
        assert 0 <= n < self.rows
        assert 0 <= m < self.cols
        return self.cells[n * self.cols + m]

    def __setitem__(self, index, value):
        n, m = index
        assert isinstance(value, Cell)
        self[n, m].walls = [wall for wall in value.walls]
        self[n, m].visited = value.visited

    def draw(self):
        for row in range(self.rows):
            for col in range(self.cols):
                cell = self[row, col]
                cell.draw()

    def get_neighbour(self, cell, direction):
        assert Cell.LEFT <= direction <= Cell.UP
        if direction == Cell.LEFT:
            row = cell.row
            col = cell.col - 1
        elif direction == Cell.RIGHT:
            row = cell.row
            col = cell.col + 1
        elif direction == Cell.UP:
            row = cell.row + 1
            col = cell.col
        elif direction == Cell.DOWN:
            row = cell.row - 1
            col = cell.col
        if row < 0 or row >= self.rows or \
                col < 0 or col >= self.cols:
            return None
        return self[row, col]

    def drill_wall(self, cell, direction):
        cell.drill_wall(direction)
        neighbour = self.get_neighbour(cell, direction)
        if neighbour is not None:
            oposite = Cell.oposite_direction(direction)
            neighbour.drill_wall(oposite)
        return neighbour

    def get_unvisited_neighbours(self, cell):
        neighbours = []
        for direction in range(4):
            neighbour = self.get_neighbour(cell, direction)
            if neighbour is not None and not neighbour.visited:
                neighbours.append(direction)
        return neighbours

    def generate(self, cell):
        cell.set_visited()

        while True:
            neighbours = self.get_unvisited_neighbours(cell)
            if not neighbours:
                return
            direction = random.choice(neighbours)
            next = self.drill_wall(cell, direction)
            self.generate(next)

    def copy(self):
        board_cp = Board(self.rows, self.cols)
        for i in range(board_cp.rows):
            for j in range(board_cp.cols):
                board_cp[i, j] = self[i, j]
                board_cp[i, j].visited = False

        return board_cp


def solve(board, cell, path=[], all_paths=[]):
    cell.visited = True

    neighbours = board.get_unvisited_neighbours(cell)
    if not neighbours:
        if cell.row in (0, board.rows - 1) or cell.col in (0, board.cols - 1):
            all_paths.append(path)

        cond = (c.visited for c in board.cells)
        if all(cond):
            longest = []
            for pth in all_paths:
                if len(pth) > len(longest):
                    longest = pth
            return longest
        return None
    else:
        for d in neighbours:
            if not cell.has_wall(d):
                new_path = path.copy()
                new_path.append(d)
                pth = solve(board, board.get_neighbour(cell, d), new_path, all_paths)
                if pth is not None:
                    return pth


def draw_solution(start, path):
    # starting position of the turtle
    turtle.pencolor('red')
    turtle.goto(start.col*start.STEP + start.STEP//2, start.row*start.STEP - start.STEP//2)
    turtle.pendown()

    for direction in path:
        turtle.setheading(90 * Cell.oposite_direction(direction))
        turtle.forward(start.STEP)

    turtle.penup()


def main():
    turtle.speed(0)
    turtle.tracer(False)
    #turtle.hideturtle()

    board = Board(20, 24)
    c = board[0, 0]
    board.generate(c)

    board_cp = board.copy()
    path = solve(board_cp, board_cp[c.row, c.col])
    board.draw()
    draw_solution(board[c.row, c.col], path)

    input("Please enter something: ")


if __name__ == "__main__":
    main()
