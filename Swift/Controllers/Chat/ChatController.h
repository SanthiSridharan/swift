/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ChatController_H
#define SWIFTEN_ChatController_H

#include "Swift/Controllers/Chat/ChatControllerBase.h"

namespace Swift {
	class AvatarManager;
	class ChatStateNotifier;
	class ChatStateMessageSender;
	class ChatStateTracker;
	class NickResolver;
	class ChatController : public ChatControllerBase {
		public:
			ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory);
			virtual ~ChatController();
			virtual void setToJID(const JID& jid);
			virtual void setEnabled(bool enabled);

		private:
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
			String getStatusChangeString(boost::shared_ptr<Presence> presence);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			void postSendMessage(const String &body, boost::shared_ptr<Stanza> sentStanza);
			void preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent);
			void preSendMessageRequest(boost::shared_ptr<Message>);
			String senderDisplayNameFromMessage(const JID& from);
			virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message>) const;
			void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
			void dayTicked() {lastWasPresence_ = false;}

		private:
			NickResolver* nickResolver_;
			JID contact_;
			ChatStateNotifier* chatStateNotifier_;
			ChatStateMessageSender* chatStateMessageSender_;
			ChatStateTracker* chatStateTracker_;
			bool isInMUC_;
			bool lastWasPresence_;
			String lastStatusChangeString_;
			std::map<boost::shared_ptr<Stanza>, String> unackedStanzas_;
	};
}
#endif

