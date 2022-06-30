#include <iostream>

#include "DataNodeSession.h"
#include "Msg/DataNode/MsgChmod.h"
#include "Msg/DataNode/MsgChown.h"
#include "Msg/DataNode/MsgClose.h"
#include "Msg/DataNode/MsgLogin.h"
#include "Msg/DataNode/MsgLs.h"
#include "Msg/DataNode/MsgMkdir.h"
#include "Msg/DataNode/MsgOpen.h"
#include "Msg/DataNode/MsgPerm.h"
#include "Msg/DataNode/MsgRead.h"
#include "Msg/DataNode/MsgRm.h"
#include "Msg/DataNode/MsgWrite.h"
#include "Msg/DataNode/MsgGetFileDesc.h"
#include "Msg/DataNode/MsgOpenOffset.h"
#include "Msg/DataNode/MsgReadOffset.h"
#include "Msg/DataNode/MsgWriteOffset.h"
#include "Msg/DataNode/MsgTruncate.h"
#include "Msg/DataNode/MsgMv.h"

namespace efs {
	DataNodeSession::DataNodeSession(int32_t buffer_size,
		boost::asio::ip::tcp::socket socket,
		std::shared_ptr<DataNodeExecutor> p_executor)
		: SessionBase(buffer_size,
			std::move(socket),
			std::bind(&DataNodeSession::readMsgHandler, this),
			std::bind(&DataNodeSession::writeMsgHandler, this))
	{
		p_in_msg = nullptr;
		p_out_msg = nullptr;
		this->p_executor = p_executor;

		memset(p_msgs, 0, sizeof(p_msgs));
		p_msgs[MsgType::LOGIN] = std::make_shared<MsgLogin>();
		p_msgs[MsgType::LOGIN_RESP] = std::make_shared<MsgLoginResp>();
		p_msgs[MsgType::GET_FILE_DESC] = std::make_shared<MsgGetFileDesc>();
		p_msgs[MsgType::GET_FILE_DESC_RESP] = std::make_shared<MsgGetFileDescResp>();
		p_msgs[MsgType::OPEN] = std::make_shared<MsgOpen>();
		p_msgs[MsgType::OPEN_RESP] = std::make_shared<MsgOpenResp>();
		p_msgs[MsgType::CLOSE] = std::make_shared<MsgClose>();
		p_msgs[MsgType::CLOSE_RESP] = std::make_shared<MsgCloseResp>();
		p_msgs[MsgType::WRITE] = std::make_shared<MsgWrite>();
		p_msgs[MsgType::WRITE_RESP] = std::make_shared<MsgWriteResp>();
		p_msgs[MsgType::READ] = std::make_shared<MsgRead>();
		p_msgs[MsgType::READ_RESP] = std::make_shared<MsgReadResp>();
		p_msgs[MsgType::LS] = std::make_shared<MsgLs>();
		p_msgs[MsgType::LS_RESP] = std::make_shared<MsgLsResp>();
		p_msgs[MsgType::MKDIR] = std::make_shared<MsgOpen>();
		p_msgs[MsgType::MKDIR_RESP] = std::make_shared<MsgMkdirResp>();
		p_msgs[MsgType::RM] = std::make_shared<MsgRm>();
		p_msgs[MsgType::RM_RESP] = std::make_shared<MsgRmResp>();
		p_msgs[MsgType::MV] = std::make_shared<MsgMv>();
		p_msgs[MsgType::MV_RESP] = std::make_shared<MsgMvResp>();
		p_msgs[MsgType::CHOWN] = std::make_shared<MsgChown>();
		p_msgs[MsgType::CHOWN_RESP] = std::make_shared<MsgChownResp>();
		p_msgs[MsgType::CHMOD] = std::make_shared<MsgChmod>();
		p_msgs[MsgType::CHMOD_RESP] = std::make_shared<MsgChmodResp>();
		p_msgs[MsgType::PERM] = std::make_shared<MsgPerm>();
		p_msgs[MsgType::PERM_RESP] = std::make_shared<MsgPermResp>();
		p_msgs[MsgType::OPENOFFSET] = std::make_shared<MsgOpenOffset>();
		p_msgs[MsgType::OPENOFFSET_RESP] = std::make_shared<MsgOpenOffsetResp>();
		p_msgs[MsgType::WRITEOFFSET] = std::make_shared<MsgWriteOffset>();
		p_msgs[MsgType::WRITEOFFSET_RESP] = std::make_shared<MsgWriteOffsetResp>();
		p_msgs[MsgType::READOFFSET] = std::make_shared<MsgReadOffset>();
		p_msgs[MsgType::READOFFSET_RESP] = std::make_shared<MsgReadOffsetResp>();
		p_msgs[MsgType::TRUNCATE] = std::make_shared<MsgTruncate>();
		p_msgs[MsgType::TRUNCATE_RESP] = std::make_shared<MsgTruncateResp>();
	}

	DataNodeSession::~DataNodeSession()
	{
		for (auto it = open_files.begin(); it != open_files.end(); it++) {
			p_executor->close(it->second);
		}
	}

	ErrorCode DataNodeSession::readMsgHandler()
	{
		int32_t in_buffer_size = p_in_buffer->read_size();
		int32_t read_size = 0;
		if (in_buffer_size <= 4 || p_in_msg != nullptr) {
			return ErrorCode::NONE;
		}

#define RECV_IN_MSG(FUNC)                                                        \
    {                                                                            \
        int n = p_msg->deserialize(buf + read_size, in_buffer_size - read_size); \
        if (n <= 0) {                                                            \
            return ErrorCode::E_DESERIALIZE;                                     \
        }                                                                        \
        read_size += n;                                                          \
        p_in_msg = p_msg;                                                        \
        p_in_buffer->read_consume(read_size);                                    \
        FUNC();                                                                  \
    }

		if (p_in_msg == nullptr) {
			const char* buf = p_in_buffer->read_raw_buffer();
			int32_t msg_size = 0;
			read_size += serialize::deserialize(msg_size, buf + read_size, in_buffer_size - read_size);
			if (msg_size + 4 > in_buffer_size) {
				return ErrorCode::NONE;
			}

			switch ((buf + read_size)[0]) {
			case MsgType::LOGIN: {
				std::shared_ptr<MsgLogin> p_msg = std::static_pointer_cast<MsgLogin>(p_msgs[MsgType::LOGIN]);
				RECV_IN_MSG(login);
				break;
			}
			case MsgType::LS: {
				std::shared_ptr<MsgLs> p_msg = std::static_pointer_cast<MsgLs>(p_msgs[MsgType::LS]);
				RECV_IN_MSG(ls);
				break;
			}
			case MsgType::RM: {
				std::shared_ptr<MsgRm> p_msg = std::static_pointer_cast<MsgRm>(p_msgs[MsgType::RM]);
				RECV_IN_MSG(rm);
				break;
			}
			case MsgType::MV: {
				std::shared_ptr<MsgMv> p_msg = std::static_pointer_cast<MsgMv>(p_msgs[MsgType::MV]);
				RECV_IN_MSG(mv);
				break;
			}
			case MsgType::CHOWN: {
				std::shared_ptr<MsgChown> p_msg = std::static_pointer_cast<MsgChown>(p_msgs[MsgType::CHOWN]);
				RECV_IN_MSG(chown);
				break;
			}
			case MsgType::CHMOD: {
				std::shared_ptr<MsgChmod> p_msg = std::static_pointer_cast<MsgChmod>(p_msgs[MsgType::CHMOD]);
				RECV_IN_MSG(chmod);
				break;
			}
			case MsgType::MKDIR: {
				std::shared_ptr<MsgMkdir> p_msg = std::static_pointer_cast<MsgMkdir>(p_msgs[MsgType::MKDIR]);
				RECV_IN_MSG(mkdir);
				break;
			}
			case MsgType::OPEN: {
				std::shared_ptr<MsgOpen> p_msg = std::static_pointer_cast<MsgOpen>(p_msgs[MsgType::OPEN]);
				RECV_IN_MSG(open);
				break;
			}
			case MsgType::CLOSE: {
				std::shared_ptr<MsgClose> p_msg = std::static_pointer_cast<MsgClose>(p_msgs[MsgType::CLOSE]);
				RECV_IN_MSG(close);
				break;
			}
			case MsgType::READ: {
				std::shared_ptr<MsgRead> p_msg = std::static_pointer_cast<MsgRead>(p_msgs[MsgType::READ]);
				RECV_IN_MSG(read);
				break;
			}
			case MsgType::WRITE: {
				std::shared_ptr<MsgWrite> p_msg = std::static_pointer_cast<MsgWrite>(p_msgs[MsgType::WRITE]);
				RECV_IN_MSG(write);
				break;
			}
			case MsgType::PERM: {
				std::shared_ptr<MsgPerm> p_msg = std::static_pointer_cast<MsgPerm>(p_msgs[MsgType::PERM]);
				RECV_IN_MSG(perm);
				break;
			}
			case MsgType::GET_FILE_DESC: {
				std::shared_ptr<MsgGetFileDesc> p_msg = std::static_pointer_cast<MsgGetFileDesc>(p_msgs[MsgType::GET_FILE_DESC]);
				RECV_IN_MSG(getFileDesc);
				break;
			}
			case MsgType::OPENOFFSET: {
				std::shared_ptr<MsgOpenOffset> p_msg = std::static_pointer_cast<MsgOpenOffset>(p_msgs[MsgType::OPENOFFSET]);
				RECV_IN_MSG(openOffset);
				break;
			}
			case MsgType::WRITEOFFSET: {
				std::shared_ptr<MsgWriteOffset> p_msg = std::static_pointer_cast<MsgWriteOffset>(p_msgs[MsgType::WRITEOFFSET]);
				RECV_IN_MSG(writeOffset);
				break;
			}
			case MsgType::READOFFSET: {
				std::shared_ptr<MsgReadOffset> p_msg = std::static_pointer_cast<MsgReadOffset>(p_msgs[MsgType::READOFFSET]);
				RECV_IN_MSG(readOffset);
				break;
			}
			case MsgType::TRUNCATE: {
				std::shared_ptr<MsgTruncate> p_msg = std::static_pointer_cast<MsgTruncate>(p_msgs[MsgType::TRUNCATE]);
				RECV_IN_MSG(truncate);
				break;
			}
			default:
				return ErrorCode::E_MSG_UNKNOWN;
			}
		}

#define SEND_OUT_MSG                                                                                      \
    {                                                                                                     \
        int32_t msg_size = p_msg->size();                                                                 \
        int32_t out_buffer_size = p_out_buffer->write_size();                                             \
        char* buf = p_out_buffer->write_raw_buffer();                                                     \
        if (msg_size + 4 <= out_buffer_size) {                                                            \
            int32_t write_size = 0;                                                                       \
            write_size += serialize::serialize(msg_size, buf + write_size, out_buffer_size - write_size); \
            write_size += p_msg->serialize(buf + write_size, out_buffer_size - write_size);               \
            p_out_buffer->write_consume(write_size);                                                      \
            p_out_msg = nullptr;                                                                          \
            p_in_msg = nullptr;                                                                           \
        }                                                                                                 \
    }

		if (p_out_msg) {
			switch (p_out_msg->msg_type) {
			case MsgType::LOGIN_RESP: {
				std::shared_ptr<MsgLoginResp> p_msg = std::static_pointer_cast<MsgLoginResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::LS_RESP: {
				std::shared_ptr<MsgLsResp> p_msg = std::static_pointer_cast<MsgLsResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::RM_RESP: {
				std::shared_ptr<MsgRmResp> p_msg = std::static_pointer_cast<MsgRmResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::MV_RESP: {
				std::shared_ptr<MsgMvResp> p_msg = std::static_pointer_cast<MsgMvResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::CHOWN_RESP: {
				std::shared_ptr<MsgChownResp> p_msg = std::static_pointer_cast<MsgChownResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::CHMOD_RESP: {
				std::shared_ptr<MsgChmodResp> p_msg = std::static_pointer_cast<MsgChmodResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::MKDIR_RESP: {
				std::shared_ptr<MsgMkdirResp> p_msg = std::static_pointer_cast<MsgMkdirResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::OPEN_RESP: {
				std::shared_ptr<MsgOpenResp> p_msg = std::static_pointer_cast<MsgOpenResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::CLOSE_RESP: {
				std::shared_ptr<MsgCloseResp> p_msg = std::static_pointer_cast<MsgCloseResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::READ_RESP: {
				std::shared_ptr<MsgReadResp> p_msg = std::static_pointer_cast<MsgReadResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::WRITE_RESP: {
				std::shared_ptr<MsgWriteResp> p_msg = std::static_pointer_cast<MsgWriteResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::PERM_RESP: {
				std::shared_ptr<MsgPermResp> p_msg = std::static_pointer_cast<MsgPermResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::GET_FILE_DESC_RESP: {
				std::shared_ptr<MsgGetFileDescResp> p_msg = std::static_pointer_cast<MsgGetFileDescResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::OPENOFFSET_RESP: {
				std::shared_ptr<MsgOpenOffsetResp> p_msg = std::static_pointer_cast<MsgOpenOffsetResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::READOFFSET_RESP: {
				std::shared_ptr<MsgReadOffsetResp> p_msg = std::static_pointer_cast<MsgReadOffsetResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::WRITEOFFSET_RESP: {
				std::shared_ptr<MsgWriteOffsetResp> p_msg = std::static_pointer_cast<MsgWriteOffsetResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			case MsgType::TRUNCATE_RESP: {
				std::shared_ptr<MsgTruncateResp> p_msg = std::static_pointer_cast<MsgTruncateResp>(p_out_msg);
				SEND_OUT_MSG;
				break;
			}
			default:
				return ErrorCode::E_MSG_UNKNOWN;
			}
		}

		return ErrorCode::NONE;
	}

	ErrorCode DataNodeSession::writeMsgHandler()
	{
		return ErrorCode::NONE;
	}

}