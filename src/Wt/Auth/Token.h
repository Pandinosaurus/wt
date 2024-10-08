// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_AUTH_TOKEN_H_
#define WT_AUTH_TOKEN_H_

#include <Wt/WDateTime.h>

#include <Wt/Auth/WAuthGlobal.h>

namespace Wt {
  namespace Auth {

/*! \class Token Wt/Auth/Token.h
 *  \brief An authentication token hash.
 *
 * An authentication token is a surrogate for identification or
 * authentication. When a random authentication token is generated,
 * \if cpp
 * e.g. using WRandom::generateId()
 * \endif
 * it is a good practice to hash it
 * using a cryptographic hash function, and only save this hash in the
 * session or database for later verification. This avoids that a
 * compromised database would leak all the authentication tokens.
 *
 * The token can be used for multiple purposes, denoted by the token's
 * name in the browser. In both cases it is used for "remember-me"
 * functionality. For regular authentication this is for the normal
 * username/password combination login. For MFA authentication this
 * is used in a similar fashion to remember the MFA verification. That
 * means a User will not have to submit a TOTP code each time they log
 * in. But only as often as the developer desires (managed by
 * AuthService::setMfaTokenValidity()).
 *
 * \sa User::addAuthToken()
 * \sa User::setEmailToken()
 *
 * \ingroup auth
 */
class WT_API Token
{
public:
  /*! \brief Default constructor.
   *
   * Creates an empty token.
   */
  Token();

  Token(const std::string& hash, const WDateTime& expirationTime);
  /*! \brief Constructor.
   */
  Token(const std::string& hash, const WDateTime& expirationTime, const std::string &purpose, const std::string &scope, const std::string &redirectUri);

  /*! \brief Returns whether the token is empty.
   *
   * An empty token is default constructed.
   */
  bool empty() const { return hash_.empty(); }

  /*! \brief Returns the hash.
   */
  const std::string& hash() const { return hash_; }

  /*! \brief Returns the expiration time.
   */
  const WDateTime& expirationTime() const { return expirationTime_; }

private:
  std::string hash_;
  WDateTime expirationTime_;
};

  }
}

#endif // WT_AUTH_TOKEN_H_
