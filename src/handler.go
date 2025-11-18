package main

import (
	"fmt"
	"log"
	"os"
	"sync"
)

const N_STREAMS = 4

type stream struct {
	Mutex sync.Mutex
	Busy  bool
}

type update_queue struct {
	queue []Update
	Mutex sync.Mutex
}

func handle(streams *[N_STREAMS]stream, token string, queue *update_queue, done chan bool, wg *sync.WaitGroup) {
	for len(done) == 0 {

		if len(queue.queue) > 0 {
			queue.Mutex.Lock()
			log.Printf("enter file i : %v", queue)
			message := queue.queue[0].Message
			queue.queue = queue.queue[1:]
			queue.Mutex.Unlock()

			for _, attachment := range message.Body.Attachments {

				wg.Go(func() {
					for i := range streams {
						if !streams[i].Busy {
							streams[i].Busy = true
							log.Printf("enter file %d : %v", i, queue)
							streams[i].Mutex.Lock()

							filename := fmt.Sprintf("%s%d%s", "audio", i, ".ogg")
							GetAudio(token, attachment.Payload.Url, filename)
							out := DecodeFile(filename)

							resp := sendMessage{Chat_id: message.Recipient.Chat_id, Text: out, Link: link{Type: "reply", Mid: message.Body.Mid}}
							SendMessage(token, resp)

							streams[i].Mutex.Unlock()
							log.Printf("exit file %d with %v", i, resp)
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
	var wg sync.WaitGroup

	var update_queue update_queue
	update_queue.queue = make([]Update, 0)
	update_queue.Mutex = sync.Mutex{}

	var lines [N_STREAMS]stream
	var marker int64 = 0

	wg.Add(1)
	go handle(&lines, token, &update_queue, done, &wg)

	for {
		log.Printf("get updates with marker: %d", marker)
		for _, update := range GetUpdates(token, &marker).Updates {
			message := update.Message

			for _, attachment := range message.Body.Attachments {

				switch attachment.Type {
				case "audio":
					update_queue.Mutex.Lock()
					update_queue.queue = append(update_queue.queue, update)
					update_queue.Mutex.Unlock()

				default:
					if message.Recipient.User_id != 0 {
						SendMessage(token, sendMessage{Chat_id: message.Recipient.Chat_id, Text: "Здравствуйте, запишите мне голосовое сообщение и я расшифрую его.", Link: link{Type: "reply", Mid: message.Body.Mid}})
					}
				}
			}
			if len(message.Body.Attachments) == 0 {
				if message.Recipient.User_id != 0 {
					SendMessage(token, sendMessage{Chat_id: message.Recipient.Chat_id, Text: "Здравствуйте, запишите мне голосовое сообщение и я расшифрую его.", Link: link{Type: "reply", Mid: message.Body.Mid}})
				}
			}
		}
	}
}
