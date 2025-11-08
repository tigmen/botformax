package main

import (
	"bytes"
	"encoding/json"
	"fmt"
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

const GETUPDATES = "https://platform-api.max.ru/updates"

const (
	Audio int = 0
	Video
)

type Attachment struct {
	_type int
	url   string
	token string
}

func ParseAttachment(attachment map[string]any) Attachment {
	new_attachment := new(Attachment)

	switch attachment["type"].(string) {
	case "audio":
		new_attachment._type = 1

	case "video":
		new_attachment._type = 2

	default:
		new_attachment._type = 0
	}

	if new_attachment._type > 0 {
		tmp := attachment["payload"].(map[string]any)
		new_attachment.url = tmp["url"].(string)
		new_attachment.token = tmp["token"].(string)
	}

	return *new_attachment
}

type Body struct {
	mid         string
	text        string
	attachments []Attachment
}

func ParseBody(body map[string]any) Body {
	new_body := new(Body)

	new_body.mid = body["mid"].(string)

	new_body.text = body["text"].(string)

	val, ok := body["attachments"]
	if ok {
		for _, value := range val.([]any) {
			new_body.attachments = append(new_body.attachments, ParseAttachment(value.(map[string]any)))
		}
	}

	return *new_body
}

type Message struct {
	body Body
}

func ParseMessage(message map[string]any) Message {
	new_msg := new(Message)
	new_msg.body = ParseBody(message["body"].(map[string]any))

	return *new_msg
}

func Handle(token string) {
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

	update := map[string]any{}
	var reader Reader = ReaderJson{&update}
	reader.Read(resp)

	for _, value := range update["updates"].([]any) {
		msg := ParseMessage(value.(map[string]any)["message"].(map[string]any))
		if len(msg.body.attachments) > 0 && msg.body.attachments[0]._type > 0 {
			GetAudio(token, msg.body.attachments[0].url)
		}
		log.Printf("%s:%s\n", msg.body.mid, msg.body.text)
	}
}

func GetAudio(token, url string) {
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

}

const SENDMESSAGE = "https://platform-api.max.ru/messages"

type link struct {
	Type string `json:"type"`
	Mid  string `json:"mid:`
}
type sendMessage struct {
	User_id int64  `json:"user_id,omitempty"`
	Chat_id int64  `json:"chat_id,omitempty"`
	Text    string `json:"text"`
	Link    link   `json:"link"`
}

func SendMessage(token string, message Message) {
	client := &http.Client{}

	req, err := http.NewRequest("POST", SENDMESSAGE, bytes.NewReader([]byte(" ")))
	if err != nil {
		log.Printf("%#v", err)
	}

	req.Header.Add("Authorization", token)

	resp, err := client.Do(req)
	if err != nil {
		log.Printf("%#v", err)
	}

	defer resp.Body.Close()
}

func main() {
	s := sendMessage{0, 100, "text", link{"type", "mid"}}
	data, err := json.Marshal(s)
	if err != nil {
		log.Printf("%s\n", err)
	}
	fmt.Printf("%s\n", string(data))
	Handle(os.Args[1])
}
