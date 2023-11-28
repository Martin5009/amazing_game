import PySimpleGUI as sg
import numpy

class Led:

    def __init__(self, state):
        self.state = state

    def __str__(self):
        return bin(self.state)

    def __print__(self):
        return bin(self.state)

    def change_color(self, rg):
        if (rg):
            self.state = self.state | 0b10
        else:
            self.state = self.state & 0b01
    
    def toggle(self, en):
        if (en):
            self.state = self.state | 0b01
        else:
            self.state = self.state & 0b10

class Canvas:

    def __init__(self, rows=16, cols=32):
        self.box_size = 15
        self.rows = rows
        self.cols = cols
        self.grid = numpy.zeros(rows * cols, dtype='i').reshape(rows, cols)

        # Set up a random initial configuration for the grid.
        for row in range(self.rows):
            for col in range(self.cols):
                self.grid[row][col] = 0
        self.init_graphics()
        self.manual_board_setup()

    def init_graphics(self):
        self.graph = sg.Graph((self.rows*self.box_size, self.cols*self.box_size), (0, 0), (self.rows*self.box_size, self.cols*self.box_size),
                              key='-GRAPH-',
                              background_color='white',
                              enable_events = True,
                              drag_submits=True)
        layout = [
            [sg.Button('G', key='-G-'), sg.Button('R', key='-R-')],
            [self.graph],
            [sg.Button('Print', key='-DONE-'), sg.Input(key='-IN-')]
        ]

        self.window = sg.Window('Canvas', layout, finalize=True)
        event, values = self.window.read(timeout=0)

    def manual_board_setup(self):
        ids = []
        drawing = True
        clicked = False
        active = False
        color = 'green'

        for row in range(self.rows):
            ids.append([])
            for col in range(self.cols):
                ids[row].append(0)
        
        while True:  # Event Loop

            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                break
            mouse = values['-GRAPH-']

            # When user presses 'Print', convert drawing to a state array and save to file
            if event == '-DONE-':
                name = values['-IN-'].rstrip()
                grid = self.grid.tolist()
                leds2d = [[Led(0b01) if x == 0b01 else Led(0b11) if x == 0b11 else Led(0b00) for x in row] for row in grid]
                with open('statearray.txt', 'a') as f:
                    f.write('//////////////////////////\n' + name + '\n' + '//////////////////////////\n\n' + make_2d_array_str(leds2d) + '\n\n')
            
            # Check for color swaps
            if event == '-G-':
                color = 'green'
            if event == '-R-':
                color = 'red'

            # Check for mouse click, hold, and drag events
            if event == '-GRAPH-':
                clicked = True
            elif event.endswith('+UP'):         # The drawing has ended because mouse up
                clicked = False
                active = False
            
            # If mouse is held down, determine whether to erase or draw
            if clicked:
                if mouse == (None, None):
                    continue
                box_x = mouse[0] // self.box_size
                box_y = mouse[1] // self.box_size
                
                if self.grid[box_x][box_y] >= 1 and not active:
                    id_val = ids[box_x][box_y]
                    self.graph.delete_figure(id_val)
                    self.grid[box_x][box_y] = 0
                    drawing = False
                    active = True
                elif self.grid[box_x][box_y] == 0 and not active:
                    drawing = True
                    active = True
                elif self.grid[box_x][box_y] >= 1 and active and not drawing:
                    id_val = ids[box_x][box_y]
                    self.graph.delete_figure(id_val)
                    self.grid[box_x][box_y] = 0
                elif self.grid[box_x][box_y] == 0 and drawing and active:
                    id_val = self.graph.draw_rectangle((box_x * self.box_size, box_y * self.box_size),
                                                       (box_x * self.box_size + self.box_size,
                                                        box_y * self.box_size + self.box_size),
                                                       line_color='black', fill_color=color)
                    ids[box_x][box_y] = id_val
                    if color == 'green':
                        self.grid[box_x][box_y] = 0b01
                    if color == 'red':
                        self.grid[box_x][box_y] = 0b11 

        if event == sg.WIN_CLOSED:
            self.window.close()
        else:
            self.window['-DONE-'].update(text='Exit')

def build_matrix(rows, cols):
    return [[Led(0b00) for col in range(cols)] for row in range(rows)]

def make_2d_array_str(leds2d):
    state_array = [[bin(led.state) for led in leds] for leds in leds2d]
    return str(state_array).replace('[','{').replace(']','}').replace("'", "")

def main():
    cv = Canvas()

if __name__ == "__main__":
    main()