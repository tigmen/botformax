# maxapi.go
Предоставляет инструменты для работы с API месседжера MAX
### Структуры
Данные для загрузки вложений:
```Go
type Payload struct {
    Url   string
        Token string
}
```
Вложение:
```Go
type Attachment struct {
    Type    string
        Payload Payload
}
```
Тело сообщения:
```Go
type Body struct {
    Mid         string
        Text        string
        Attachments []Attachment
}
```
Полуатель сообщения:
```Go
type Recipient struct {
    Gohat_id int64
        User_id int64
}
```
Отправитель сообщения:
```Go
type Sender struct {
    User_id    int64
        First_name string
        Last_name  string
        Username   string
}
```
Сообщение:
```Go
type Message struct {
    Body      Body
        Recipient Recipient
        Sender    Sender
}
```
Обновление с сервера:
```Go
type Update struct {
    Message Message
}
```
Группа обновлений:
```Go
type Updates struct {
    Updates []Update
}
```
Ссылка на сообщение для ответа:
```Go
type link struct {
    Type string `json:"type"`
        Mid  string `json:"mid"`
}
```
Сообщение для отправки:
```
type sendMessage struct {
    Chat_id int64  `json:"-"`
        Text    string `json:"text,omitempty"`
        Link    link   `json:"link"`
}
```
### Функции

```Go
func GetUpdates(token string) Updates
```
Если происходит событие за t < timeout из макса, то возвращает это событие
```Go
func GetAudio(token, url string) []byte
```
Загружает по ссылки аудио-файл с сервера max. Возвращает массив байт загруженного файла
```Go
func SendMessage(token string, message sendMessage)
```
Отправляет сообщение

# aiapi.go
### Структуры
Отправляеммый аудио-файл
```Go
type sendFile struct {
    File_name  string `json:"file_name"`
        File_bytes string `json:"file_bytes"`
}
```
### Функции
```Go
func DecodeFile() string
```
Отправляет запрос с айдиофайлом в микросервис с моделью и возвращает расшифрованный текст

