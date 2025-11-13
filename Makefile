all: api handler

TOKEN = "NULL"
handler:
	docker build -t handler -f DockerfileHandler .
# 	docker run -e TOKEN="$(TOKEN)" --net=host handler
	
api:
	docker build -t api -f DockerfileApi .
# 	docker run -p 8080:80 -d api 





