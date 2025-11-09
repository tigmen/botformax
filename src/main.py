from fastapi import FastAPI, File
from pydantic import BaseModel
import whisper

class VoiceTranscriber:
    def __init__(self):
        self.model = whisper.load_model("base")
    def audio_to_text(self, audio_file,language="ru"):
        result = self.model.transcribe(audio_file,language=language)
        return result["text"]

class Data(BaseModel):
    file_name: str
    file_bytes: str

transcriber = VoiceTranscriber()
app = FastAPI()

@app.post("/decode/")
def decodeFile(file: bytes = File()):
    if file is None:
        return "None response"
    else:
        path = "file.ogg"
        with open(path, "wb") as f:
            f.write(file)

        out = transcriber.audio_to_text(path)
        return out

