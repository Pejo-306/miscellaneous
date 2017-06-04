import turtle
import random


class Cell:
    STEP = 20

    LEFT = 0
    DOWN = 1
    RIGHT = 2
    UP = 3

    def __init__(self, row, col):
        self.row = row
        self.col = col
        self.walls = [True, True, True, True]

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
        turtle.goto(
            -turtle.window_width() // 2 + self.col * self.STEP,
            turtle.window_height() // 2 - self.row * self.STEP
        )

        turtle.setheading(0)  # east - up
        if self.walls[self.UP]:
            turtle.pendown()
        turtle.forward(self.STEP)
        turtle.penup()

        turtle.setheading(270)  # south - right
        if self.walls[self.RIGHT]:
            turtle.pendown()
        turtle.forward(self.STEP)
        turtle.penup()

        turtle.setheading(180)  # west - down
        if self.walls[self.DOWN]:
            turtle.pendown()
        turtle.forward(self.STEP)
        turtle.penup()

        turtle.setheading(90)  # north - left
        if self.walls[self.LEFT]:
            turtle.pendown()
        turtle.forward(self.STEP)
        turtle.penup()


class Board:
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

    def drill(self, direction, row, col):
        cell = self[row, col]
        cell.drill_wall(direction)

        if col > 0 and direction == Cell.LEFT:
            other_cell = self[row, col - 1]
            other_cell.drill_wall(Cell.RIGHT)
        elif row < self.rows-1 and direction == Cell.DOWN:
            other_cell = self[row + 1, col]
            other_cell.drill_wall(Cell.UP)
        elif col < self.cols-1 and direction == Cell.RIGHT:
            other_cell = self[row, col + 1]
            other_cell.drill_wall(Cell.LEFT)
        elif row > 0 and direction == Cell.UP:
            other_cell = self[row - 1, col]
            other_cell.drill_wall(Cell.DOWN)

    def draw(self):
        turtle.penup()
        for i in range(0, self.rows):
            for j in range(0, self.cols):
                self[i, j].draw()
        turtle.penup()


if __name__ == "__main__":
    turtle.speed(0)
    b = Board(10, 10)
    for i in range(0, 30):
        b.drill(random.randint(0, 3), random.randint(0, 9), random.randint(0, 9))
    b.draw()

    input("Press any key to exit...\n")
