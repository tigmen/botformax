package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strings"
)

type Reader interface {
	Read(*http.Response)
}

type ReaderToFile struct {
	filepath string
}

func (reader ReaderToFile) Read(resp *http.Response) {
	out, err := os.Create(reader.filepath)
	if err != nil {
	}
	defer out.Close()

	_, err = io.Copy(out, resp.Body)
	if err != nil {
	}
}

type ReaderToString struct {
	str *string
}

func (reader ReaderToString) Read(resp *http.Response) {
	out, err := io.ReadAll(resp.Body)
	if err != nil {
	}
	if out == nil {
		*(reader.str) = "nil"
	}
	*(reader.str) = string(out)
}

const GETHOSTURL = "https://ipv4.icanhazip.com"

func GetHost() string {
	var out string

	resp, err := http.Get(GETHOSTURL)
	if err != nil {
	}
	defer resp.Body.Close()

	var reader Reader = ReaderToString{&out}
	reader.Read(resp)

	return out
}

func main() {
	url := "https://api.telegram.org/bot7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w/setWebhook"
	// url := "https://ipv4.icanhazip.com"
	// filepath := "out"
	fmt.Printf("%s", GetHost())

	resp, err := http.NewRequest("GET", url, strings.NewReader("{\"url\" : \"https://62.109.1.123:443\"}"))

	if resp.Response != nil {
		var out string
		var reader Reader = ReaderToString{&out}
		reader.Read(resp.Response)

		fmt.Printf("%s\n", out)
	}

	http.HandleFunc("/hello", HelloServer)
	err = http.ListenAndServeTLS(":443", "keys/server.crt", "keys/server.key", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
func HelloServer(w http.ResponseWriter, req *http.Request) {
	defer req.Body.Close()

	w.Header().Set("Content-Type", "text/plain")
	w.Write([]byte("This is an example server.\n"))
	if req.Response != nil {
		var out string
		var reader Reader = ReaderToString{&out}
		reader.Read(req.Response)

		fmt.Printf("%s", out)
	}
}
