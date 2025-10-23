#include <stdint.h>

typedef int64_t bigint;
typedef bigint user_id;
typedef bigint chat_id;
typedef bigint time;

typedef int32_t p_count;

typedef char* message_id;

typedef struct {
	user_id id;
	char* first_name;
	char* last_name;
	char* user_name;
	char is_bot;
	time last_activity_time;
} User;

typedef struct {
	chat_id id;
	enum CHAT_TYPE chat_type;
	enum CHAT_STATUS chat_status;
	char* title;
	img icon;
	time last_event_time;
	p_count p_cnt;
	char is_public;
} Chat;

typedef struct {
	enum ATTACHMENT_TYPE type;
	/* пока пусто  */
}

typedef struct {
	message_id id;
	int64_t seq;
	char* text;
	unsigned attachments_n;
	Attachment* attachments;
} MessageBody;

typedef struct {
	User sender;
	Recepient recepient;
	time timestamp;
	MessageBody body;
} Message;

typedef struct {
	int i;
	/*  */
} ChatAdminPermission;
typedef struct {
	User user;
	char is_owner;
	char is_admin;
	time join_time;
	ChatAdminPermission permission[1];
} ChatMember;

User* get_me(void);
ChatList* get_chats(int* count, bigint* marker);
Chat* get_chat_by_url(char* chatLink);
Chat* get_chat_by_id(chat_id id);
Chat* patch_chat(chat_id id);
int delete_chat(chat_id id);
int post_chat_action(chat_id id);
Message* get_chat_pin(chat_id id);
int put_message_pin(message_id id);
int delete_chat_pin(message_id id);
ChatMember* is_member(void);
int delete_bot_from_chat(chat_id id);
ChatMember** get_admins(chat_id id, unsigned* size);
int set_admins(chat_id id, ChatMemmbers** chat_members, unsigned* size);
int delete_admin(chat_is id, user_id user);
ChatMember** get_members(chat_id id,
                         user_id* users,
                         unsigned size,
                         bigint* marker);
int add_members(chat_id id, user_id* users, unsigned size);
int delete member(chat_id id, user_id user, char block);

Message** get_messages_by_time(chat_id id, time from, time to, int* count);
Message** get_messages_by_id(chat_id id, message_id* messages, int count);
Message** send_message_to_user(user_id, Message* msg);
Message** send_message_to_chat(chat_id, Message* msg);
int edit_message(message_id id, Message* msg);
int delete_message(message_id id);
Message* get_message(message_id id);

/* Методы для работы c WebHook */
/* Возможно когда-то будут существовать */
