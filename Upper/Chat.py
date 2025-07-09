from PySide6.QtWidgets import QWidget, QLabel, QTextEdit
from PySide6.QtGui import QFont, QPixmap, QPainter, QColor, QPen, QTextOption, QPainterPath, QFontMetrics
from PySide6.QtCore import QRect, QSize, Qt, QDateTime

class RoleType:
    SYSTEM = 1
    USER = 2
    TIME = 3
    ASSISTANT = 1

class ChatMessageWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFont(QFont("Microsoft YaHei", 10))

        # 初始化属性
        self.message_text = ""
        self.message_time = ""
        self.message_type = RoleType.SYSTEM
        self.message_size = QSize()

        self.ai_avatar = QPixmap("chat/img.png")
        self.user_avatar = QPixmap("chat/农民.png")

        # 布局参数
        self.avatar_size = 40
        self.spacing = 10
        self.triangle_width = 6
        self.border_radius = 5
        self.text_padding = 12

    def set_message(self, text, time, size, msg_type):
        self.message_text = text
        self.message_time = time
        self.message_size = size
        self.message_type = msg_type
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing | QPainter.TextAntialiasing)

        text_width = self.width() - 2 * (self.avatar_size + self.spacing + self.triangle_width)
        text_rect = QRect(0, 0, text_width, self.height())

        if self.message_type == RoleType.SYSTEM:
            avatar_rect = QRect(self.spacing, self.spacing, self.avatar_size, self.avatar_size)
            bubble_rect = QRect(
                self.avatar_size + self.spacing + self.triangle_width,
                self.spacing,
                text_width,
                self.height() - 2 * self.spacing
            )
            painter.drawPixmap(avatar_rect, self.ai_avatar)
            path = QPainterPath()
            path.addRoundedRect(bubble_rect, self.border_radius, self.border_radius)
            path.moveTo(bubble_rect.left(), bubble_rect.top() + 20)
            path.lineTo(bubble_rect.left() - self.triangle_width, bubble_rect.top() + 25)
            path.lineTo(bubble_rect.left(), bubble_rect.top() + 30)
            path.closeSubpath()
            painter.fillPath(path, QColor(240, 240, 240))

            painter.setPen(QColor(51, 51, 51))
            text_option = QTextOption(Qt.AlignLeft | Qt.AlignVCenter)
            text_option.setWrapMode(QTextOption.WrapAtWordBoundaryOrAnywhere)
            painter.drawText(bubble_rect.adjusted(self.text_padding, self.text_padding, -self.text_padding, -self.text_padding),
                             self.message_text, text_option)

        elif self.message_type == RoleType.USER:
            avatar_rect = QRect(
                self.width() - self.avatar_size - self.spacing,
                self.spacing,
                self.avatar_size,
                self.avatar_size
            )
            bubble_rect = QRect(
                self.width() - self.avatar_size - self.spacing - text_width - self.triangle_width,
                self.spacing,
                text_width,
                self.height() - 2 * self.spacing
            )
            painter.drawPixmap(avatar_rect, self.user_avatar)
            path = QPainterPath()
            path.addRoundedRect(bubble_rect, self.border_radius, self.border_radius)

            path.moveTo(bubble_rect.right(), bubble_rect.top() + 20)
            path.lineTo(bubble_rect.right() + self.triangle_width, bubble_rect.top() + 25)
            path.lineTo(bubble_rect.right(), bubble_rect.top() + 30)
            path.closeSubpath()

            painter.fillPath(path, QColor(0, 179, 87))

            painter.setPen(Qt.white)
            text_option = QTextOption(Qt.AlignLeft | Qt.AlignVCenter)
            text_option.setWrapMode(QTextOption.WrapAtWordBoundaryOrAnywhere)
            painter.drawText(bubble_rect.adjusted(self.text_padding, self.text_padding, -self.text_padding, -self.text_padding),
                             self.message_text, text_option)
