package main

import (
	"fmt"
	"log"
	"os"
)

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
