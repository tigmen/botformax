all: src/client.go go.mod requirements
	go mod tidy
	go build -o build/index.exe src/client.go
	pip install -r requirements

go.mod:
	go mod init main
