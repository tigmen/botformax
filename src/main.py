from fastapi import FastAPI
from typing import Optional

app = FastAPI()

@app.get("/decode")
def decodeFile(path: Optional[str] = None):
    if path is None:
        return "None path"
    else:
        return "OK" + path

