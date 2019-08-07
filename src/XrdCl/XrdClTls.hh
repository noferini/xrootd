//------------------------------------------------------------------------------
// Copyright (c) 2011-2012 by European Organization for Nuclear Research (CERN)
// Author: Michal Simon <simonm@cern.ch>
//------------------------------------------------------------------------------
// XRootD is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// XRootD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with XRootD.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#ifndef __XRD_CL_TLS_HH__
#define __XRD_CL_TLS_HH__

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include "XrdTls/XrdTlsSocket.hh"

#include "XrdCl/XrdClStatus.hh"
#include "XrdCl/XrdClAsyncSocketHandler.hh"

namespace XrdCl
{
  class Socket;

  //----------------------------------------------------------------------------
  //! TLS layer for socket connection
  //----------------------------------------------------------------------------
  class Tls
  {
    public:

      //------------------------------------------------------------------------
      //! Constructor - creates async TLS layer for given socker file descriptor
      //------------------------------------------------------------------------
      Tls( Socket *socket, AsyncSocketHandler *socketHandler );

      //------------------------------------------------------------------------
      //! Destructor
      //------------------------------------------------------------------------
      ~Tls()
      {
      }

      //------------------------------------------------------------------------
      //! Read through the TLS layer from the socket
      //! If necessary, will establish a TLS/SSL session.
      //------------------------------------------------------------------------
      Status Read( char *buffer, size_t size, int &bytesRead );

      //------------------------------------------------------------------------
      //! Write through the TLS layer to the socket
      //! If necessary, will establish a TLS/SSL session.
      //------------------------------------------------------------------------
      Status Send( const char *buffer, size_t size, int &bytesWritten );

      //------------------------------------------------------------------------
      //! Map:
      //!     * in case the TLS layer requested reads on writes map
      //!       ReadyToWrite to ReadyToRead
      //!     * in case the TLS layer requested writes on reads map
      //!       ReadyToRead to ReadyToWrite
      //------------------------------------------------------------------------
      uint8_t MapEvent( uint8_t event );

    private:

      //------------------------------------------------------------------------
      //! Flags to indicate what is the TLS hand-shake revert state
      //!
      //! - None        : there is no revert state
      //! - ReadOnWrite : OnRead routines will be called on write event due to
      //!                 TLS handshake
      //! - WriteOnRead : OnWrite routines will be called on read event due to
      //!                 TLS handshake
      //------------------------------------------------------------------------
      enum TlsHSRevert{ None, ReadOnWrite, WriteOnRead };

      //------------------------------------------------------------------------
      //! Translate OPEN SSL error code into XRootD Status
      //------------------------------------------------------------------------
      Status ToStatus( int rc );

      //------------------------------------------------------------------------
      //! The underlying vanilla socket
      //------------------------------------------------------------------------
      Socket                       *pSocket;

      //------------------------------------------------------------------------
      //! The TSL I/O wrapper over socket
      //------------------------------------------------------------------------
      std::unique_ptr<XrdTlsSocket> pTls;

      //------------------------------------------------------------------------
      // In case during TLS hand-shake WantRead has been returned on write or
      // WantWrite has been returned on read we need to flip the following events.
      //
      // None        : all events should be processed normally
      // ReadOnWrite : on write event the OnRead routines should be called
      // WriteOnRead : on read event the OnWrite routines should be called
      //------------------------------------------------------------------------
      TlsHSRevert                   pTlsHSRevert;

      //------------------------------------------------------------------------
      //! Socket handler (for enabling/disabling write notification)
      //------------------------------------------------------------------------
      AsyncSocketHandler           *pSocketHandler;
  };
}

#endif // __XRD_CL_TLS_HH__
