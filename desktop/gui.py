from PyQt5.QtCore import Qt, QMargins
from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QHBoxLayout, QVBoxLayout,
    QPushButton, QComboBox, QDial, QLabel, QLineEdit,
    QApplication
)

import logic, serial

class Vernier(QWidget):
    name = None
    value = 0

    font = None

    def __init__(self, name: str, font: QFont):
        super().__init__()
        self.name = name
        self.font = font
        self.setFixedSize(100, 200)

        self.dial = QDial()
        self.dial.setRange(0, 255)
        self.dial.setValue(self.value)
        self.dial.valueChanged.connect(self.update)

        self.name_label = QLabel(self.name)
        self.name_label.setFont(self.font)
        self.name_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.value_label = QLabel(self.generate_value_text())
        self.value_label.setFont(self.font)
        self.value_label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.layout = QVBoxLayout()
        self.layout.addWidget(self.name_label)
        self.layout.addWidget(self.dial)
        self.layout.addWidget(self.value_label)
        self.layout.setContentsMargins(QMargins(2, 2, 2, 2))
        self.setLayout(self.layout)
    
    def update(self, value: int):
        self.value = value
        self.value_label.setText(self.generate_value_text())
    
    def generate_value_text(self) -> str:
        return f'{self.value}'

class CommandPanel(QWidget):
    font = None
    commands = []
    buttons = []

    def __init__(self, commands: list[str], handlers: list, font: QFont):
        super().__init__()
        self.commands = zip(commands, handlers)
        self.font = font
        self.setFixedSize(100, 100)

        self.layout = QVBoxLayout()

        for command, handler in self.commands:
            button = QPushButton(command)
            button.setFont(self.font)
            button.clicked.connect(handler)
            self.layout.addWidget(button)
            self.buttons.append(button)

        self.setLayout(self.layout)

class SerialPortSelection(QWidget):
    font = None
    ports = []
    port = None

    def __init__(self, font: QFont):
        super().__init__()
        self.font = font
        self.layout_left = QVBoxLayout()
        self.layout_right = QVBoxLayout()
        self.layout = QHBoxLayout()

        self.cont_left = QWidget()
        self.cont_right = QWidget()
        self.cont_left.setLayout(self.layout_left)
        self.cont_right.setLayout(self.layout_right)

        self.port_selection = QComboBox()
        self.port_selection.setFont(self.font)
        self.port_selection.insertItems(0, self.ports)

        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.connect_port)
        self.update_button = QPushButton("Update")
        self.update_button.clicked.connect(self.update_ports)

        self.baud_rate_line = QLineEdit()
        self.baud_rate_line.setText("115200")
        self.baud_rate_line.setFont(self.font)

        self.layout_left.addWidget(self.port_selection)
        self.layout_left.addWidget(self.baud_rate_line)
        self.layout_right.addWidget(self.connect_button)
        self.layout_right.addWidget(self.update_button)
        self.layout.addWidget(self.cont_left)
        self.layout.addWidget(self.cont_right)
        self.setLayout(self.layout)

        self.update_ports()
        if len(self.ports) > 0:
            self.connect_port()
    
    def update_ports(self):
        self.ports = logic.serial_ports()
        self.port_selection.clear()
        self.port_selection.insertItems(0, self.ports)
    
    def connect_port(self):
        self.port = serial.Serial(self.port_selection.currentText(), int(self.baud_rate_line.text()))
        print(self.port)
    
    def get_port(self) -> serial.Serial:
        return self.port

class ControlApp(QMainWindow):
    version = "0.0.1"
    global_font_size = 12
    font = None
    ports = None

    def __init__(self, font_size: int):
        super().__init__()
        self.setWindowTitle(f"Stepper motor control app v. {self.version}")

        self.global_font_size = font_size
        self.font = QFont()
        self.font.setFamily(self.font.defaultFamily())
        self.font.setPointSize(self.global_font_size)

        self.main_layout = QHBoxLayout()
        self.overhead_layout = QVBoxLayout()
        self.container = QWidget()

        self.steps_vernier = Vernier("Steps", self.font)
        self.delay_vernier = Vernier("Delay, ms", self.font)

        self.panel = CommandPanel(["Forward!", "Reverse!", "Halt!"], 
            [self.forward, self.reverse, self.halt], self.font)
        self.ports_panel = SerialPortSelection(self.font)

        self.response_lcd = QLabel()
        self.response_lcd.setFont(self.font)
        self.response_lcd.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.response_lcd.setText(str(logic.ACK_RESP))

        self.main_layout.addWidget(self.panel)
        self.main_layout.addWidget(self.steps_vernier)
        self.main_layout.addWidget(self.delay_vernier)
        self.main_container = QWidget()
        self.main_container.setLayout(self.main_layout)

        self.overhead_layout.addWidget(self.main_container)
        self.overhead_layout.addWidget(self.ports_panel)
        self.overhead_layout.addWidget(self.response_lcd)
        self.container.setLayout(self.overhead_layout)
        self.setCentralWidget(self.container)
    
    def forward(self):
        print(self.ports_panel.port.write(logic.rotate_command(
            logic.CLOCKWISE_BYTE,
            self.steps_vernier.value,
            self.delay_vernier.value)))
        self.response_lcd.setText(str(self.ports_panel.port.read()))
    
    def reverse(self):
        print(self.ports_panel.port.write(logic.rotate_command(
            logic.COUNTERCLOCKWISE_BYTE,
            self.steps_vernier.value,
            self.delay_vernier.value)))
        self.response_lcd.setText(str(self.ports_panel.port.read()))
    
    def halt(self):
        print(self.ports_panel.port.write(logic.halt_command()))
        self.response_lcd.setText(str(self.ports_panel.port.read()))

app = QApplication([])
window = ControlApp(12)
window.show()
app.exec()
        
