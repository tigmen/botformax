package main

import (
	"log"
	"os"
)

func main() {
	token := os.Args[1]
	for _, update := range GetUpdates(token).Updates {
		message := update.Message

		log.Printf("%#v\n", message)
		for _, attachment := range message.Body.Attachments {
			switch attachment.Type {
			case "audio":
				GetAudio(token, attachment.Payload.Url)
				out := DecodeFile()

				resp := sendMessage{Chat_id: message.Recipient.Chat_id, Text: out, Link: link{Type: "reply", Mid: message.Body.Mid}}
				SendMessage(token, resp)

			default:
			}

		}
	}
}
