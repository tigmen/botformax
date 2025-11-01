package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
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
	// url := "https://api.telegram.org/bot7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w/getMe"
	url := "https://ipv4.icanhazip.com"
	// filepath := "out"
	resp, err := http.Get(url)
	if err != nil {
	}
	defer resp.Body.Close()

	fmt.Printf("%s", GetHost())

	http.HandleFunc("/hello", HelloServer)
	err = http.ListenAndServeTLS(":443", "keys/server.crt", "keys/server.key", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
func HelloServer(w http.ResponseWriter, req *http.Request) {
	w.Header().Set("Content-Type", "text/plain")
	w.Write([]byte("This is an example server.\n"))
	fmt.Fprintf(w, "This is an example server.\n")
}
