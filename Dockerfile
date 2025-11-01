FROM golang:alpine AS builder

RUN apk update --no-cache && apk add --no-cache make

WORKDIR /build
COPY ./src ./src
COPY ./keys ./keys
COPY ./Makefile ./Makefile

RUN make

FROM alpine

WORKDIR /build
COPY --from=builder /build/build/index ./index
COPY --from=builder /build/keys ./keys

CMD ["./index"]
