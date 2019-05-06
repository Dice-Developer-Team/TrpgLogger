#pragma once

#include "CQconstant.h"

namespace CQ
{
	// 事件基类
	struct EVE
	{
		//不对消息做任何动作
		//如果之前拦截了消息,这里将重新放行本条消息
		void message_ignore();
		//拦截本条消息
		void message_block();

		int _EVEret = Msg_Ignored;

		virtual ~EVE()
		{
		}
	};
}
