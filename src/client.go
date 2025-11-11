package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"mime/multipart"
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

const GETUPDATES = "https://platform-api.max.ru/updates"

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
}

func GetUpdates(token string) Updates {
	client := &http.Client{}

	req, err := http.NewRequest("GET", GETUPDATES, nil)
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
	return updates

}

func GetAudio(token, url string) []byte {
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

	var reader Reader = ReaderFile{"file.ogg"}
	reader.Read(resp)
	out, err := os.ReadFile("file.ogg")

	if err != nil {
		log.Fatalf("Ошибка чтения файла: %v", err)
	}

	return out
}

const SENDMESSAGE = "https://platform-api.max.ru/messages?chat_id="

type link struct {
	Type string `json:"type"`
	Mid  string `json:"mid"`
}
type sendMessage struct {
	Chat_id int64  `json:"-"`
	Text    string `json:"text,omitempty"`
	Link    link   `json:"link"`
}

func SendMessage(token string, message sendMessage) {
	client := &http.Client{}

	data, err := json.Marshal(message)
	if err != nil {
		log.Printf("%#v", err)
	}

	log.Printf("%s\n", data)

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

	log.Printf("\n%s\n", out)
}

const MODELAPIURL = "http://localhost:8080/decode/"

func DecodeFile() string {
	file, err := os.Open("file.ogg")
	if err != nil {
	}
	defer file.Close()

	form := new(bytes.Buffer)
	writer := multipart.NewWriter(form)
	part, err := writer.CreateFormFile("file", "file.ogg")

	if err != nil {
	}

	_, err = io.Copy(part, file)
	if err != nil {
	}

	writer.Close()

	client := &http.Client{}
	req, err := http.NewRequest("POST", MODELAPIURL, form)
	if err != nil {
		log.Printf("%#v", err)
	}

	req.Header.Set("Content-Type", writer.FormDataContentType())

	resp, err := client.Do(req)
	if err != nil {
		log.Printf("%#v", err)
	}

	defer resp.Body.Close()

	var out string

	var reader Reader = ReaderString{&out}
	reader.Read(resp)

	return out
}

func handle(token string) {
	for _, update := range GetUpdates(token).Updates {
		message := update.Message

		log.Printf("%#v\n", message)
		for _, attachment := range message.Body.Attachments {
			switch attachment.Type {
			case "audio":
				go func() {
					GetAudio(token, attachment.Payload.Url)
					out := DecodeFile()

					resp := sendMessage{Chat_id: message.Recipient.Chat_id, Text: out, Link: link{Type: "reply", Mid: message.Body.Mid}}
					SendMessage(token, resp)

				}()
			default:
			}

		}
	}
}

func main() {
	token := os.Args[1]
	done := make(chan bool, 1)

	go func() {
		var n int
		fmt.Scanln(&n)
		done <- true
	}()

	for len(done) == 0 {
		handle(token)
	}

	<-done
}
