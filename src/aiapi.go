package main

import (
	"bytes"
	"io"
	"log"
	"mime/multipart"
	"net/http"
	"os"
)

const MODELAPIURL = "http://localhost:8080/decode/"

type sendFile struct {
	File_name  string `json:"file_name"`
	File_bytes string `json:"file_bytes"`
}

func DecodeFile(filename string) string {
	file, err := os.Open(filename)
	if err != nil {
		log.Printf("%#v", err)
	}
	defer file.Close()

	form := new(bytes.Buffer)
	writer := multipart.NewWriter(form)
	part, err := writer.CreateFormFile("file", filename)
	if err != nil {
		log.Printf("%#v", err)
	}

	_, err = io.Copy(part, file)
	if err != nil {
		log.Printf("%#v", err)
	}

	writer.Close()

	client := &http.Client{}
	req, err := http.NewRequest("POST", MODELAPIURL, form)
	if err != nil {
		log.Printf("%#v", err)
	}

	req.Header.Set("Content-Type", writer.FormDataContentType())

	var out string = ""

	resp, err := client.Do(req)
	if err != nil {
		log.Printf("%#v", err)
	} else {

		defer resp.Body.Close()

		var reader Reader = ReaderString{&out}
		reader.Read(resp)

	}
	return out
}
