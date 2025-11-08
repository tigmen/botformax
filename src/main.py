from fastapi import FastAPI
from typing import Optional
import whisper

class VoiceTranscriber:
    def __init__(self):
        self.model = whisper.load_model("base")
    def audio_to_text(self, audio_file,language="ru"):
        result = self.model.transcribe(audio_file,language=language)
        return result["text"]
transcriber = VoiceTranscriber()
app = FastAPI()

@app.get("/decode")
def decodeFile(path: Optional[str] = None):
    if path is None:
        return "None path"
    else:
        out = transcriber.audio_to_text(path)
        return out

