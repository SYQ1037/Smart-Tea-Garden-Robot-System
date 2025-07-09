import sys
import random
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QFileDialog, QListWidgetItem
from PyQt5.QtCore import QThread, pyqtSignal, QDateTime, Qt, QSize, QRect
from PyQt5.QtGui import QPixmap, QImage
import cv2
from ultralytics import YOLO
from Central import Ui_mainWindow
import serial
import serial.tools.list_ports
import DeepSeek
import Chat
import requests
import json

class MainWindow(QtWidgets.QMainWindow, Ui_mainWindow):
    def __init__(self):
        super().__init__()
        try:
            self.setupUi(self)

            self.model = YOLO("best.pt")
            self.timer = QtCore.QTimer()
            self.timer.timeout.connect(self.update_frame)

            self.serial_port = None
            self.serial_timer = QtCore.QTimer()
            self.serial_timer.timeout.connect(self.read_serial_data)

            self.cap = None
            self.is_detection_active = False
            self.current_frame = None
            self.show_soil_data = False

            self.open_camera_button.clicked.connect(self.start_camera)
            self.close_camera_button.clicked.connect(self.stop_detection)

            self.text_update_timer = QtCore.QTimer()
            self.text_update_timer.timeout.connect(self.update_text)
            self.text_update_timer.start(1000)

            self.soil_data_timer = QtCore.QTimer()
            self.soil_data_timer.timeout.connect(self.update_soil_data)
            self.init_serial_connection()

            # DS聊天功能初始化
            self.deepseek_api_key = "sk-2e90b9ce03af47ecaf039c152609f645"  # API密钥
            self.chat_thread = None
            self.send_button.clicked.connect(self.send_chat_message)
            self.chat_input.installEventFilter(self)

        except Exception as e:
            print(f"初始化错误: {e}")
            QtWidgets.QMessageBox.critical(self, '错误', f'初始化失败: {str(e)}')

    def eventFilter(self, obj, event):
        if obj == self.chat_input and event.type() == QtCore.QEvent.KeyPress:
            if event.key() in (QtCore.Qt.Key_Return, QtCore.Qt.Key_Enter):
                if not (event.modifiers() & QtCore.Qt.ShiftModifier):
                    self.send_chat_message()
                    return True
        return super().eventFilter(obj, event)

    def send_chat_message(self):
        message = self.chat_input.toPlainText().strip()
        if not message:
            return

        self.add_chat_message(message, Chat.RoleType.USER)
        self.chat_input.clear()

        self.chat_thread = DeepSeek.DeepSeekThread(message)
        self.chat_thread.response_received.connect(self.handle_chat_response)
        self.chat_thread.finished.connect(self.chat_thread.deleteLater)
        self.chat_thread.start()

        self.ai_message_item = QListWidgetItem(self.chat_list)
        self.ai_message_widget = Chat.ChatMessageWidget()
        self.ai_message_widget.set_message("思考中...", QDateTime.currentDateTime().toString("hh:mm:ss"),
                                           QSize(self.chat_list.width(), 60), Chat.RoleType.SYSTEM)
        self.chat_list.setItemWidget(self.ai_message_item, self.ai_message_widget)
        self.chat_list.scrollToBottom()

    def handle_chat_response(self, response):
        if not hasattr(self, 'ai_message_widget'):
            return

        current_text = self.ai_message_widget.message_text
        if current_text == "思考中...":
            current_text = ""

        self.ai_message_widget.set_message(
            current_text + response,
            QDateTime.currentDateTime().toString("hh:mm:ss"),
            QSize(self.chat_list.width(), self.calculate_message_height(current_text + response)),
            Chat.RoleType.SYSTEM
        )
        self.ai_message_item.setSizeHint(QSize(
            self.chat_list.width(),
            self.calculate_message_height(current_text + response)
        ))
        self.chat_list.scrollToBottom()

    def add_chat_message(self, message, msg_type):
        time_str = QDateTime.currentDateTime().toString("hh:mm:ss")
        message_height = self.calculate_message_height(message)

        time_item = QListWidgetItem(self.chat_list)
        time_widget = Chat.ChatMessageWidget()
        time_widget.set_message(time_str, time_str, QSize(self.chat_list.width(), 30), Chat.RoleType.TIME)
        time_item.setSizeHint(QSize(self.chat_list.width(), 30))
        self.chat_list.setItemWidget(time_item, time_widget)

        message_item = QListWidgetItem(self.chat_list)
        message_widget = Chat.ChatMessageWidget()
        message_widget.set_message(message, time_str, QSize(self.chat_list.width(), message_height), msg_type)
        message_item.setSizeHint(QSize(self.chat_list.width(), message_height))
        self.chat_list.setItemWidget(message_item, message_widget)

        self.chat_list.scrollToBottom()

    def calculate_message_height(self, text):
        line_count = text.count('\n') + 1
        if len(text) > 50 and '\n' not in text:
            line_count += len(text) // 50
        return max(60, line_count * 20 + 40)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        for i in range(self.chat_list.count()):
            item = self.chat_list.item(i)
            widget = self.chat_list.itemWidget(item)
            if widget:
                widget.setFixedWidth(self.chat_list.width())
                item.setSizeHint(QSize(
                    self.chat_list.width(),
                    self.calculate_message_height(widget.message_text)
                ))

    # def send_message(self):
    #     user_message = self.user_input.toPlainText().strip()
    #     if not user_message:
    #         return
    #
    #     self.append_message("用户", user_message)
    #     self.user_input.clear()
    #
    #     self.chat_thread = DeepSeek.DeepSeekChatThread(self.deepseek_api_key, user_message)
    #     self.chat_thread.response_received.connect(self.handle_response)
    #     self.chat_thread.start()
    #
    # def handle_response(self, response):
    #     self.append_message("DeepSeek", response)
    #
    # def append_message(self, sender, message):
    #     current_time = QtCore.QDateTime.currentDateTime().toString("hh:mm:ss")
    #     formatted_message = f"""
    #     <div style='margin-bottom: 10px;'>
    #         <div style='font-weight: bold; color: {'#00b357' if sender == 'DeepSeek' else '#333'};'>
    #             {sender} <span style='font-size: 10px; color: #999;'>{current_time}</span>
    #         </div>
    #         <div style='margin-left: 10px;'>{message}</div>
    #     </div>
    #     """
    #
    #     scrollbar = self.chat_history.verticalScrollBar()
    #     at_bottom = scrollbar.value() == scrollbar.maximum()
    #     self.chat_history.insertHtml(formatted_message)
    #     if at_bottom:
    #         scrollbar.setValue(scrollbar.maximum())

    def init_serial_connection(self):
        try:
            ports = serial.tools.list_ports.comports()
            for port in ports:
                try:
                    self.serial_port = serial.Serial(port.device, 9600, timeout=1)
                    if self.serial_port.is_open:
                        print(f"已连接到串口: {port.device}")
                        self.serial_timer.start(100)
                        return
                except:
                    continue
            print("未找到可用的串口")
        except Exception as e:
            print(f"串口连接错误: {e}")

    def read_serial_data(self):
        if self.serial_port and self.serial_port.in_waiting:
            try:
                data = self.serial_port.readline().decode('utf-8').strip()
                print(f"收到串口数据: {data}")

                if data == "1":
                    self.show_soil_data = True
                    if not self.soil_data_timer.isActive():
                        self.soil_data_timer.start(1000)
                    self.start_camera()

                elif data == "2":
                    self.show_soil_data = False
                    self.clear_soil_data()
                    self.stop_detection()

            except Exception as e:
                print(f"串口数据处理错误: {e}")

    def update_soil_data(self):
        if not self.show_soil_data:
            return
        try:
            # 土壤温度 (15.0-30.0)
            temp = round(random.uniform(15.0, 30.0), 1)
            if hasattr(self, 'soil_temp_label'):
                self.soil_temp_label.setText(f"{temp} °C")

            # 土壤湿度 (30.0-80.0)
            humidity = round(random.uniform(30.0, 80.0), 1)
            if hasattr(self, 'soil_humidity_label'):
                self.soil_humidity_label.setText(f"{humidity} %")

            # 土壤pH (5.0-8.0)
            ph = round(random.uniform(5.0, 8.0), 1)
            if hasattr(self, 'soil_ph_label'):
                self.soil_ph_label.setText(f"{ph}")

            # 氮含量 (50-200 mg/kg)
            nitrogen = random.randint(50, 200)
            if hasattr(self, 'soil_nitrogen_label'):
                self.soil_nitrogen_label.setText(f"{nitrogen} mg/kg")

            # 钾含量 (100-300 mg/kg)
            potassium = random.randint(100, 300)
            if hasattr(self, 'soil_potassium_label'):
                self.soil_potassium_label.setText(f"{potassium} mg/kg")

            # 磷含量 (20-100 mg/kg)
            phosphorus = random.randint(20, 100)
            if hasattr(self, 'soil_phosphorus_label'):
                self.soil_phosphorus_label.setText(f"{phosphorus} mg/kg")
        except Exception as e:
            print(f"更新土壤数据错误: {e}")

    def clear_soil_data(self):
        if hasattr(self, 'soil_temp_label'):
            self.soil_temp_label.setText("")
        if hasattr(self, 'soil_humidity_label'):
            self.soil_humidity_label.setText("")
        if hasattr(self, 'soil_ph_label'):
            self.soil_ph_label.setText("")
        if hasattr(self, 'soil_nitrogen_label'):
            self.soil_nitrogen_label.setText("")
        if hasattr(self, 'soil_potassium_label'):
            self.soil_potassium_label.setText("")
        if hasattr(self, 'soil_phosphorus_label'):
            self.soil_phosphorus_label.setText("")

    def update_text(self):
        try:
            temp = round(random.uniform(22.75, 23.12), 2)
            if hasattr(self, 'wendu_tu'):
                self.wendu_tu.setPlainText(f"{temp:.2f}℃")

            humidity = round(random.uniform(54.39, 58.13), 2)
            if hasattr(self, 'shidu_tu'):
                self.shidu_tu.setPlainText(f"{humidity:.2f}%")

            pm25 = random.randint(23, 27)
            if hasattr(self, 'ph_3'):
                self.ph_3.setPlainText(f"{pm25}ug/m³")

            pm10 = random.randint(22, 25)
            if hasattr(self, 'ph_4'):
                self.ph_4.setPlainText(f"{pm10}ug/m³")

            co2 = random.randint(900, 1200)
            if hasattr(self, 'ec'):
                self.ec.setPlainText(f"{co2}ppm")

        except Exception as e:
            print(f"更新文本错误: {e}")

    def load_picture(self):
        try:
            fileName, _ = QFileDialog.getOpenFileName(self, "选择图片", "", "Image Files (*.jpg *.png)")
            self.is_detection_active = False

            if fileName:
                if self.timer.isActive():
                    self.timer.stop()
                if self.cap:
                    self.cap.release()
                    self.cap = None

                self.current_frame = cv2.imread(fileName)
                self.display_image(self.current_frame, self.camera_label)
                results = self.model.predict(self.current_frame)
                self.detected_frame = results[0].plot()
                self.display_image(self.detected_frame, self.result_label)
        except Exception as e:
            print(f"加载图片错误: {e}")
            QtWidgets.QMessageBox.warning(self, '错误', f'加载图片失败: {str(e)}')

    def load_video(self):
        try:
            fileName, _ = QFileDialog.getOpenFileName(self, "选择视频", "", "Video Files (*.mp4 *.avi)")
            if fileName:
                if self.cap:
                    self.cap.release()
                    self.cap = None

                self.cap = cv2.VideoCapture(fileName)

                if self.cap.isOpened():
                    ret, frame = self.cap.read()
                    if ret:
                        self.current_frame = frame.copy()
                        self.display_image(frame, self.camera_label)
                        self.display_image(frame, self.result_label)
                    else:
                        QtWidgets.QMessageBox.warning(self, '错误', '无法读取视频文件的第一帧。')
        except Exception as e:
            print(f"加载视频错误: {e}")
            QtWidgets.QMessageBox.warning(self, '错误', f'加载视频失败: {str(e)}')

    def start_camera(self):
        try:
            self.is_detection_active = False
            if self.cap:
                self.cap.release()
            self.cap = cv2.VideoCapture(0)
            self.timer.start(20)
            self.is_detection_active = True
        except Exception as e:
            print(f"启动摄像头错误: {e}")
            QtWidgets.QMessageBox.warning(self, '错误', f'无法启动摄像头: {str(e)}')

    def update_frame(self):
        try:
            if self.cap:
                ret, frame = self.cap.read()
                if ret:
                    self.current_frame = frame.copy()
                    self.display_image(frame, self.camera_label)

                    if self.is_detection_active:
                        results = self.model.predict(frame)
                        self.detected_frame = results[0].plot()
                        self.display_image(self.detected_frame, self.result_label)
        except Exception as e:
            print(f"更新帧错误: {e}")

    def display_image(self, frame, target_label):
        try:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            height, width, channel = frame.shape
            step = channel * width
            qImg = QImage(frame.data, width, height, step, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(qImg)
            scaled_pixmap = pixmap.scaled(target_label.size(), QtCore.Qt.KeepAspectRatio)
            target_label.setPixmap(scaled_pixmap)
        except Exception as e:
            print(f"显示图像错误: {e}")

    def stop_detection(self):
        try:
            self.is_detection_active = False
            if self.timer.isActive():
                self.timer.stop()
            if self.cap:
                self.cap.release()
                self.cap = None
            self.camera_label.clear()
            self.result_label.clear()
        except Exception as e:
            print(f"停止检测错误: {e}")

    def closeEvent(self, event):
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
        if self.cap:
            self.cap.release()
        event.accept()

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())