/*
 * This file is part of the Nice GLib ICE library.
 *
 * (C) 2007 Nokia Corporation. All rights reserved.
 *  Contact: Rémi Denis-Courmont
 * COPYRIGHT (C) 1986 Gary S. Brown 
 *  See documentation of the function crc32() below.
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Nice GLib ICE library.
 *
 * The Initial Developers of the Original Code are Collabora Ltd and Nokia
 * Corporation. All Rights Reserved.
 *
 * Contributors:
 *   Rémi Denis-Courmont, Nokia
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
 * case the provisions of LGPL are applicable instead of those above. If you
 * wish to allow use of your version of this file only under the terms of the
 * LGPL and not to allow others to use your version of this file under the
 * MPL, indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL. If you do
 * not delete the provisions above, a recipient may use your version of this
 * file under either the MPL or the LGPL.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h> /* htons() */
#endif

#include <string.h>
#include <stdlib.h>

#include "stuncrc32.h"
#include "stunmessage.h"

uint32_t stun_fingerprint (const uint8_t *msg, size_t len)
{
  crc_data data[3];
  uint16_t fakelen = htons (len - 20u);

  // assert (len >= 28u);

  data[0].buf = (void *)msg;
  data[0].len = 2;
  data[1].buf = (uint8_t *)&fakelen;
  data[1].len = 2;
  data[2].buf = (void *)(msg + 4);
  /* first 4 bytes done, last 8 bytes not summed */
  data[2].len = len - 12u;

  return htonl (crc32 (data, 3) ^ 0x5354554e);
}

bool stun_has_cookie (const StunMessage *msg)
{
  StunTransactionId id;
  uint32_t cookie = htonl (STUN_MAGIC_COOKIE);
  stun_message_id (msg, id);
  return memcmp (id, &cookie, sizeof (cookie)) == 0;
}


StunMessageReturn stun_message_append_software (StunMessage *msg)
{
  static const char software[] = PACKAGE_STRING;
  // assert (strlen (software) < 128);

  return stun_message_append_string (msg, STUN_ATTRIBUTE_SOFTWARE, software);
}
