all: src/client.go go.mod
	go mod tidy
	go build -o build/index src/client.go

go.mod:
	go mod init main
