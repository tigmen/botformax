import whisper
class VoiceTranscriber:
    def __init__(self):
        self.model = whisper.load_model("base")
    def audio_to_text(self, audio_file,language="ru"):
        result = self.model.transcribe(audio_file,language=language)
        return result["text"]
transcriber = VoiceTranscriber()
audio_path = r"C:\Users\Гоша\Desktop\voice_09-12-2024_11-35-27.oga"
print (transcriber.audio_to_text(audio_path))