package main

import (
	"fmt"
	"log"
	"os"
	"sync"
)

const N_STREAMS = 8

type stream struct {
	Mutex sync.Mutex
	Busy  bool
}

func handle(streams *[N_STREAMS]stream, token string, queue *[]Update, done chan bool, wg *sync.WaitGroup) {
	for len(done) == 0 {

		if len(*queue) > 0 {
			message := (*queue)[0].Message
			*queue = (*queue)[1:]

			log.Printf("%#v\n", message)
			for _, attachment := range message.Body.Attachments {

				wg.Go(func() {
					for i := range streams {
						if !streams[i].Busy {
							streams[i].Busy = true
							streams[i].Mutex.Lock()

							filename := fmt.Sprintf("%s%d%s", "file/audio", i, ".ogg")
							GetAudio(token, attachment.Payload.Url, filename)
							out := DecodeFile(filename)

							resp := sendMessage{Chat_id: message.Recipient.Chat_id, Text: out, Link: link{Type: "reply", Mid: message.Body.Mid}}
							SendMessage(token, resp)

							streams[i].Mutex.Unlock()
							streams[i].Busy = false
							break
						}
					}
				})

			}
		}
	}
}

func main() {
	token := os.Args[1]
	done := make(chan bool, 1)
	var lines [N_STREAMS]stream
	var marker int64 = 0
	var update_queue []Update
	var wg sync.WaitGroup

	wg.Go(func() {
		var n int
		fmt.Scanln(&n)
		done <- true
		wg.Done()
	})

	wg.Add(1)
	go handle(&lines, token, &update_queue, done, &wg)

	for len(done) == 0 {
		for _, update := range GetUpdates(token, &marker).Updates {
			message := update.Message

			log.Printf("%#v\n", message)
			for _, attachment := range message.Body.Attachments {

				switch attachment.Type {
				case "audio":
					update_queue = append(update_queue, update)

				default:
				}
			}
		}
	}

	<-done
	wg.Wait()
}
