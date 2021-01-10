#ifndef COMMAND_H
#define COMMAND_H

#define MAX_MSG_LEN 512
#define MAX_CMD_DATA_LEN (MAX_MSG_LEN - sizeof(CommandType))
#define MAX_RPLY_DATA_LEN (MAX_MSG_LEN - sizeof(CommandType) - sizeof(ReplyResult))

enum class CommandType: uint16_t
{
    Unknown = 0,
    Ping = 1,
    AuthRequest = 1,
    SetValue = 2,
    GetValue = 3,
    Reboot = 4
};

enum class ReplyResult: uint16_t
{
    Ok = 0,
    Fail = 1,
    NotSupported = 2,
    Delayed = 3
};

enum class Detail: uint16_t
{
    CommandTopic = 0,
    DataTopic = 1,
    Serial = 2

};

struct Command
{
    CommandType cmd;
    char data[MAX_CMD_DATA_LEN];
    uint16_t len;

    Detail getDetail() {
        return *(reinterpret_cast<Detail*>(data));
    }

    const char* getDetailData() {
        return data + sizeof(Detail);
    }

    void setDetail(Detail det, const char* data, uint16_t len) {
        memcpy(this->data, &det, sizeof(det));
        memcpy(this->data, data, len < (MAX_CMD_DATA_LEN - sizeof(Detail)) ? len : (MAX_CMD_DATA_LEN - sizeof(Detail)));
    }
};

struct Reply
{
    CommandType cmd;
    ReplyResult result;
    char data[MAX_RPLY_DATA_LEN];
    uint16_t len;

    void set(const char* data, uint16_t len)
    {
        memcpy(this->data, data, len);
    }

    void set(Detail detail, const char* data, uint16_t len)
    {
        memcpy(this->data, &detail, sizeof(Detail));
        memcpy(this->data + sizeof(Detail), data, len);
    }
};

#endif // COMMAND_H