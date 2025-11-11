package main

import (
	"encoding/json"
	"io"
	"log"
	"net/http"
	"os"
)

type Reader interface {
	Read(*http.Response)
}

type ReaderFile struct {
	filepath string
}

func (reader ReaderFile) Read(resp *http.Response) {
	out, err := os.Create(reader.filepath)
	if err != nil {
	}
	defer out.Close()

	_, err = io.Copy(out, resp.Body)
	if err != nil {
	}
}

type ReaderString struct {
	str *string
}

func (reader ReaderString) Read(resp *http.Response) {
	out, err := io.ReadAll(resp.Body)
	if err != nil {
	}

	if out == nil {
		*(reader.str) = "nil"
	}
	*(reader.str) = string(out)
}

type ReaderJson struct {
	out *map[string]any
}

func (reader ReaderJson) Read(resp *http.Response) {
	decoder := json.NewDecoder(resp.Body)
	err := decoder.Decode(reader.out)
	if err != nil {
		log.Printf("%#v", err)
	}
	defer resp.Body.Close()
}
