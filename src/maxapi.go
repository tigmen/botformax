package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
)

const GETUPDATES = "https://platform-api.max.ru/updates"
const SENDMESSAGE = "https://platform-api.max.ru/messages?chat_id="

type Payload struct {
	Url   string
	Token string
}
type Attachment struct {
	Type    string
	Payload Payload
}

type Body struct {
	Mid         string
	Text        string
	Attachments []Attachment
}

type Recipient struct {
	Chat_id int64
	User_id int64
}
type Sender struct {
	User_id    int64
	First_name string
	Last_name  string
	Username   string
}
type Message struct {
	Body      Body
	Recipient Recipient
	Sender    Sender
}

type Update struct {
	Message Message
}
type Updates struct {
	Updates []Update
	Marker  int64
}
type getUpdates struct {
	Marker int64 `json:"marker"`
}

type link struct {
	Type string `json:"type"`
	Mid  string `json:"mid"`
}
type sendMessage struct {
	Chat_id int64  `json:"-"`
	Text    string `json:"text,omitempty"`
	Link    link   `json:"link"`
}

func GetUpdates(token string, marker *int64) Updates {
	client := &http.Client{}

	data, err := json.Marshal(getUpdates{Marker: *marker})
	if err != nil {
		log.Printf("%#v", err)
	}

	req, err := http.NewRequest("GET", GETUPDATES, bytes.NewBuffer(data))
	if err != nil {
		log.Printf("%#v", err)
	}
	req.Header.Add("Authorization", token)
	resp, err := client.Do(req)

	if err != nil {
		log.Printf("%#v", err)
	}
	defer resp.Body.Close()

	var out string
	var reader Reader = ReaderString{&out}

	reader.Read(resp)

	var updates Updates
	err = json.Unmarshal([]byte(out), &updates)
	if err != nil {
		log.Printf("%s\n", err)
	}

	if updates.Marker > 0 {
		*marker = updates.Marker
	}

	return updates
}

func GetAudio(token, url, filename string) []byte {
	client := &http.Client{}

	req, err := http.NewRequest("GET", url, nil)
	if err != nil {
		log.Printf("%#v", err)
	}
	req.Header.Add("Authorization", token)
	resp, err := client.Do(req)

	if err != nil {
		log.Printf("%#v", err)
	}

	var reader Reader = ReaderFile{filename}
	reader.Read(resp)
	out, err := os.ReadFile(filename)

	if err != nil {
		log.Fatalf("Ошибка чтения файла: %v", err)
	}

	return out
}

func SendMessage(token string, message sendMessage) {
	client := &http.Client{}

	data, err := json.Marshal(message)
	if err != nil {
		log.Printf("%#v", err)
	}

	req, err := http.NewRequest("POST", SENDMESSAGE+fmt.Sprint(message.Chat_id), bytes.NewReader(data))
	if err != nil {
		log.Printf("%#v", err)
	}

	req.Header.Add("Authorization", token)

	resp, err := client.Do(req)
	if err != nil {
		log.Printf("%#v", err)
	}

	defer resp.Body.Close()

	var out string
	var reader Reader = ReaderString{&out}
	reader.Read(resp)
}
