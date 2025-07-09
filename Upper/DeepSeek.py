from PyQt5.QtCore import QThread, pyqtSignal
import requests
import json

# DeepSeek API交互类
class DeepSeekThread(QThread):
    response_received = pyqtSignal(str)
    finished = pyqtSignal()

    def __init__(self, message, parent=None):
        super().__init__(parent)
        self.message = message
        self.api_key = "sk-2e90b9ce03af47ecaf039c152609f645"

    def run(self):
        try:
            headers = {
                "Authorization": f"Bearer {self.api_key}",
                "Content-Type": "application/json"
            }

            data = {
                "model": "deepseek-chat",
                "messages": [{"role": "user", "content": self.message}],
                "temperature": 0.7,
                "stream": True
            }

            response = requests.post(
                "https://api.deepseek.com/v1/chat/completions",
                headers=headers,
                json=data,
                stream=True
            )

            if response.status_code == 200:
                for line in response.iter_lines():
                    if line:
                        decoded_line = line.decode('utf-8')
                        if decoded_line.startswith("data:"):
                            try:
                                json_data = json.loads(decoded_line[5:])
                                if "content" in json_data["choices"][0]["delta"]:
                                    content = json_data["choices"][0]["delta"]["content"]
                                    self.response_received.emit(content)
                            except json.JSONDecodeError:
                                pass
            else:
                self.response_received.emit(f"API请求失败: {response.status_code}")

        except Exception as e:
            self.response_received.emit(f"请求异常: {str(e)}")
        finally:
            self.finished.emit()