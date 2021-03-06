/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

#include <openssl/ssl.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/TLSContext.h>

namespace std {
    template<>
    class default_delete<SSL_CTX> {
    public:
        void operator()(SSL_CTX *ptr) {
            SSL_CTX_free(ptr);
        }
    };

    template<>
    class default_delete<SSL> {
    public:
        void operator()(SSL *ptr) {
            SSL_free(ptr);
        }
    };
}

namespace Swift {
    class OpenSSLContext : public TLSContext, boost::noncopyable {
        public:
            OpenSSLContext();
            virtual ~OpenSSLContext() override final;

            void connect() override final;
            bool setClientCertificate(CertificateWithKey::ref cert) override final;

            void handleDataFromNetwork(const SafeByteArray&) override final;
            void handleDataFromApplication(const SafeByteArray&) override final;

            std::vector<Certificate::ref> getPeerCertificateChain() const override final;
            std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const override final;

            virtual ByteArray getFinishMessage() const override final;

        private:
            static void ensureLibraryInitialized();

            static CertificateVerificationError::Type getVerificationErrorTypeForResult(int);

            void doConnect();
            void sendPendingDataToNetwork();
            void sendPendingDataToApplication();

        private:
            enum class State { Start, Connecting, Connected, Error };

            State state_;
            std::unique_ptr<SSL_CTX> context_;
            std::unique_ptr<SSL> handle_;
            BIO* readBIO_ = nullptr;
            BIO* writeBIO_ = nullptr;
    };
}
