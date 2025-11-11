all: build_handler requirements
	pip install -r requirements

go.mod:
	go mod init main

go_resources = src/client.go src/readers.go src/maxapi.go src/aiapi.go
out = build/index
build_handler: $(go_resources) go.mod
	go mod tidy
	go build -o $(out) $(go_resources)

