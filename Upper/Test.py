import sys
import random
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QFileDialog, QListWidgetItem
from PyQt5.QtCore import QThread, pyqtSignal, QDateTime, Qt, QSize, QRect, QTimer
from PyQt5.QtGui import QPixmap, QImage
import cv2
import torch
import yolov5
import numpy as np
from yolov5.utils.general import non_max_suppression, scale_boxes
from yolov5.utils.augmentations import letterbox
from yolov5.utils.plots import Annotator, colors
from yolov5.models.experimental import attempt_load
from Central import Ui_mainWindow
import serial
import serial.tools.list_ports
from time import sleep
import DeepSeek
import Chat
import requests
import json
import platform
import pathlib
plt = platform.system()
if plt != 'Windows':
    pathlib.WindowsPath = pathlib.PosixPath

class SerialThread(QThread):
    data_received = pyqtSignal(int)
    serial_port = serial.Serial("/dev/ttyS4", 9600, 8, 'N', 1)
    def run(self):
        while True:
            try:
                if self.serial_port and self.serial_port.in_waiting:
                    try:
                        data = self.serial_port.read(3)
                        print(data)
                        num = int(data[0:1])
                        print(num)
                        self.data_received.emit(num)
                    except Exception as e:
                        print(f"串口数据处理错误: {e}")
            except ValueError:
                print("请输入数字！")

class MainWindow(QtWidgets.QMainWindow, Ui_mainWindow):
    def __init__(self):
        super().__init__()
        try:
            self.setupUi(self)

            self.model = attempt_load("/home/cat/PycharmProjects/Upper/best.pt")
            self.model.eval()
            self.timer = QtCore.QTimer()
            self.timer.timeout.connect(self.update_frame)

            self.serial_port = None
            # self.serial_timer = QtCore.QTimer()
            # self.serial_timer.timeout.connect(self.read_serial_data)

            self.cap = None
            self.is_detection_active = False
            self.current_frame = None
            self.show_soil_data = True

            self.open_camera_button.clicked.connect(self.start_camera)
            self.close_camera_button.clicked.connect(self.stop_detection)

            self.text_update_timer = QtCore.QTimer()
            self.text_update_timer.timeout.connect(self.update_text)
            self.text_update_timer.start(1000)

            self.soil_data_timer = QtCore.QTimer()
            self.soil_data_timer.timeout.connect(self.update_soil_data)
            #self.soil_data_timer.start(1000)
            self.init_serial_connection()
            self.serial_thread = SerialThread()
            self.serial_thread.data_received.connect(self.handle_serial_data)
            self.serial_thread.start()
            self.send_button.clicked.connect(self.send_chat_message)
            self.textEdit_8.hide()
            self.label_42.hide()
            self.textEdit_7.hide()
            self.label_41.hide()

        except Exception as e:
            print(f"初始化错误: {e}")
            QtWidgets.QMessageBox.critical(self, '错误', f'初始化失败: {str(e)}')

    def init_serial_connection(self):
        try:
            self.serial_port = serial.Serial("/dev/ttyS4", 9600, 8, 'N', 1)
            if self.serial_port.is_open:
                print(f"已连接到串口")
                self.serial_timer.start(100)
        except Exception as e:
            print(f"串口连接错误: {e}")

    def handle_serial_data(self, num):
        if num == 1:
            self.show_soil_data = False
            self.clear_soil_data()
            self.start_camera()
            QTimer.singleShot(10000,self.stop_detection)

        elif num == 2:
            self.show_soil_data = True
            if not self.soil_data_timer.isActive():
                self.soil_data_timer.start(1000)
            self.update_soil_data()
            #self.stop_detection()
            self.clear_soil_data()
        else:
            print("Error")

    def read_serial_data(self):
        while True:
            num=int(input("输入操作:"))
            print(num)
            if num == 1:
                self.show_soil_data = False
                self.clear_soil_data()
                self.start_camera()
                sleep(10)
                self.stop_detection()

            elif num == 2:
                self.show_soil_data = True
                self.stop_detection()
                if not self.soil_data_timer.isActive():
                    self.soil_data_timer.start(1000)
                self.update_soil_data()
                sleep(10)
                self.clear_soil_data()
            else:
                print("Error")

    def update_soil_data(self):
        if not self.show_soil_data:
            return
        try:
            #print("update")
            # 土壤温度 (15.0-30.0)
            temp = round(random.uniform(15.0, 30.0), 1)
            self.wendu_tu_2.setText(f"{temp} °C")

            # 土壤湿度 (30.0-80.0)
            humidity = round(random.uniform(30.0, 80.0), 1)
            self.shidu_tu_2.setText(f"{humidity} %")

            # 土壤pH (5.0-8.0)
            ph = round(random.uniform(5.0, 8.0), 1)
            self.ph_6.setText(f"{ph}")

            # 氮含量 (50-200 mg/kg)
            nitrogen = random.randint(50, 200)
            self.ph_5.setText(f"{nitrogen} mg/kg")

            # 钾含量 (100-300 mg/kg)
            potassium = random.randint(100, 300)
            self.ec_5.setText(f"{potassium} mg/kg")

            # 磷含量 (20-100 mg/kg)
            phosphorus = random.randint(20, 100)
            self.ec_2.setText(f"{phosphorus} mg/kg")
        except Exception as e:
            print(f"更新土壤数据错误: {e}")

    def clear_soil_data(self):
        self.wendu_tu_2.setText("0")
        self.shidu_tu_2.setText("0")
        self.ph_6.setText("0")
        self.ph_5.setText("0")
        self.ec_5.setText("0")
        self.ec_2.setText("0")
        QTimer.singleShot(10000, self.soil_data_timer.stop)

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
                    frame = frame.copy()
                    self.current_frame = frame.copy()
                    self.display_image(frame, self.camera_label)
                    if self.is_detection_active:
                        # YOLOv5预处理
                        img = letterbox(frame, new_shape=640)[0]
                        img = img[:, :, ::-1].transpose(2, 0, 1)
                        img = np.ascontiguousarray(img)
                        img = torch.from_numpy(img).to('cpu')
                        img = img.float()
                        img /= 255.0
                        if img.ndimension() == 3:
                            img = img.unsqueeze(0)
                        with torch.no_grad():
                            pred = self.model(img)[0]
                        pred = non_max_suppression(pred, conf_thres=0.25, iou_thres=0.45)
                        det = pred[0]
                        if det is not None and len(det):
                            det[:, :4] = scale_boxes(img.shape[2:], det[:, :4], frame.shape).round()
                            annotator = Annotator(frame, line_width=3)
                            for *xyxy, conf, cls in reversed(det):
                                c = int(cls)
                                label = f'{self.model.names[c]} {conf:.2f}'
                                annotator.box_label(xyxy, label, color=colors(c, True))

                            self.detected_frame = annotator.result()
                            self.display_image(self.detected_frame, self.result_label)
                        else:
                            self.display_image(frame, self.result_label)
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

    def show_ai_response(self):
        QtCore.QTimer.singleShot(10000, lambda: [
            self.textEdit_7.show(),
            self.label_41.show()
        ])
    def send_chat_message(self):
        # 清空输入框
        self.textEdit_input.clear()
        self.textEdit_8.show()
        self.label_42.show()
        self.show_ai_response()
        # message = self.chat_input.toPlainText()
        # print(message)
        # time_str = QDateTime.currentDateTime().toString("hh:mm:ss")
        # print(time_str)
        # # 清空输入框
        # self.chat_input.clear()
        #
        # user_msg_widget = Chat.ChatMessageWidget()
        # user_msg_widget.set_message(message, time_str, QSize(), Chat.RoleType.USER)
        # self.chat_layout.addWidget(user_msg_widget)
        #
        # # 创建并启动线程
        # self.chat_thread = ChatRequestThread(message)
        # self.chat_thread.result_signal.connect(lambda response: self.show_ai_response(response, time_str))
        # self.chat_thread.error_signal.connect(self.handle_chat_error)
        # self.chat_thread.start()

    #def show_ai_response(self, response, time_str):
        # ai_msg_widget = Chat.ChatMessageWidget()
        # ai_msg_widget.set_message(response, time_str, QSize(), Chat.RoleType.SYSTEM)
        # self.chat_layout.addWidget(ai_msg_widget)

    def handle_chat_error(self, error):
        print(f"聊天请求出错: {error}")

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())